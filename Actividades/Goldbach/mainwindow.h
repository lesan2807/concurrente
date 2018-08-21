#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>

class QProgressBar;
class GoldbachWorker;

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
    GoldbachWorker* goldbachWorker = nullptr;
    QTime time;

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    /**
     * @brief Start the calculations of sums for the given @a number
     * @param number The number given by user
     */
    void startCalculation(long long number);
    void updateProgressBar(int percent);
    inline bool isStopped() const
    {
        return this->stopped;
    }

  private slots: // manejador de eventos
    void on_lineEditNumber_textEdited(const QString &arg1);
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();


  protected slots:
    void workerFinished();
    void appendResult(const QString& result);


};

#endif // MAINWINDOW_H
