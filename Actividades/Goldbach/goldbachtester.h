#ifndef GOLDBACHTESTER_H
#define GOLDBACHTESTER_H

#include <QCoreApplication>

class GoldbachTester : public QCoreApplication
{
    Q_OBJECT
    Q_DISABLE_COPY(GoldbachTester)

public:
    explicit GoldbachTester(int &argc, char **argv);

signals:

public slots:
};

#endif // GOLDBACHTESTER_H
