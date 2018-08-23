#include "goldbachworkermanager.h"

class QThread;

GoldbachWorkerManager::GoldbachWorkerManager(long long number)
{
    for( int goldbachWorkerIndex = 0; goldbachWorkerIndex < QThread::idealThreadCount(); ++goldbachWorkerIndex)
    {
        this->goldbachWorkers.append(new GoldbachWorker(number, goldbachWorkerIndex, QThread::idealThreadCount())); // ¿cuál es el papá?, ¿tiene papá?
    }

}

GoldbachWorkerManager::~GoldbachWorkerManager()
{
    for( int goldbachWorkerIndex = 0; goldbachWorkerIndex < QThread::idealThreadCount(); ++goldbachWorkerIndex)
    {
        delete this->goldbachWorkers[goldbachWorkerIndex];
    }

}

