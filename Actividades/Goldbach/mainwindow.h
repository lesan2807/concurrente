#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QProgressBar;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)

  protected:
    Ui::MainWindow *ui;
    QProgressBar* progressBar = nullptr;
    bool stopped = true;

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    /**
     * @brief Start the calculations of sums for the given @a number
     * @param number The number given by user
     */
    void startCalculation(long long number);
    void appendResult(const QString& result);
    void updateProgressBar(int percent);
    inline bool isStopped() const
    {
        return this->stopped;
    }

  private slots:
    void on_lineEditNumber_textEdited(const QString &arg1);
    void on_pushButtonStart_clicked();

    void on_pushButtonStop_clicked();

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
};

#endif // MAINWINDOW_H
