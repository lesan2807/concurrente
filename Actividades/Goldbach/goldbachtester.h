#ifndef GOLDBACHTESTER_H
#define GOLDBACHTESTER_H

#include <QCoreApplication>
#include <QFileInfo>
#include <QMap>

class GoldbachCalculator;

class GoldbachTester : public QCoreApplication
{
    Q_OBJECT
    Q_DISABLE_COPY(GoldbachTester)

protected:
    QMap<GoldbachCalculator*, QFileInfo> calculators;
    int passed = 0;
    int failed = 0;
    int total = 0;


public:
    explicit GoldbachTester(int &argc, char **argv);
    int run();
    bool testDirectory(const QString& path);
    bool testFile(const QFileInfo &fileInfo);
    static QVector<QString> loadLines(const QFileInfo& fileInfo);
    int percentPassed() const;
    int percentFailed() const;

signals:

protected slots:
    void calculationDone(long long sumCount);
};

#endif // GOLDBACHTESTER_H
