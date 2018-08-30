#include "goldbachcalculator.h"
#include "goldbachworker.h"
#include <iostream>

GoldbachCalculator::GoldbachCalculator(QObject *parent)
    : QAbstractListModel(parent)
{

}

void GoldbachCalculator::calculate(long long number)
{
    this->beginResetModel();
    int ideal = QThread::idealThreadCount() - 1;
    for (int current = 0; current < ideal; ++current)
    {
        this->workers.append( new GoldbachWorker(number, current, ideal, this->results, this) );
        this->workers[current]->connect( this->workers[current], &GoldbachWorker::calculationDone, this, &GoldbachCalculator::workerDone);
        this->workers[current]->start();
    }
    this->lastRowFetched = 0;
    this->endResetModel();
}

void GoldbachCalculator::stop()
{
    Q_ASSERT(this);
    int ideal = QThread::idealThreadCount() - 1;
    for(int current = 0; current < ideal; ++current)
    {
        this->workers[current]->requestInterruption();
    }
}

QVector<QString> GoldbachCalculator::getAllSums() const
{
    return this->results;
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

   if (index.row() < 0 || index.row() >= this->results.size())
       return QVariant();

   if (role == Qt::DisplayRole)
   {
       return this->results[ index.row() ];
   }

   return QVariant();
}

bool GoldbachCalculator::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return this->lastRowFetched < this->results.count();
}

void GoldbachCalculator::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);
    int remainder = this->results.size() - this->lastRowFetched;
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
    emit this->calculationDone(workerNumber, results.size());
    this->workers[workerNumber]->deleteLater();
    this->workers[workerNumber] = nullptr;

}


