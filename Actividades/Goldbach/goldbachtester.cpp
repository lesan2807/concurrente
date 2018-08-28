#include <iostream>
#include <QDir>

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

bool GoldbachTester::testFile(const QString &path)
{
    Q_UNUSED(path);
    return true;
}
