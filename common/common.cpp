#include "common.h"
#include <QTime>


QTextStream& operator << (QTextStream& stream, A053 &a053)
{
    QDateTime currentTime = QDateTime::currentDateTime();

    stream << currentTime.toString("hh:mm:ss.zzz") << ';'
           << a053.altitude  << '\n';

    return stream;
}

QDataStream &operator >>(QDataStream &stream, A053 &a053)
{
    stream >> a053.altitude;

    return stream;
}

QDataStream &operator <<(QDataStream &stream, A053 &a053)
{
    stream << a053.altitude;

    return stream;
}

QTextStream &operator <<(QTextStream &stream, BpiData &bpi)    // 34 params
{
    QDateTime currentTime = QDateTime::currentDateTime();

    stream << currentTime.toString("hh:mm:ss.zzz") << ';'
        << bpi.nNavCounter << ';'
        << bpi.sns_utc << ';'
        << bpi.snd_utc << ';'
        << bpi.V_X << ';'
        << bpi.V_Y << ';'
        << bpi.V_Z << ';'
        << bpi.roll << ';'
        << bpi.pitch << ';'
        << bpi.head << ';'
        << bpi.head_magn << ';'
        << bpi.B << ';'
        << bpi.L << ';'
        << bpi.h << ';'
        << bpi.Wx << ';'
        << bpi.Wy << ';'
        << bpi.Wz << ';'
        << bpi.alfa << ';'
        << bpi.beta << ';'
        << bpi.Pst << ';'
        << bpi.Tnv << ';'
        << bpi.Nx << ';'
        << bpi.Ny << ';'
        << bpi.Nz << ';'
        << bpi.coarse << ';'
        << bpi.align_time << ';'
        << bpi.nav_mode << ';'
        << bpi.nUDPCounter << ';'
        << bpi.unix_time << ';'
        << bpi.work_time << ';'
        << bpi.pni_bits << ';'
        << bpi.fix_bits << ';'
        << bpi.recv_status << '\n';

    return stream;
}

QDataStream &operator <<(QDataStream &stream, BpiData &bpi)
{
    stream.writeRawData(bpi.hdr, 4);

    stream  << bpi.nNavCounter
            << bpi.sns_utc
            << bpi.snd_utc
            << bpi.V_X
            << bpi.V_Y
            << bpi.V_Z
            << bpi.Vair
            << bpi.roll
            << bpi.pitch
            << bpi.head
            << bpi.head_magn
            << bpi.B
            << bpi.L
            << bpi.h
            << bpi.Wx
            << bpi.Wy
            << bpi.Wz
            << bpi.alfa
            << bpi.beta
            << bpi.H_baro
            << bpi.Pst
            << bpi.Tnv
            << bpi.Nx
            << bpi.Ny
            << bpi.Nz
            << bpi.coarse
            << bpi.WindDir
            << bpi.WindV
            << bpi.op_ns
            << bpi.op_ver
            << bpi.np_ns
            << bpi.np_ver
            << bpi.np_num
            << bpi.align_time
            << bpi.nav_mode
            << bpi.nUDPCounter
            << static_cast<quint64>(bpi.unix_time)
            << bpi.work_time
            << bpi.priority
            << bpi.gps_glo
            << bpi.sns_mode
            << bpi.sns_cmd
            << bpi.pni_bits
            << bpi.fix_bits
            << bpi.recv_status;

    return stream;
}

QDataStream &operator >>(QDataStream &stream, BpiData &bpi)
{
    quint64 unix_time;
    stream.readRawData(bpi.hdr, 4);

    stream >> bpi.nNavCounter
           >> bpi.sns_utc
           >> bpi.snd_utc
           >> bpi.V_X
           >> bpi.V_Y
           >> bpi.V_Z
           >> bpi.Vair
           >> bpi.roll
           >> bpi.pitch
           >> bpi.head
           >> bpi.head_magn
           >> bpi.B
           >> bpi.L
           >> bpi.h
           >> bpi.Wx
           >> bpi.Wy
           >> bpi.Wz
           >> bpi.alfa
           >> bpi.beta
           >> bpi.H_baro
           >> bpi.Pst
           >> bpi.Tnv
           >> bpi.Nx
           >> bpi.Ny
           >> bpi.Nz
           >> bpi.coarse
           >> bpi.WindDir
           >> bpi.WindV
           >> bpi.op_ns
           >> bpi.op_ver
           >> bpi.np_ns
           >> bpi.np_ver
           >> bpi.np_num
           >> bpi.align_time
           >> bpi.nav_mode
           >> bpi.nUDPCounter
           >> unix_time
           >> bpi.work_time
           >> bpi.priority
           >> bpi.gps_glo
           >> bpi.sns_mode
           >> bpi.sns_cmd
           >> bpi.pni_bits
           >> bpi.fix_bits
           >> bpi.recv_status;

    bpi.unix_time = static_cast<uint64_t>(unix_time);

    return stream;
}
