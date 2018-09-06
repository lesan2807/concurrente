#include <iostream>

#include <QDir>
#include <QFile>
#include <QtMath>
#include <QTextStream>

#include "goldbachcalculator.h"
#include "goldbachtester.h"


GoldbachTester::GoldbachTester(int &argc, char **argv)
    : QCoreApplication(argc, argv)
{

}

int GoldbachTester::run()
{
    if(this->arguments().count() < 1)
        return std::cerr << "Usage: GoldbachTester test_folder1 test_folder2 ... test_folderN" << std::endl, 2;

    for( int index = 1; index < this->arguments().count(); ++index )
    {
        if( ! this->testDirectory(this->arguments()[index]) )
            return 3;
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
    if(fileList.count() == 0)
        return false;

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
    ++this->total;
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

int GoldbachTester::percentPassed() const
{
    return qCeil((this->passed*100)/this->total);
}

int GoldbachTester::percentFailed() const
{
    return qCeil((this->failed*100)/this->total);
}

bool GoldbachTester::compareSums(const QFileInfo &fileInfo, const QVector<QString> &results, const QVector<QString> &expected)
{
    for(int index = 0; index < expected.count(); ++index)
    {
        if( expected[index]  == "")
            return true;
        if( results[index] != expected[index])
        {
            std::cerr << "test case " << fileInfo.baseName().toStdString() << " failed at line " << index+1 << ":" << std::endl;
            std::cerr << "\t expected: \"" << expected[index].toStdString() << "\"" << std::endl;
            std::cerr << "\t produced: \"" << results[index].toStdString() << "\"" << std::endl << std::endl;

            return false;
        }
    }
    return true;
}

void GoldbachTester::calculationDone(long long sumCount)
{
    Q_UNUSED(sumCount)
    GoldbachCalculator* goldbachCalculator = dynamic_cast<GoldbachCalculator*>( sender() );
    Q_ASSERT(goldbachCalculator); // tiene que ser un goldbachCalculator

    QFileInfo fileInfo = this->calculators.value(goldbachCalculator);

    const QVector<QString>& calculatorSums = goldbachCalculator->getAllSums();
    const QVector<QString>& expectedSums = loadLines( fileInfo );
    if( ! this->compareSums(fileInfo, calculatorSums, expectedSums) )
    {

        ++this->failed;
    }
    else
    {
        ++this->passed;
    }
    this->calculators.remove(goldbachCalculator);
    goldbachCalculator->deleteLater();

    if( this->calculators.count() <= 0 )
    {
        std::cerr << this->total << " test cases: " << this->passed << " passed (" << this->percentPassed() << "%), " << this->failed << " failed (" << this->percentFailed() << "%)."  << std::endl;
        this->quit();
    }

}
