#include "goldbachcalculator.h"
#include "goldbachworker.h"
#include <iostream>
#include <algorithm>
#include <QtMath>
#include <QTimer>

GoldbachCalculator::GoldbachCalculator(QObject *parent)
    : QAbstractListModel(parent)
{

}

void GoldbachCalculator::calculate(long long number)
{

    this->beginResetModel();
    int ideal = QThread::idealThreadCount() - 1;
    this->results.resize(ideal);

    for (int current = 0; current < ideal; ++current)
    {
        GoldbachWorker* worker = new GoldbachWorker(number, current, ideal, this->results[current], this);
        this->workers.append(worker);
        this->connect(this->workers[current], &GoldbachWorker::calculationDone, this, &GoldbachCalculator::workerDone);
        this->connect(this->workers[current], &GoldbachWorker::percent, this, &GoldbachCalculator::percent);
        //this->workers[current]->connect( this->workers[current], &GoldbachWorker::calculationDone, this, &GoldbachCalculator::workerDone);
        this->workers[current]->start();
    }
    this->lastRowFetched = 0;
    this->endResetModel();
}

void GoldbachCalculator::stop()
{
    int ideal = QThread::idealThreadCount() - 1;
    for(int current = 0; current < ideal; ++current)
    {
        Q_ASSERT(this->workers[current]);
        this->workers[current]->requestInterruption();
    }
    emit this->percent(0);
}

// Method for tester:
QVector<QString> GoldbachCalculator::getAllSums() const
{
    QVector<QString> allSums;
    int count = 1;
    for(int index = 0; index < this->results.size(); ++index)
    {
        for(int indexSums = 0; indexSums < this->results[index].size(); ++indexSums)
        {
            allSums.append(tr("%1: %2").arg(count).arg(this->results[index][indexSums]));
            ++count;
        }
    }
    return allSums;
}

void GoldbachCalculator::percent(int percent)
{
    this->percentSoFar += percent;
    if( this->percentSoFar == 1 )
        this->fetchMore(QModelIndex());
    emit updateProgressBar(this->percentSoFar);
}

long long GoldbachCalculator::sumsFound() const
{
    long long count = 0;
    for(int index = 0; index < this->results.count(); ++index)
        count += this->results[index].count();
    return count;
}

void GoldbachCalculator::printSums() const
{
    QVector<QString> allSums = getAllSums();
    for(int index = 0; index < allSums.count(); ++index)
        std::cerr << allSums[index].toStdString() << std::endl;
}

bool GoldbachCalculator::allWorkersDone()
{
    int count = 0;
    for(int index = 0; index < this->workers.count(); ++index)
    {
        if(this->workers[index] == nullptr)
            ++count;
    }
    return count == this->workers.count();
}

int GoldbachCalculator::rowCount(const QModelIndex &parent) const //interfaz no se enloquezca, cantidad que pide el usuario
{
    Q_UNUSED(parent);
    return this->lastRowFetched;
}

QVariant GoldbachCalculator::data(const QModelIndex &index, int role) const
{
    // dar cuantas filas quiere
    // cada vez que se necesita una 2fila se invoca data

    if (!index.isValid())
        return QVariant();

    if (index.row() < 0 || index.row() >= this->sumsFound())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        int count = 0;
        for(int indexSums = 0; indexSums < this->results.count(); ++indexSums)
        {
            count+= this->results[indexSums].count();
            if( count > index.row() )
            {
                QString text = QString::number( index.row()+1 );
                int difference = qAbs(index.row()-count);
                return QVariant(text+". "+this->results[indexSums][this->results[indexSums].count()-difference]);
            }
        }
    }

    return QVariant();
}

bool GoldbachCalculator::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return this->lastRowFetched < this->sumsFound() ;
}

void GoldbachCalculator::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);
    int remainder = this->sumsFound() - this->lastRowFetched;
    int itemsToFetch = qMin(100, remainder);

    if (itemsToFetch <= 0)
        return;

    int firstRow = this->lastRowFetched;
    int lastRow  = this->lastRowFetched + itemsToFetch-1;
    beginInsertRows(QModelIndex(), firstRow, lastRow);
    this->lastRowFetched += itemsToFetch;
    endInsertRows();

}

void GoldbachCalculator::workerDone(int workerNumber, long long sumCount)
{
    Q_UNUSED(sumCount);
    Q_ASSERT(this->workers[workerNumber]);
    QTimer::singleShot( 10, this->workers[workerNumber], SLOT(deleteLater()) );
    this->workers[workerNumber]->deleteLater();
    this->workers[workerNumber] = nullptr;
    //this->printSums();
    if( this->allWorkersDone()  )
    {
        emit this->percent(100);
        emit this->calculationDone(workerNumber, this->sumsFound());
    }
}


