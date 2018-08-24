#ifndef GOLDBACHCALCULATOR_H
#define GOLDBACHCALCULATOR_H

#include <QObject>

class GoldbachCalculator : public QObject
{
    Q_OBJECT
public:
    explicit GoldbachCalculator(QObject *parent = nullptr);

signals:

public slots:
};

#endif // GOLDBACHCALCULATOR_H