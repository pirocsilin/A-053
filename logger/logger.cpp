#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QStringList>
#include <QDataStream>
#include <QDateTime>
#include "logger.h"

Logger::Logger()
{
    this->moveToThread(this);
    this->start();

    writeIsAllow    = false;
    checkOpenFiles  = true;
}

Logger::~Logger()
{
    closeFiles();
}

void Logger::initFiles()
{
    QDir currentDir(".");
    QStringList filter;
    filter << fileNameBpi+'*' << fileNameA053+'*';
    currentDir.setNameFilters(filter);

    QFileInfoList fileInfoList = currentDir.entryInfoList();
    QVector<QString> fileNameList;

    foreach(QFileInfo fileInfo, fileInfoList)
        fileNameList.append(fileInfo.fileName());

    int countBpiFiles{};
    int countA053Files{};

    foreach(QString filename, fileNameList)
    {
        if (filename.contains(fileNameBpi))      countBpiFiles++;
        else if(filename.contains(fileNameA053)) countA053Files++;
    }

    QDateTime currentDateTime = QDateTime::currentDateTime();
    fileBpi.setFileName(fileNameBpi + "_" + currentDateTime.toString(DATE_TIME) + expansion);
    fileA053.setFileName(fileNameA053 + "_" + currentDateTime.toString(DATE_TIME) + expansion);

    if(!fileBpi.open(QIODevice::Append  | QIODevice::Text) ||
       !fileA053.open(QIODevice::Append | QIODevice::Text)  )
    {
        qDebug() << "ошибка открытия файла(ов)";
        checkOpenFiles = false;
        closeFiles();
    }
    else
    {
        writeHeaderToFile(headerBpi, fileBpi);
        writeHeaderToFile(headerA053, fileA053);
    }
}

void Logger::closeFiles()
{
    if(fileBpi.isOpen())    fileBpi.close();
    if(fileA053.isOpen())   fileA053.close();
}

void Logger::writeHeaderToFile(const QVector<QString> &list, QFile &file)
{
    QTextStream out(&file);

    int ctr = list.size();

    foreach(QString hName, list)
    {
        out << hName << (--ctr != 0 ? ';' : '\n');
    }
}

void Logger::writeBpiData(BpiData data)
{
    if(writeIsAllow && checkOpenFiles)
    {
        QTextStream out(&fileBpi);

        out << data;
    }
    else{
        //qDebug() << "запись выключена!";
    }
}

void Logger::writeA053Data(DWORD data)
{
    if(writeIsAllow && checkOpenFiles)
    {
        QTextStream out(&fileA053);
        QDateTime currentTime = QDateTime::currentDateTime();

        out << currentTime.toString("hh:mm:ss.zzz")
            << ';' << data  << '\n';
    }
    else{
        //qDebug() << "запись выключена!";
    }
}

void Logger::slotSwitchStateRecord(bool state)
{
    writeIsAllow = state;

    if(writeIsAllow == true)
        initFiles();
    else
        closeFiles();
}
