#include <QtMath>
#include <QProgressBar>

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

    this->stopped = false;
    this->calculate(number);
    this->stopped = true;

    this->ui->pushButtonStart->setEnabled(true);
    this->ui->pushButtonStop->setEnabled(false);
    this->ui->statusBar->showMessage(tr("Ready"));
}

long long MainWindow::calculate(long long number)
{
    if ( number < 4 || number == 5 ) return 0;
    return number % 2 == 0 ? calculateEvenGoldbach(number) : calculateOddGoldbach(number);
}

long long MainWindow::calculateEvenGoldbach(long long number)
{
    long long results = 0;
    for ( long long a = 2; a < number; ++a )
    {
        if ( ! isPrime(a) ) continue;
        long long b = number - a;
        if ( b >= a && isPrime(b) )
            this->appendResult( tr("%1: %2 + %3").arg(++results).arg(a).arg(b) );

        // Update the progress bar
        this->updateProgressBar((a + 1) * 100 / number);
        // If user cancelled, stop calculations
        if ( this->isStopped() )
            return results;
        QApplication::processEvents();
    }
    return results;
}

long long MainWindow::calculateOddGoldbach(long long number)
{
    long long results = 0;
    for ( long long a = 2; a < number; ++a )
    {
        if ( ! isPrime(a) ) continue;
        for ( long long b = a; b < number; ++b )
        {
            if ( ! isPrime(b) ) continue;
            long long c = number - a - b;
            if ( c >= b && isPrime(c) )
                this->appendResult( tr("%1: %2 + %3 + %4").arg(++results).arg(a).arg(b).arg(c) );

            // Update the progress bar
            this->updateProgressBar((a + 1) * 100 / number);
            // If user cancelled, stop calculations
            if ( this->isStopped() )
                return results;
            QApplication::processEvents();
        }
    }
    return results;
}

bool MainWindow::isPrime(long long number)
{
    if ( number < 2 ) return false;

    for ( long long i = 2, last = qSqrt(number); i <= last; ++i )
        if ( number % i == 0 )
            return false;

    return true;
}
