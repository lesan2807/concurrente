#ifndef GOLDBACHWORKER_H
#define GOLDBACHWORKER_H

#include <QObject>

#include <QThread>
#include <QVector>
#include <QTime>

class GoldbachWorker : public QThread
{
    Q_OBJECT

  protected:
    long long number = 0;
    int workerCurrent = 0;
    int workerIdeal = 1;
    QTime time;
    QVector<QString>& sums;

  public:
    explicit GoldbachWorker(long long number, int current, int ideal, QVector<QString>& sums, QObject *parent = nullptr);

  protected:
    void run() override;

  signals: // eventos que genera el worker
    void calculationDone(int workerNumber, long long sumCount);
    void percent(int percent);

  public slots:

  protected:
    /**
     * @brief Calcula las sumas de Goldbach para el numero dado y las agrega a una pizarra
     * @param number El numero dado por el usuario
     * @return La cantidad de sumas encontradas
     */
    long long calculate(long long number);
    /**
     * Calcula todas las sumas de dos primos que equivalen al numero dado, y las presenta
     * en el area resultado. Incrementa la barra de progreso mientras realiza el calculo
     * de la conjetura fuerte de Goldbach, por ejemplo:
     *
     *   4 == 2 + 2
     *   6 == 3 + 3
     *  10 == 3 + 7 == 5 + 5
     *
     * @param numero Un numero entero par mayor o igual a 4
     * @return la cantidad de sumas de dos primos encontradas
     */
    long long calculateEvenGoldbach(long long number);
    /**
     * Calcula todas las sumas de tres primos que equivalen al numero dado, y las presenta
     * en el area resultado. Incrementa la barra de progreso mientras realiza el calculo
     * de la conjetura debil de Goldbach, por ejemplo:
     *
     *   7 == 2 + 2 + 3
     *   9 == 2 + 2 + 5 == 3 + 3 + 3
     *
     * @param numero Un numero entero impar mayor o igual a 7
     * @return la cantidad de sumas de tres primos encontradas
     */
    long long calculateOddGoldbach(long long number);
    /**
     * Retorna true si numero es primo, false si numero no es primo o menor que 2
     * Por definicion 1 no es primo ni compuesto, este metodo retorna false
     */
    static bool isPrime(long long numero);

  protected:
    long long initialRange();
    long long finalRange();
    double onePercent(long long initial, long long final);
    bool isEqual(double a, double b);
};

#endif // GOLDBACHWORKER_H
