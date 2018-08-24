#include <QtMath>
#include <QProgressBar>
#include <QTime>
#include <iostream>

#include "goldbachworker.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->ui->statusBar->showMessage(tr("Ready"));

    this->progressBar = new QProgressBar();
    this->ui->statusBar->addPermanentWidget(this->progressBar);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendResult(const QString &result)
{
    this->ui->plainTextEditResults->appendPlainText( result );
}

void MainWindow::updateProgressBar(int percent)
{
    this->progressBar->setValue(percent);
}

void MainWindow::on_lineEditNumber_textEdited(const QString &arg1)
{
    (void)arg1;
    //Q_UNUSED(arg1);
    bool enable = this->ui->lineEditNumber->text().trimmed().length() > 0;
    this->ui->pushButtonStart->setEnabled( enable );
}


void MainWindow::on_pushButtonStop_clicked()
{
    this->stopped = true;
    Q_ASSERT( this->goldbachWorker ); // checkear que tenga un thread, programación defensiva

    this->goldbachWorker->requestInterruption();
}



void MainWindow::on_pushButtonStart_clicked()
{
    const QString& text = this->ui->lineEditNumber->text();
    bool valid = true;
    long long number = text.toLongLong(&valid);

    if ( valid )
    {
        this->startCalculation(number);
    }
    else
    {
        this->ui->statusBar->showMessage( tr("Invalid number: %1").arg(text) );
    }
}

void MainWindow::startCalculation(long long number)
{
    this->ui->pushButtonStart->setEnabled(false);
    this->ui->pushButtonStop->setEnabled(true);
    this->ui->plainTextEditResults->clear();
    this->ui->statusBar->showMessage(tr("Calculating..."));


    this->time.start();
    this->stopped = false;

    QVector<GoldbachWorker*> workers;

    int ideal = QThread::idealThreadCount() - 1;
    for ( int current = 0; current < ideal; ++current  )
    {
        GoldbachWorker* worker = new GoldbachWorker(number, current, ideal, this);
        // Hacer las conexiones para reaccionar cuando el worker:
        // (1) encuentra una suma, (2) incrementa el porcentaje, (3) termina
        this->connect(worker, &GoldbachWorker::sumFound, this, &MainWindow::appendResult);
        //this->connect(worker, &GoldbachWorker::percent, this, &MainWindow::updateProgressBar);
        this->connect( worker, &GoldbachWorker::finished, this, &MainWindow::workerFinished );
        workers.append(worker);
    }

    for(int index = 0; index < workers.size(); ++index)
    {
        workers[index]->start();
        double seconds = this->time.elapsed() / 1000.0;
        std::cerr << index << ": " << seconds << std::endl;
    }

}

void MainWindow::workerFinished()
{

    long long sumCount = 0; // = this->calculate(number);
    this->stopped = true;
    double seconds = this->time.elapsed() / 1000.0;
    this->ui->pushButtonStart->setEnabled(true);
    this->ui->pushButtonStop->setEnabled(false);
    this->ui->statusBar->showMessage(tr("%1 sums found in %2 seconds").arg(sumCount).arg(seconds));

    this->goldbachWorker->deleteLater(); // evitar eliminar el obj antes de que se ejec los eventos
}

