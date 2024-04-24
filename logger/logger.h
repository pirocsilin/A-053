#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QThread>
#include <QVector>
#include <QFile>
//#include <objbase.h>
#include "common.h"

#define DATE_TIME "yyyy-MM-dd[hh:mm:ss]"

typedef unsigned long DWORD;

class Logger : public QThread
{
    Q_OBJECT

public:

    Logger();
    ~Logger();

    void initFiles();
    void closeFiles();

private:
    const QVector<QString> headerBpi    // 33 params
    {
        "Time", "nNavCounter", "sns_utc", "snd_utc",
        "V_X", "V_Y", "V_Z", "roll", "pitch",
        "head", "head_magn", "B", "L", "h", "Wx", "Wy", "Wz",
        "alfa", "beta", "Pst", "Tnv", "Nx", "Ny", "Nz", "coarse",
        "align_time", "nav_mode", "nUDPCounter", "unix_time",
        "work_time", "pni_bits", "fix_bits", "recv_status"
    };

    const QVector<QString> headerA053 { "Time", "Altitude", "Arink_word"};

    QString fileNameBpi {"Log_BPI"};
    QString fileNameA053{"Log_A-053"};
    QString expansion   {".txt"};

    QFile fileBpi;
    QFile fileA053;
    bool checkOpenFiles;
    bool writeIsAllow;

    void writeHeaderToFile(const QVector<QString> &list, QFile &file);

public slots:

    void writeBpiData(BpiData data);
    void writeA053Data(DWORD data);
    void slotSwitchStateRecord(bool state);

};

#endif // LOGGER_H
