#include "goldbachworker.h"
#include <QtMath>
#include <iostream>

GoldbachWorker::GoldbachWorker(long long number, int current,int ideal, QObject *parent)
    : QThread(parent)
    , number(number)
    , workerCurrent{current}
    , workerIdeal{ideal}

{

}

void GoldbachWorker::run()
{
    this->calculate(this->number);
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
        if ( isPrime(b) )
            emit sumFound( tr("%1: %2 + %3").arg(++results).arg(a).arg(b) );

//        // Update the progress bar
//        this->updateProgressBar((a + 1) * 100 / number);
        // If user cancelled, stop calculations
        if ( this->isInterruptionRequested() )
            return results;
    }
    return results;
}

long long GoldbachWorker::calculateOddGoldbach(long long number)
{
    long long results = 0;
    for ( long long a = this->initialRange(); a < this->finalRange(); ++a )
    {
        if ( ! isPrime(a) ) continue;
        for ( long long b = a; b < number/2; ++b )
        {
            if ( ! isPrime(b) ) continue;
            long long c = number - a - b;
            if (  isPrime(c) )
                emit sumFound( tr("%1: %2 + %3 + %4").arg(++results).arg(a).arg(b).arg(c) );

//            // Update the progress bar
//            this->updateProgressBar((a + 1) * 100 / number);
            // If user cancelled, stop calculations
            if ( this->isInterruptionRequested() )
                return results;
        }
    }
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
    return 0;
}

long long GoldbachWorker::finalRange()
{
    if (this->workerCurrent == this->workerIdeal-1)
        return this->number/2;
    long long initialRange = 0;
    long long division = (this->number-2)/this->workerIdeal;
    initialRange = 2 + ((this->workerCurrent+1)*division);
    return 0;
}
