#ifndef A053WATCHER_H
#define A053WATCHER_H

#include <QThread>
#include <QString>
#include <QTimer>
#include <QSharedPointer>
#include <codecvt>
#include <locale>

#include <windows.h>
#include <setupapi.h>
#include <winioctl.h>
#include <stdio.h>
#include <objbase.h>
#include <initguid.h>

#include "intrfECE0206.h"
#include "ioctlECE0206.h"

class A053watcher : public QThread
{
    Q_OBJECT

public:
    A053watcher();
    ~A053watcher();

    #pragma pack(1);
    // Параметр выдачи из каналов
    struct InputParam
    {
        DWORD  param;	// код параметра
        DWORD  timer;	// показания таймера при поступлении параметра
        UCHAR  error;	// ошибка приема параметра

        bool isEmpty() { return param == 0; }

    } inputParam;

    // Настройки входного канала
    struct BufInput
    {
        UCHAR  ChanNumber;  //номер канала (1..4)
        UCHAR  Mode;        //= 0  - рабочий режим; = 1 - режим самоконтроля
        UCHAR  Parity;      //= 0  - без контроля нечетности; = 1 - контроль нечетности
        UCHAR  Freq;        //= 0 – прием на частотах 36-100КГц; = 1 – прием на частотах 11-14,5 КГц
    } bufInputCh;

    // Настройки выходного канала
    struct BufOutput
    {
        UCHAR  Err_en;	// = 0  - выдача 32-битных слов; =1 выдача 33-битных слов;  = 255 - выдача 31-битных слов
        UCHAR  Parity;	// = 0  - без формирования нечетности; = 1 - формирование бита нечетности
        UCHAR  Freq;	//        выдача на частотах
                        // = 0    12,5КГц;
                        // = 1    50КГц;
                        // = 2    100 КГц;
        UCHAR  ArrayDim;    // размер массива (1..255слов или 0 для массива размером 256 слов);
        UCHAR  Delay;       // для многократной и циклической выдачи: интервал между массивами = 0, 1, 2, .., 255 (что соответствует  0  10,24  20,48  ..  2611,2мс);  для однократной выдачи = 0
        UCHAR  ArrayNumber;	// для однократной выдачи = 1; для многократной выдачи - количество выдаваемых массивов (2..255); для циклической выдачи = 0.
    } bufOutputCh;

    // Параметры очистки буфера выходного канала
    struct BuffClearOutCh
    {
        UCHAR   paramAddress;   // номер ячейки буфера BUF256x32 (0..255)
        UCHAR   bufferDimension;// размер обнуляемой области (не более 127)
    } buffClearOutCh;

    struct Arink429Frame
    {
        UCHAR Address;
        UCHAR RVcode;
        UCHAR TestIsAllow;
        USHORT Data;
        UCHAR Sign;
        UCHAR Matrix;
    }arink429Frame;


private:
    HANDLE OpenDeviceByIndex(DWORD Index, PDWORD pError);

    UCHAR chanNumber{0};
    DWORD codeChannel;
    HANDLE hECE0206;
    DWORD nOutput;
    BOOL result;
    UCHAR bufOutput;    // переменная, в которой драйвер возвращает код ошибки

    QSharedPointer<QTimer> timerTryConnect;
    QSharedPointer<QTimer> timerSendData;

    uint8_t countEmptyWord{};

    void setShortMode();
    void readParam(DWORD bffrNum, UCHAR ParamNumber, InputParam &ParamCod);
    void SI_pusk(UCHAR ChanNumber, UCHAR Mode, UCHAR Parity, UCHAR Freq);
    void SO_pusk(UCHAR Err_en, UCHAR Parity, UCHAR Freq, UCHAR ArrayDim, UCHAR Delay, UCHAR ArrayNumber);
    void SI_stop(UCHAR ChanNumber);
    void SO_stop(void);
    void SI_clear_array(UCHAR ChanNumber);
    void SO_clear_array(UCHAR Addr, UCHAR Dim);
    UCHAR getChannelA053();
    void stopChannels();
    QString wcharToString(const wchar_t* wstr);
    void getDataFromArink429Word(Arink429Frame &data, DWORD word);
    DWORD reverseDWORD(DWORD word);
    void delay(uint32_t);

public slots:
    void initA053();
    void tryConnectToA053();
    void deviceIsLive();
    void slotSendData();
signals:
    void signalSendData(DWORD data);
};

#endif // A053WATCHER_H
