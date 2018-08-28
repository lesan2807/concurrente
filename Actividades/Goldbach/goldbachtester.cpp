#include <iostream>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include "goldbachcalculator.h"
#include "goldbachtester.h"


GoldbachTester::GoldbachTester(int &argc, char **argv)
    : QCoreApplication(argc, argv)
{

}

int GoldbachTester::run()
{
    for( int index = 1; index < this->arguments().count(); ++index )
    {
        this->testDirectory(this->arguments()[index]);
    }

    return this->exec();
}

bool GoldbachTester::testDirectory(const QString &path)
{
    QDir dir(path);
    if( ! dir.exists() )
        return std::cerr << "error: directory not found " << qPrintable(path) << std::endl, false;

    dir.setFilter(QDir::Files);
    QFileInfoList fileList = dir.entryInfoList();
    for ( int index = 0; index < fileList.count(); ++index)
        this->testFile( fileList[index].absoluteFilePath() );
        // std::cout << "Processing file " << qPrintable( fileList[index].absoluteFilePath() ) << std::endl;

    return true;
}

bool GoldbachTester::testFile(const QFileInfo& fileInfo)
{

    bool ok = true;
    long long number = fileInfo.baseName().toLongLong(&ok);
    if ( ok == false )
        return std::cerr << "error: invalid number: " << qPrintable(fileInfo.baseName()) << std::endl, false;

    GoldbachCalculator* goldbachCalculator = new GoldbachCalculator(this);
    this->calculators.insert( goldbachCalculator, fileInfo );
    this->connect(goldbachCalculator, &GoldbachCalculator::calculationDone, this, &GoldbachTester::calculationDone);
    goldbachCalculator->calculate(number); // eso hace que una cosa que vaya corriendo paralelo siguente. el codigo dejo de ser secuencial. y queremos los resultados, y guardarlos en un arreglo de strings.


    return true;
}

QVector<QString> GoldbachTester::loadLines(const QFileInfo &fileInfo)
{
    QVector<QString> result;
    QFile file( fileInfo.absoluteFilePath() );
    if( ! file.open( QIODevice::ReadOnly) )
        return std::cerr << "error: could not open: " << qPrintable(fileInfo.fileName()), result;
    QTextStream textStream( &file );
    while( !textStream.atEnd() )
        result.append( textStream.readLine() );

    return result;
}

void GoldbachTester::calculationDone(long long sumCount)
{
    Q_UNUSED(sumCount)
    GoldbachCalculator* goldbachCalculator = dynamic_cast<GoldbachCalculator*>( sender() );
    Q_ASSERT(goldbachCalculator); // tiene que ser un goldbachCalculator

    QFileInfo fileInfo = this->calculators.value(goldbachCalculator);

    const QVector<QString>& calculatorSums = goldbachCalculator->getAllSums();
    const QVector<QString>& expectedSums = loadLines( fileInfo );
    if( calculatorSums != expectedSums )
        std::cerr << "test case failed: " << qPrintable(fileInfo.fileName()) << std::endl;

    this->calculators.remove(goldbachCalculator);
    goldbachCalculator->deleteLater();

    if( this->calculators.count() <= 0 )
        this->quit();
}
