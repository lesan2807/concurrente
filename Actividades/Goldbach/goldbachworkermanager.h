#ifndef GOLDBACHWORKERMANAGER_H
#define GOLDBACHWORKERMANAGER_H

#include "goldbachworker.h"

#include <QVector>
#include <QThread>

class GoldbachWorkerManager : QThread
{
    Q_OBJECT

  protected:
    QVector<GoldbachWorker*> goldbachWorkers;

  public:
    explicit GoldbachWorkerManager(long long number);
    virtual ~GoldbachWorkerManager();


  protected:


  signals:
    void sendSum();
  protected slots:

};

#endif // GOLDBACHWORKERMANAGER_H
