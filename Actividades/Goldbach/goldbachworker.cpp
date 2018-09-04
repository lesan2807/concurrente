#include "goldbachworker.h"
#include <QtMath>
#include <iostream>

GoldbachWorker::GoldbachWorker(long long number, int current, int ideal, QVector<QVector<QString>*>& results, QObject *parent)
    : QThread(parent)
    , number(number)
    , workerCurrent{current}
    , workerIdeal{ideal}
    , results{results}
{
    this->sums = new QVector<QString>();
}

void GoldbachWorker::run()
{
    long long sumCount = this->calculate(this->number);
    this->results.append(this->sums);
    emit this->calculationDone(this->workerCurrent, this->sums->count());
}

long long GoldbachWorker::calculate(long long number)
{
    this->time.start();
    if ( number < 4 || number == 5 ) return 0;
    return number % 2 == 0 ? calculateEvenGoldbach(number) : calculateOddGoldbach(number);
}

long long GoldbachWorker::calculateEvenGoldbach(long long number)
{
    long long results = 0;
    for ( long long a = this->initialRange(); a < this->finalRange(); ++a )
    {
        if ( ! isPrime(a) ) continue;
        long long b = number - a;
        if ( b >= a && isPrime(b) )
        {
            this->sums->append( tr("%1: %2 + %3").arg(this->results.count()+1).arg(a).arg(b) );
            ++results;
        }
        // If user cancelled, stop calculations
        if ( this->isInterruptionRequested() )
            return results;
    }
    double seconds = this->time.elapsed() / 1000.0;
//    std::cerr << "Worker #" << this->workerCurrent << " time elapsed: " << seconds << "sums found: " << results << std::endl;
//    std::cerr << "initial: " << this->initialRange() << " final: " << this->finalRange() << std::endl;
    return results;
}

long long GoldbachWorker::calculateOddGoldbach(long long number)
{
    long long results = 0;
    for ( long long a = this->initialRange(); a < this->finalRange(); ++a )
    {
        if ( ! isPrime(a) ) continue;
        for ( long long b = a; b < number; ++b )
        {
            if ( ! isPrime(b) ) continue;
            long long c = number - a - b;
            if ( c >= b && isPrime(c) )
            {
                this->sums->append(tr("%1: %2 + %3 + %4").arg(this->results.count()+1).arg(a).arg(b).arg(c) );
                ++results;
            }

            // If user cancelled, stop calculations
            if ( this->isInterruptionRequested() )
                return results;
        }
    }
    double seconds = this->time.elapsed() / 1000.0;
//    std::cerr << "Worker #" << this->workerCurrent << " time elapsed: " << seconds  << " sums found: " << results << std::endl;
//    std::cerr << "initial: " << this->initialRange() << " final: " << this->finalRange() << std::endl;
    return results;
}

bool GoldbachWorker::isPrime(long long number)
{
    if ( number < 2 ) return false;

    for ( long long i = 2, last = qSqrt(number); i <= last; ++i )
        if ( number % i == 0 )
            return false;

    return true;
}

long long GoldbachWorker::initialRange()
{
    long long initialRange = 0;
    long long division = (this->number-2)/this->workerIdeal;
    initialRange = 2 + (this->workerCurrent*division);
    return initialRange;
}

long long GoldbachWorker::finalRange()
{
    if (this->workerCurrent == this->workerIdeal-1)
        return this->number;
    long long finalRange = 0;
    long long division = (this->number-2)/this->workerIdeal;
    finalRange = 2 + ((this->workerCurrent+1)*division);
    return finalRange;
}
