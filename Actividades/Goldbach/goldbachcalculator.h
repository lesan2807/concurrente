#ifndef GOLDBACHCALCULATOR_H
#define GOLDBACHCALCULATOR_H

#include <QAbstractListModel>
#include <QVector>

class GoldbachWorker;

class GoldbachCalculator : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(GoldbachCalculator)

  protected:
    QVector<GoldbachWorker*> workers;
    int lastRowFetched = 0;
    int countRows = 0;
    QVector<QVector<QString>> results;
    int percentSoFar = 0;

  public:
    explicit GoldbachCalculator(QObject *parent = nullptr);
    void calculate(long long number);
    void stop();
    QVector<QString> getAllSums() const;

    long long sumsFound() const;
    void printSums() const;
    bool allWorkersDone();

  public: //obliga el modelo a usar esos métodos
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  protected:
    virtual bool canFetchMore(const QModelIndex &parent) const override;
    virtual void fetchMore(const QModelIndex &parent) override;

  signals:
    void calculationDone(int workerNumber, long long sumCount); //avisa que ha terminado los calculos
    void updateProgressBar(int percent);

  protected slots:
    void workerDone(int workerNumber, long long sumCount); // cuando termina solo 1 worker
    void percent(int percent);

};

#endif // GOLDBACHCALCULATOR_H
