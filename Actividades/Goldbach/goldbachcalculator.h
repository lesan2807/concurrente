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
    GoldbachWorker* goldbachWorker = nullptr;
    int lastRowFetched = -1;
    QVector<QString> results;

  public:
    explicit GoldbachCalculator(QObject *parent = nullptr);
    void calculate(long long number);
    void stop();

  public: //obliga el modelo a usar esos métodos
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  protected:
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

  signals:
    void calculationDone(long long sumCount); //avisa que ha terminado los calculos

  protected slots:
    void workerDone(long long sumCount); // cuando termina solo 1 worker

};

#endif // GOLDBACHCALCULATOR_H
