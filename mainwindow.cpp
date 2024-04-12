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

    ui->label_ind_bpi->setStyleSheet(RED_BACKGROUNG);
    ui->label_ind_a053->setStyleSheet(RED_BACKGROUNG);
    ui->button_startRecord->setEnabled(false);
    ui->button_stopRecord->setEnabled(false);
    ui->label_hint_bpi->setText(INVALID_DATA);
    ui->label_hint_a053->setText(INVALID_DATA);

    connect(&bpiWatcher, &BPIwatcher::signalSendBpiData, this, &MainWindow::slotInputBpiData);
    connect(&bpiWatcher, &BPIwatcher::signalSendBpiData, &logger, &Logger::writeBpiData);
    //connect(&a053watcher, &A053watcher::signalSendData, this, &MainWindow::slotInputA053Data);
    //connect(&a053watcher, &A053watcher::signalSendData, &logger, &Logger::writeA053Data);
    //
    connect(this, &MainWindow::signalSwitchStateRecord, &logger, &Logger::slotSwitchStateRecord);
    connect(ui->button_startRecord, SIGNAL(clicked()), this, SLOT(slotStartRecord()));
    connect(ui->button_stopRecord,  SIGNAL(clicked()), this, SLOT(slotStopRecord()));
    connect(&stateControlTimer, &QTimer::timeout, this, &MainWindow::slotStateController);
    connect(&timerShowData, &QTimer::timeout, this, &MainWindow::slotShowData);
    //
    connect(&bpiWatcher, &QThread::finished, &bpiWatcher, &QThread::deleteLater);
    //connect(&a053watcher, &QThread::finished, &a053watcher, &QThread::deleteLater);
    connect(&logger, &QThread::finished, &logger, &QThread::deleteLater);

    timerShowData.start(200);
    stateControlTimer.start(500);
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
        if(recordingIsActive)
        {
            slotInvalidData();
        }
        else
            disableButtons();
        comingDataBpi = true;
        comingDataA053 = true;
        break;

    case NO_DATA_FROM_BPI:
        ui->label_ind_bpi->setStyleSheet(RED_BACKGROUNG);
        ui->label_ind_a053->setStyleSheet(GREEN_BACKGROUNG);
        ui->label_hint_bpi->setText(INVALID_DATA);
        ui->label_hint_a053->setText(VALID_DATA);
        if(recordingIsActive)
        {
            slotInvalidData();
        }
        else
            disableButtons();
        comingDataBpi = false;
        comingDataA053 = true;
        break;

    case NO_DATA_FROM_A053:
        ui->label_ind_bpi->setStyleSheet(GREEN_BACKGROUNG);
        ui->label_ind_a053->setStyleSheet(RED_BACKGROUNG);
        ui->label_hint_bpi->setText(VALID_DATA);
        ui->label_hint_a053->setText(INVALID_DATA);
        if(recordingIsActive)
        {
            slotInvalidData();
        }
        else
            disableButtons();

        comingDataBpi = true;
        comingDataA053 = false;
        break;

    case DATA_IS_SUCESS:
        if(!recordingIsActive)
        {
            ui->button_startRecord->setEnabled(true);
            ui->button_stopRecord->setEnabled(false);
            ui->label_ind_bpi->setStyleSheet(GREEN_BACKGROUNG);
            ui->label_ind_a053->setStyleSheet(GREEN_BACKGROUNG);
            ui->label_hint_bpi->setText(VALID_DATA);
            ui->label_hint_a053->setText(VALID_DATA);
        }
        comingDataBpi = comingDataA053 = true;
        break;
    }

    prevNumberBpiFrm = counterBpiFrames;
    prevNumberA053Frame = counterA053Frames;
}

void MainWindow::slotStartRecord()
{
    recordingIsActive = true;
    ui->button_startRecord->setEnabled(false);
    ui->button_stopRecord->setEnabled(true);
    emit signalSwitchStateRecord(true);
}

void MainWindow::slotStopRecord()
{
    recordingIsActive = false;
    ui->button_startRecord->setEnabled(true);
    ui->button_stopRecord->setEnabled(false);
    emit signalSwitchStateRecord(false);
}

void MainWindow::slotInvalidData()
{
    recordingIsActive = false;
    ui->button_startRecord->setEnabled(false);
    ui->button_stopRecord->setEnabled(false);
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

void MainWindow::disableButtons()
{
    ui->button_startRecord->setEnabled(false);
    ui->button_stopRecord->setEnabled(false);
}

