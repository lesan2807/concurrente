#include "goldbachcalculator.h"
#include "goldbachworker.h"

GoldbachCalculator::GoldbachCalculator(QObject *parent)
    : QAbstractListModel(parent)
{

}

void GoldbachCalculator::calculate(long long number)
{
    this->beginResetModel();
    this->goldbachWorker = new GoldbachWorker(number, 0, 0, this->results, this);
    this->connect(goldbachWorker, &GoldbachWorker::calculationDone, this, &GoldbachCalculator::workerDone); //terminó el trabajador
    this->goldbachWorker->start();

//    QVector<GoldbachWorker*> workers;

//    int ideal = QThread::idealThreadCount() - 1;
//    for ( int current = 0; current < ideal; ++current  )
//    {
//        GoldbachWorker* worker = new GoldbachWorker(number, current, ideal, this);
//        // Hacer las conexiones para reaccionar cuando el worker:
//        // (1) encuentra una suma, (2) incrementa el porcentaje, (3) termina
//        this->connect(worker, &GoldbachWorker::sumFound, this, &MainWindow::appendResult);
//        //this->connect(worker, &GoldbachWorker::percent, this, &MainWindow::updateProgressBar);
//        this->connect( worker, &GoldbachWorker::finished, this, &MainWindow::workerFinished );
//        workers.append(worker);
//    }

//    for(int index = 0; index < workers.size(); ++index)
//    {
//        workers[index]->start();
//        double seconds = this->time.elapsed() / 1000.0;
//        std::cerr << index << ": " << seconds << std::endl;
//    }

    this->endResetModel();
}

void GoldbachCalculator::stop()
{
    Q_ASSERT(this->goldbachWorker);
    this->goldbachWorker->requestInterruption();
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

void GoldbachCalculator::workerDone(long long sumCount)
{
    emit this->calculationDone(sumCount);
    this->goldbachWorker->deleteLater(); // evitar eliminar el obj antes de que se ejec los eventos
    this->goldbachWorker = nullptr;
}


