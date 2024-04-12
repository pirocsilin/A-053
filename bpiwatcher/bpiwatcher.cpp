#include "bpiwatcher.h"
#include <QEventLoop>
#include <QByteArray>


BPIwatcher::BPIwatcher()
{
    connect(this, &QThread::started, this, &BPIwatcher::slotInitBPI);
    this->moveToThread(this);
    this->start();
}

BPIwatcher::~BPIwatcher()
{
    //this->quit();
}

void BPIwatcher::slotInitBPI()
{
    socket = QSharedPointer<QUdpSocket>(new QUdpSocket);
    timerData = QSharedPointer<QTimer>(new QTimer);
    socket->bind(receivePort);
    connect(timerData.data(), &QTimer::timeout, this, &BPIwatcher::slotSendData);
    connect(socket.data(), &QUdpSocket::readyRead, this, &BPIwatcher::slotReadyRead);

    timerData->start(20);
}

void BPIwatcher::slotReadyRead()
{
    QByteArray datagram;
    BpiData data;

    do
    {
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(), datagram.size());

        QDataStream in(&datagram, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_3);
        in >> data;

        signalSendBpiData(data);

    }while(socket->hasPendingDatagrams());
}

// ================================ START TEST ======================== //

void BPIwatcher::slotSendData()
{
    (++mutex) %= 30;

    BpiData data{"Bpi", 32, 32, 32, 0.1, 0.2, 0.3, 0.4, 15+mutex*1., 9+mutex*1., 45+mutex*1., 0.8, 55+mutex*1.,
                45+mutex*1., 9400.1+mutex, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5,
                8, 8, 8, 8, 8, 8, 8, 32, 64, 16, 8, 8, 8, 8, 32, 32, 32}; // 46 params
    QByteArray buffr;
    QDataStream out(&buffr, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    out << data;

    socket->writeDatagram(buffr, QHostAddress::LocalHost, 50000);
}
