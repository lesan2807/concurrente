#include <QtMath>
#include <QProgressBar>
#include <QTime>
#include <iostream>

#include "goldbachworker.h"
#include "goldbachcalculator.h"
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

void MainWindow::on_lineEditNumber_textEdited(const QString &arg1)
{
    (void)arg1;
    //Q_UNUSED(arg1);
    bool enable = this->ui->lineEditNumber->text().trimmed().length() > 0;
    this->ui->pushButtonStart->setEnabled( enable );
}

void MainWindow::on_pushButtonStop_clicked()
{
    Q_ASSERT( this->goldbachCalculator ); // checkear que tenga un thread, programación defensiva
    this->goldbachCalculator->stop();
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

    this->goldbachCalculator = new GoldbachCalculator(this);
    this->ui->listViewResults->setModel( goldbachCalculator );
    this->connect(this->goldbachCalculator, &GoldbachCalculator::calculationDone, this, &MainWindow::calculationDone);
    this->connect(this->goldbachCalculator, &GoldbachCalculator::updateProgressBar, this, &MainWindow::updateProgressBar);

    this->ui->statusBar->showMessage(tr("Calculating..."));
    this->time.start();
    this->goldbachCalculator->calculate(number);
}

void MainWindow::calculationDone(int index, long long sumCount)
{
    Q_UNUSED(index);
    double seconds = this->time.elapsed() / 1000.0;
    this->ui->pushButtonStart->setEnabled(true);
    this->ui->pushButtonStop->setEnabled(false);
    this->ui->statusBar->showMessage(tr("%1 sums found in %2 seconds").arg(sumCount).arg(seconds));
}

void MainWindow::updateProgressBar(int percent)
{
    this->progressBar->setValue(percent);
}
