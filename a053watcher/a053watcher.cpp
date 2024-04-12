#include "a053watcher.h"
#include <QDebug>

A053watcher::A053watcher()
{
    connect(this, &QThread::started, this, &A053watcher::initA053);
    this->moveToThread(this);
    this->start();
}

A053watcher::~A053watcher()
{
    stopChannels();
    CloseHandle(hECE0206);
}

void A053watcher::initA053()
{
    timerTryConnect = QSharedPointer<QTimer>(new QTimer);
    timerSendData = QSharedPointer<QTimer>(new QTimer);

    connect(timerSendData.data(), &QTimer::timeout, this, &A053watcher::slotSendData);
    connect(timerTryConnect.data(), &QTimer::timeout, this, &A053watcher::tryConnectToA053);
    hECE0206 = INVALID_HANDLE_VALUE;
    timerSendData->setInterval(20);
    timerTryConnect->setInterval(2000);
    timerTryConnect->start();

    tryConnectToA053();
}

void A053watcher::slotSendData()
{
    InputParam paramCode{};
    Arink429Frame arink429Frame{};

    readParam(codeChannel, 0, paramCode);

    SI_clear_array(chanNumber);

    if(!paramCode.isEmpty())
    {
        countEmptyWord = 0;
        DWORD rdata = paramCode.param;
        getDataFromArink429Word(arink429Frame, rdata);

        emit signalSendData(arink429Frame.Data);
    }
    else if(++countEmptyWord >= 5)
    {
        countEmptyWord = 0;
        timerSendData->stop();
        timerTryConnect->start();
        qDebug() << "empty param, countEmptyWord:" << countEmptyWord;
    }

    qDebug() << "Code:"     << paramCode.param
             << "\nTimer:"  << paramCode.timer * 4 / 1000 << "msec"
             << "\nError:"  << QString::number(paramCode.error, 16)
             << "\n----------------------------------";

    qDebug() << "hexWord"       << QString::number(paramCode.param, 16)
             << "\nAddress"     << QString::number(arink429Frame.Address, 16)
             << "\nRVcode"      << arink429Frame.RVcode
             << "\nTestIsAllow" << arink429Frame.TestIsAllow
             << "\nData"        << arink429Frame.Data
             << "\nSign"        << arink429Frame.Sign
             << "\nMatrix"      << arink429Frame.Matrix
             << "\n==================================";
}

void A053watcher::tryConnectToA053()
{
    CloseHandle(hECE0206);
    hECE0206 = INVALID_HANDLE_VALUE;

    DWORD err;
    hECE0206 = OpenDeviceByIndex(0, &err);

    if(hECE0206 == INVALID_HANDLE_VALUE)
    {
        qDebug() << "INVALID_HANDLE_VALUE";
    }
    else
    {
        qDebug() << "HANDLE_SUCESS !!!";

        chanNumber = getChannelA053();

        if(chanNumber != 0)
        {
            qDebug() << "Work channel is:" << chanNumber;
            timerTryConnect->stop();
            timerSendData->start();
        }
        else
        {
            qDebug() << "No work channel";
        }
    }
}

UCHAR A053watcher::getChannelA053()
{
    stopChannels();
    setShortMode();

    UCHAR channel{1};

    // параметры входного канала
    UCHAR  siMode   = 0; // = 0  - рабочий режим; = 1 - режим самоконтроля
    UCHAR  siParity = 1; // = 0  - без контроля нечетности; = 1 - контроль нечетности
    UCHAR  siFreq   = 0; // = 0  - прием на частотах 36-100КГц; = 1 - прием на частотах 11-14,5 КГц

    // параметры выходного канала
    UCHAR  soLenWord = 0; // = 0  - выдача 32-битных слов; =1 выдача 33-битных слов;  = 255 - выдача 31-битных слов
    UCHAR  soParity = 1; // = 0  - без формирования нечетности; = 1 - формирование бита нечетности
    UCHAR  soFreq = 2;   //выдача на частотах
                         //	= 0    12,5КГц;
                         //	 = 1    50КГц;
                         //	 = 2    100 КГц;
    UCHAR  soArrayDim = 1;      // размер массива (1..255слов или 0 для массива размером 256 слов);
    UCHAR  soDelay = 1;         // для многократной и циклической выдачи: интервал между массивами = 0, 1, 2, .., 255 (что соответствует  0  10,24  20,48  ..  2611,2мс); для однократной выдачи = 0
    UCHAR  soArrayNumber = 0;   // для однократной выдачи = 1; для многократной выдачи - количество выдаваемых массивов (2..255); для циклической выдачи = 0.

    for(; channel<=4; channel++)
    {
        SI_pusk(channel, siMode, siParity, siFreq);

        SI_clear_array(channel);

        SO_pusk(soLenWord, soParity, soFreq, soArrayDim, soDelay, soArrayNumber);

        codeChannel = channel==1 ? ECE02061_XP_READ_PARAM_CC1 :
                      channel==2 ? ECE02061_XP_READ_PARAM_CC2 :
                      channel==3 ? ECE02061_XP_READ_PARAM_CC3 :
                                   ECE02061_XP_READ_PARAM_CC4 ;

        InputParam  paramCode{};
        for(int i{}; i<10; i++)
        {
            readParam(codeChannel, 0, paramCode);
            if(paramCode.param)
                return channel;
            delay(20);
        }

        SO_stop();
        SI_stop(channel);
    }

    return 0;
}

void A053watcher::stopChannels()
{
    SO_stop();
    SI_stop(1);
    SI_stop(2);
    SI_stop(3);
    SI_stop(4);
}

DWORD A053watcher::reverseDWORD(DWORD word)
{
    DWORD rWord{0};
    while(word)
    {
        rWord <<= 1;
        rWord |= word & 1;
        word >>= 1;
    }
    return rWord;
}

HANDLE A053watcher::OpenDeviceByIndex(DWORD Index, PDWORD pError)
{
    _GUID guid{0xc32079a6, 0x2663, 0x4647, 0xb0, 0xa4, 0xa2, 0xc2, 0x4, 0xa9, 0x34, 0x1};

    GUID ClassGuid = guid; //GUID_DEVINTERFACE_ECE0206_1;

    HANDLE hDev;
    HDEVINFO hDevinfo;
    SP_DEVICE_INTERFACE_DATA	DeviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA	pDetail = NULL;
    DWORD  ReqSize;


    hDevinfo = SetupDiGetClassDevs(	(LPGUID)&ClassGuid,
                                    NULL,
                                    NULL,
                                    DIGCF_DEVICEINTERFACE |DIGCF_PRESENT );

    if(hDevinfo == INVALID_HANDLE_VALUE)
    {
        *pError = GetLastError();
    printf("hARINC1\n");
        return INVALID_HANDLE_VALUE;
    }

    DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);

    if (!SetupDiEnumDeviceInterfaces(	hDevinfo,
                                        NULL,
                                        (LPGUID)&ClassGuid,
                                        Index,
                                        &DeviceInterfaceData))

    {
        *pError = GetLastError();
    printf("hARINC2  %d\n",DeviceInterfaceData.cbSize);
        return INVALID_HANDLE_VALUE;
    }


        SetupDiGetDeviceInterfaceDetail(	hDevinfo,
                                            &DeviceInterfaceData,
                                            NULL,
                                            0,
                                            &ReqSize,
                                            NULL );

        pDetail = PSP_INTERFACE_DEVICE_DETAIL_DATA(new char[ReqSize]);

        if ( !pDetail )
        {
    printf("hARINC3");
            *pError = ERROR_NOT_ENOUGH_MEMORY;
            return INVALID_HANDLE_VALUE;
        }
        pDetail->cbSize = sizeof (SP_INTERFACE_DEVICE_DETAIL_DATA);

    if (! SetupDiGetDeviceInterfaceDetail(	hDevinfo,
                                            &DeviceInterfaceData,
                                            pDetail,
                                            ReqSize,
                                            &ReqSize,
                                            NULL )
                                            )
    {
    printf("hARINC4");
        *pError = GetLastError();
        return INVALID_HANDLE_VALUE;
    }


    SetupDiDestroyDeviceInfoList(hDevinfo);


    hDev = CreateFile(
        pDetail->DevicePath,
        0,//GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        );

    if (hDev == INVALID_HANDLE_VALUE)
    {
    printf("hARINC5");
        *pError = GetLastError();
        return INVALID_HANDLE_VALUE;
    }


        *pError = ERROR_SUCCESS;

    return hDev;
}

// Пуск входного канала ============================================== //
void A053watcher::SI_pusk(UCHAR ChanNumber, UCHAR Mode, UCHAR Parity, UCHAR Freq)
{
    bufInputCh.ChanNumber = ChanNumber;
    bufInputCh.Mode = Mode;
    bufInputCh.Parity = Parity;
    bufInputCh.Freq = Freq;

    DeviceIoControl (
        hECE0206
        ,ECE02061_XP_SI_PUSK
        ,&bufInputCh
        ,4
        ,&bufOutput
        ,1
        ,&nOutput
        ,NULL  );

    delay(50);
    //DWORD tick = GetTickCount();
    //while (( GetTickCount()- tick)<300) {};
}

// Пуск выходного канала ========================================== //
void A053watcher::SO_pusk(UCHAR Err_en, UCHAR Parity, UCHAR Freq, UCHAR ArrayDim, UCHAR Delay, UCHAR ArrayNumber)
{
    bufOutputCh.Err_en = Err_en;
    bufOutputCh.Parity = Parity;
    bufOutputCh.Freq = Freq;
    bufOutputCh.ArrayDim = ArrayDim;
    bufOutputCh.Delay = Delay;
    bufOutputCh.ArrayNumber = ArrayNumber;

    DeviceIoControl (
        hECE0206
        ,ECE02061_XP_SO_PUSK1
        ,&bufOutputCh
        ,6
        ,NULL
        ,0
        ,&nOutput
        ,NULL  );

    delay(50);
    //DWORD tick = GetTickCount();
    //while (( GetTickCount()- tick)<300) {};
}

// Чтение входного параметра ======================= //
void A053watcher::readParam(DWORD bffrNum, UCHAR ParamNumber, InputParam &ParamCod)
{
    DeviceIoControl (
        hECE0206
        ,bffrNum
        ,&ParamNumber
        ,1
        ,&ParamCod
        ,9
        ,&nOutput
        ,NULL  );

    //DWORD tick = GetTickCount();
    //while (( GetTickCount()- tick)<300) {};
}

// Остановка приема по каналу ===================== //
void A053watcher::SI_stop(UCHAR ChanNumber)
{
    DeviceIoControl (
        hECE0206
        ,ECE02061_XP_SI_STOP
        ,&ChanNumber
        ,1
        ,&bufOutput
        ,1
        ,&nOutput
        ,NULL  );

    delay(50);
    //DWORD tick = GetTickCount();
    //while (( GetTickCount()- tick)<300) {};
}

// Остановка выдачи ============================== //
void A053watcher::SO_stop(void)
{
    DeviceIoControl (
        hECE0206
        ,ECE02061_XP_SO_STOP1
        ,NULL
        ,0
        ,NULL
        ,0
        ,&nOutput
        ,NULL  );

    delay(50);
    //DWORD tick = GetTickCount();
    //while (( GetTickCount()- tick)<300) {};
}

// Установка режима коротких посылок ============ //
void A053watcher::setShortMode()
{
    DeviceIoControl (
        hECE0206  ,
        ECE02061_XP_SET_SHORT_MODE,
        NULL,
        0 ,
        NULL,
        0 ,
        &nOutput,
        NULL
    );

    delay(50);
}

// Обнуление буферов приема ===================== //
void A053watcher::SI_clear_array(UCHAR ChanNumber)
{
    DeviceIoControl (
        hECE0206
        ,ECE02061_XP_CLEAR_SI_ARRAY
        ,&ChanNumber
        ,1
        ,&bufOutput
        ,1
        ,&nOutput
        ,NULL  );

    //delay(50);
    //DWORD tick = GetTickCount();
    //while (( GetTickCount()- tick)<300) {};
}

// Обнуление буфера выходного канала ============= //
void A053watcher::SO_clear_array(UCHAR Addr, UCHAR Dim)
{
    buffClearOutCh.paramAddress = Addr;
    buffClearOutCh.bufferDimension = Dim;

    DeviceIoControl (
        hECE0206
        ,ECE02061_XP_CLEAR_ARRAY_1
        ,&buffClearOutCh
        ,2
        ,NULL
        ,0
        ,&nOutput
        ,NULL
    );

    delay(50);
}

QString A053watcher::wcharToString(const wchar_t* wstr)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string utf8str = converter.to_bytes(wstr);
    return QString::fromUtf8(utf8str.c_str());
}

void A053watcher::getDataFromArink429Word(Arink429Frame &data, DWORD word)
{
    data.Address     = word & 0xff;     word >>= 8;     // 8
    data.RVcode      = word & 0b11;     word >>= 2;     // 9-10
    data.TestIsAllow = word & 1;        word >>= 1;     // 11
                                        word >>= 1;     // 12
    data.Data        = word & 0xffff;   word >>= 16;    // 13-28
    data.Sign        = word & 1;        word >>= 1;     // 29
    data.Matrix      = word & 0b11;     word >>= 2;     // 30-31
}

void A053watcher::delay(uint32_t ms)
{
    QThread::msleep(ms);
}

void A053watcher::deviceIsLive()
{
    // Чтение показаний таймера
    DWORD deviceTimer{};
    DeviceIoControl (
        hECE0206,
        ECE02061_XP_GET_TIMER,
        NULL,
        0 ,
        &deviceTimer,
        4,
        &nOutput ,
        NULL
    );

    qDebug() << "Device timer:" << deviceTimer * 1024 / 1000;

    if(deviceTimer == 0 || chanNumber == 0)
    {
        stopChannels();
        CloseHandle(hECE0206);
        hECE0206 = INVALID_HANDLE_VALUE;
        tryConnectToA053();
    }
}

