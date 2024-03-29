#include "goldbachworker.h"
#include <QtMath>
#include <iostream>

GoldbachWorker::GoldbachWorker(long long number, int current, int ideal, QVector<QString>& sums, QObject *parent)
    : QThread(parent)
    , number(number)
    , workerCurrent{current}
    , workerIdeal{ideal}
    , sums{sums}
{

}

void GoldbachWorker::run()
{
    double seconds = this->time.elapsed() / 1000.0;
    long long results = this->calculate(this->number);
#ifdef INFO
    std::cerr << "Worker #" << this->workerCurrent << " time elapsed: " << seconds << " sums found: " << results << " Range: " << this->initialRange() << "-" << this->finalRange() << std::endl;
    std::cerr << std::endl;
#endif
    emit this->calculationDone(this->workerCurrent, this->sums.count());
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
    long long a = this->initialRange();
    long long finalRange = this->finalRange();
    double percent = 0.0;
    double count = 0.0;
    if( finalRange-a > 100)
    {
        percent = this->onePercent(a, finalRange);
        count = percent/static_cast<int>(percent);
    }
    double sum = count;
    for (;  a < finalRange; ++a )
    {
        sum += count;
        if( this->isEqual(sum, percent) )
        {
            sum = count;
            emit this->percent(1);
        }
        if ( ! isPrime(a) ) continue;
        long long b = number - a;
        if ( b >= a && isPrime((a)) && isPrime(b) )
        {
            this->sums.append( tr("%2 + %3").arg(a).arg(b) );
            ++results;
        }
        // If user cancelled, stop calculations
        if ( this->isInterruptionRequested() )
            return results;
    }
    return results;
}

long long GoldbachWorker::calculateOddGoldbach(long long number)
{

    long long results = 0;
    long long a = this->initialRange();
    long long finalRange = this->finalRange();
    double percent = 0.0;
    double count = 0.0;
    if( finalRange-a > 100)
    {
        percent = this->onePercent(a, finalRange);
        count = percent/static_cast<int>(percent);
    }
    double sum = count;

    for (; a < finalRange; ++a )
    {
        sum += count;
        if( this->isEqual(sum, percent) )
        {
            sum = count;
            emit this->percent(1);
        }
        if ( ! isPrime(a) ) continue;
        for ( long long b = a; b < number; ++b )
        {
            if ( ! isPrime(b) ) continue;
            long long c = number - a - b;
            if ( c >= b && isPrime(c) )
            {
                ++results;
                this->sums.append(tr("%2 + %3 + %4").arg(a).arg(b).arg(c) );
            }

            // If user cancelled, stop calculations
            if ( this->isInterruptionRequested() )
                return results;
        }

    }
    return results;
}

bool GoldbachWorker::isPrime(long long n)
{
    if (n%2==0) return n==2;
    if (n%3==0) return n==3;
    int d=5;
    long long end = qSqrt(n);
    while(d<=end)
    {
        if(n%d==0) return 0;
        d+=2;
        if(n%d==0) return 0;
        d+=4;
    }
    return 1;
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

double GoldbachWorker::onePercent(long long initial, long long final)
{
    double range = static_cast<double>(final-initial);
    return range/100;
}

bool GoldbachWorker::isEqual(double a, double b)
{
    const double dEpsilon = 0.00000000001; // or some other small number
    return qAbs(a - b) <= ( (qAbs(a) < qAbs(b) ? qAbs(b) : qAbs(a)) * dEpsilon);
}
