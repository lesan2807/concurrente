#ifndef GOLDBACHWORKERMANAGER_H
#define GOLDBACHWORKERMANAGER_H

#include "goldbachworker.h"

#include <QVector>
#include <QThread>

class GoldbachWorkerManager : QThread
{
    Q_OBJECT

  private:
    QVector<GoldbachWorker*> goldbachWorkers;

  public:
    GoldbachWorkerManager(long long number);
    ~GoldbachWorkerManager();


  protected:


  signals:
    void sendSum();
  protected slots:

};

#endif // GOLDBACHWORKERMANAGER_H
