#ifndef COMMON_H
#define COMMON_H

#include <QDataStream>
#include <QTextStream>
#include <stdint.h>

#pragma pack(1)
struct A053
{
    uint32_t altitude;
};
QTextStream& operator << (QTextStream& out, A053 &a053);
QDataStream& operator >> (QDataStream& out, A053 &a053);
QDataStream& operator << (QDataStream& out, A053 &a053);

struct BpiData
{
    char hdr[4];            //!< Заголовок при записи в файл BIV=   -

    uint32_t nNavCounter;   //!< Счетчик работы навигационного алгоритма
    uint32_t sns_utc;       //!< hhmmssxxx, пример 101420100 это 10:14:20.100
    uint32_t snd_utc;       //!< hhmmssxxx, пример 101420100 это 10:14:20.100, время СНД

    double V_X;             //!< Продольная составляющая земной скорости в ЛКС
    double V_Y;             //!< Вертикальная составляющая земной скорости в ЛКС
    double V_Z;             //!< Боковая составляющая земной скорости в ЛКС

    double Vair;            //!< Воздушная скорость, м/c    -

    double roll;            //!< Крен, радиан
    double pitch;           //!< Тангаж, радиан
    double head;            //!< Курс, радиан
    double head_magn;       //!< Курс магнитный, радиан

    double B;               //!< Широта в WGS-84, радианы
    double L;               //!< Долгота в WGS-84, радианы
    double h;               //!< Высота в WGS-84, метры

    double Wx;              //!< проекция угловой скорости на ось Ox в ССК, радиан/с
    double Wy;              //!< проекция угловой скорости на ось Oy в ССК, радиан/с
    double Wz;              //!< проекция угловой скорости на ось Oz в ССК, радиан/с

    double alfa;            //!< Угол атаки, радиан
    double beta;            //!< Угол скольжения, радиан

    double H_baro;          //!< Барометрическая высота, м  -

    double Pst;             //!< Статическое давление, Па
    double Tnv;             //!< Температура наружного воздуха °, К

    double Nx;              //!< Продольная перегрузка в ССК, g
    double Ny;              //!< Вертикальная перегрузка в ССК, g
    double Nz;              //!< Боковая перегрузка в ССК, g

    double coarse;          //!< Путевой угол, радианы

    double WindDir;         //!< Направление ветра, радиан-
    double WindV;           //!< Скорость ветра, м/c-

    uint8_t op_ns;          //!< Номер прошивки ОВ  -
    uint8_t op_ver;         //!< Версия ОВ          -
    uint8_t np_ns;          //!< Номер прошивки НВ  -
    uint8_t np_ver;         //!< Версия НВ          -
    uint8_t np_num;         //!< Номер НВ           -

    uint8_t align_time;     //!< Время до окончания выставки или прогрева

    uint8_t nav_mode;       //!< режим работы БИВ

    uint32_t nUDPCounter;   //!< Счетчик UDP пакетов

    uint64_t unix_time;
    uint16_t work_time;     //!< Время наработки БИВ в часах

    uint8_t priority;       //!< Приоритет источника (0 - НАВИГАЦИОННЫЙ, 1 - РЕЗЕРВНЫЙ) -

    uint8_t gps_glo;        //!< мл. байт число GPS, старший - ГЛОНАСС      -

    uint8_t sns_mode;       //!< Режим работы СНС: @sa UNIT_PNI_SNS_MODE    -
    uint8_t sns_cmd;        //!< Режим работы СНС: @sa UNIT_PNI_SNS_CMD     -

    uint32_t pni_bits;      //!< Это биты достоверности данных  @sa UNIT_PNI_PNI_BITS
    uint32_t fix_bits;      //!< Признаки исправности устройств @sa UNIT_PNI_FIX_BITS

    uint32_t recv_status;   //!< статус приема данных для unit_pni_process_canmsg и unit_pni_get_data

};
QTextStream& operator << (QTextStream& out, BpiData &bpi);
QDataStream& operator >> (QDataStream& out, BpiData &bpi);
QDataStream& operator << (QDataStream& out, BpiData &bpi);

#pragma pack()

#endif // COMMON_H
