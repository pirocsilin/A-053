#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaType<BpiData>("BpiData");
    qRegisterMetaType<DWORD>("DWORD");

    slotStateController();

    connect(&bpiWatcher, &BPIwatcher::signalSendBpiData, this, &MainWindow::slotInputBpiData);
    connect(&bpiWatcher, &BPIwatcher::signalSendBpiData, &logger, &Logger::writeBpiData);
    //connect(&a053watcher, &A053watcher::signalSendData, this, &MainWindow::slotInputA053Data);
    //connect(&a053watcher, &A053watcher::signalSendData, &logger, &Logger::writeA053Data);
    //
    connect(this, &MainWindow::signalSwitchStateRecord, &logger, &Logger::slotSwitchStateRecord);
    connect(ui->button_startRecord, SIGNAL(clicked()), this, SLOT(slotRecordController()));

    connect(&timerStateControl, &QTimer::timeout, this, &MainWindow::slotStateController);
    connect(&timerShowData, &QTimer::timeout, this, &MainWindow::slotShowData);
    //
    connect(&bpiWatcher, &QThread::finished, &bpiWatcher, &QThread::deleteLater);
    //connect(&a053watcher, &QThread::finished, &a053watcher, &QThread::deleteLater);
    connect(&logger, &QThread::finished, &logger, &QThread::deleteLater);

    timerShowData.start(200);
    timerStateControl.start(500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotInputBpiData(BpiData data)
{
    lastBpiData = data;
    counterBpiFrames++;
}

void MainWindow::slotInputA053Data(DWORD data)
{
    lastA053Data = data;
    counterA053Frames++;
}

void MainWindow::slotStateController()
{
    quint8 stateData{};
    stateData |= prevNumberBpiFrm != counterBpiFrames;
    stateData <<= 1;
    stateData |= prevNumberA053Frame != counterA053Frames;

    switch (stateData)
    {
    case NO_DATA_ALL:
        ui->label_ind_bpi->setStyleSheet(RED_BACKGROUNG);
        ui->label_ind_a053->setStyleSheet(RED_BACKGROUNG);
        ui->label_hint_bpi->setText(INVALID_DATA);
        ui->label_hint_a053->setText(INVALID_DATA);
        //
        ui->button_startRecord->setFixedWidth(250);
        ui->button_startRecord->setText("Нет данных от BPI, A053");
        ui->button_startRecord->setEnabled(false);
        //
        if(recordingIsActive) invalidData();
        //
        comingDataBpi = false;
        comingDataA053 = false;
        break;

    case NO_DATA_FROM_BPI:
        ui->label_ind_bpi->setStyleSheet(RED_BACKGROUNG);
        ui->label_ind_a053->setStyleSheet(GREEN_BACKGROUNG);
        ui->label_hint_bpi->setText(INVALID_DATA);
        ui->label_hint_a053->setText(VALID_DATA);
        //
        ui->button_startRecord->setFixedWidth(200);
        ui->button_startRecord->setText("Нет данных от BPI");
        ui->button_startRecord->setEnabled(false);
        //
        if(recordingIsActive) invalidData();
        //
        comingDataBpi = false;
        comingDataA053 = true;
        break;

    case NO_DATA_FROM_A053:{
        ui->label_ind_bpi->setStyleSheet(GREEN_BACKGROUNG);
        ui->label_ind_a053->setStyleSheet(RED_BACKGROUNG);
        ui->label_hint_bpi->setText(VALID_DATA);
        ui->label_hint_a053->setText(INVALID_DATA);
        //
        /*ui->button_startRecord->setFixedWidth(215);
        ui->button_startRecord->setText("Нет данных от A-053");
        ui->button_startRecord->setEnabled(false);
        //
        if(recordingIsActive) invalidData();
        //
        comingDataBpi = true;
        comingDataA053 = false;*/

        ui->button_startRecord->setEnabled(true);
        ui->button_startRecord->setFixedWidth(210);
        QString text = !recordingIsActive ? "Начать запись" :
                                            "Остановить запись";
        ui->button_startRecord->setText(text);
        //
        comingDataBpi = comingDataA053 = true;

        break;
    }
    case DATA_IS_SUCESS:
        ui->label_ind_bpi->setStyleSheet(GREEN_BACKGROUNG);
        ui->label_ind_a053->setStyleSheet(GREEN_BACKGROUNG);
        ui->label_hint_bpi->setText(VALID_DATA);
        ui->label_hint_a053->setText(VALID_DATA);
        //
        ui->button_startRecord->setEnabled(true);
        ui->button_startRecord->setFixedWidth(210);
        QString text = !recordingIsActive ? "Начать запись" :
                                            "Остановить запись";
        ui->button_startRecord->setText(text);
        //
        comingDataBpi = comingDataA053 = true;
        break;
    }

    prevNumberBpiFrm = counterBpiFrames;
    prevNumberA053Frame = counterA053Frames;
}

void MainWindow::slotRecordController()
{
    if(!recordingIsActive)
    {
        recordingIsActive = true;
        ui->button_startRecord->setFixedWidth(210);
        ui->button_startRecord->setText("Остановить запись");
        //
        emit signalSwitchStateRecord(true);
    }
    else
    {
        recordingIsActive = false;
        ui->button_startRecord->setFixedWidth(210);
        ui->button_startRecord->setText("Начать запись");
        //
        emit signalSwitchStateRecord(false);
    }
}

void MainWindow::invalidData()
{
    recordingIsActive = false;
    emit signalSwitchStateRecord(false);
}

void MainWindow::slotShowData()
{
    QTime currentTime = QTime::currentTime();
    //
    if(comingDataBpi)
    {
        ui->lineEdit_time_bpi->setText(currentTime.toString("hh:mm:ss:zzz"));
        ui->lineEdit_alt_bpi->setText(QString::number(lastBpiData.h));
        ui->lineEdit_kren->setText(QString::number(lastBpiData.roll));
        ui->lineEdit_tangaj->setText(QString::number(lastBpiData.pitch));
        ui->lineEdit_course->setText(QString::number(lastBpiData.head));
        ui->lineEdit_long->setText(QString::number(lastBpiData.B));
        ui->lineEdit_lat->setText(QString::number(lastBpiData.L));
    }
    //
    if(comingDataA053)
    {
        ui->lineEdit_time_a053->setText(currentTime.toString("hh:mm:ss:zzz"));
        ui->lineEdit_alt_a053->setText(QString::number(lastA053Data));
    }
}

