#ifndef BPIWATCHER_H
#define BPIWATCHER_H

#include "common.h"
#include <QThread>
#include <QSharedPointer>
#include <QUdpSocket>
#include <QTimer>

class BPIwatcher : public QThread
{
    Q_OBJECT

public:
    BPIwatcher();
    ~BPIwatcher();

private:
    const uint16_t receivePort{50000};

    QSharedPointer<QUdpSocket> socket;
    QSharedPointer<QTimer> timerData;

    int mutex{};

public slots:
    void slotReadyRead();
    void slotInitBPI();

    // for test
    void slotSendData();

signals:
    void signalSendBpiData(BpiData data);

};

#endif // BPIWATCHER_H
