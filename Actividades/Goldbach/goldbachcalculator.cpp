#include "goldbachcalculator.h"
#include "goldbachworker.h"
#include <iostream>
#include <algorithm>

GoldbachCalculator::GoldbachCalculator(QObject *parent)
    : QAbstractListModel(parent)
{

}

void GoldbachCalculator::calculate(long long number)
{
    this->beginResetModel();
    int ideal = QThread::idealThreadCount() - 1;

    //this->results.resize(ideal);
    for(int index = 0; index < ideal; ++index)
        this->results.append(QVector<QString>());

    for (int current = 0; current < ideal; ++current)
    {
        GoldbachWorker* worker = new GoldbachWorker(number, current, ideal, this->results[current], this);
        this->workers.append(worker);
        this->workers[current]->connect( this->workers[current], &GoldbachWorker::calculationDone, this, &GoldbachCalculator::workerDone);
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
}

// Method for tester:
QVector<QString> GoldbachCalculator::getAllSums() const
{
    QVector<QString> allSums;
    for(int index = 0; index < this->results.size(); ++index)
    {
        for(int indexSums = 0; indexSums < this->results.at(index).count(); ++index)
            allSums.append(this->results[index][indexSums]);
    }
    return allSums;
}

int GoldbachCalculator::percent()
{
    return 100;
}

long long GoldbachCalculator::sumsFound()
{
    long long sums = 0;
    for(int index = 0; index < this->results.size(); ++index)
    {
        sums += this->results[index].size();
    }
    return sums;
}

void GoldbachCalculator::printSums(int workerId)
{
    for(int indexString = 0; indexString < this->results.at(workerId).size(); ++indexString)
    {
        std::cerr << workerId << ": " << this->results.at(workerId).at(indexString).toStdString() << std::endl;
    }
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
       return QVariant();
       //return this->results[ index.row() ];
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
    emit this->calculationDone(workerNumber, this->sumsFound());
    this->printSums(workerNumber);
    this->workers[workerNumber]->deleteLater();
    this->workers[workerNumber] = nullptr;
}


