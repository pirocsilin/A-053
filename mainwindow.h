#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define RED_BACKGROUNG   "background-color: rgb(255, 0, 0); border: 1 solid black;"
#define GREEN_BACKGROUNG "background-color: rgb(0, 255, 0); border: 1 solid black;"
#define VALID_DATA       "[Данные поступают]"
#define INVALID_DATA     "[Данные не поступают]"

#include <QMainWindow>
#include <QTimer>

#include "bpiwatcher.h"
#include "common.h"
#include "logger.h"
//#include "a053watcher.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    BPIwatcher bpiWatcher;
    //A053watcher a053watcher;
    Logger logger;

    enum stateInputData : uint8_t
    {
        NO_DATA_ALL      = 0,  // Нет данных от обоих приборов
        NO_DATA_FROM_BPI = 1,  // Нет данных от БПИ
        NO_DATA_FROM_A053= 2,  // Нет данных от A-053
        DATA_IS_SUCESS   = 3   // Данные с обоих приборов поступают
    };

    quint64 counterBpiFrames    {0};
    quint64 prevNumberBpiFrm    {0};
    quint64 counterA053Frames   {0};
    quint64 prevNumberA053Frame {0};

    bool recordingIsActive  {false};
    bool comingDataBpi      {false};
    bool comingDataA053     {false};
    QTimer timerStateControl;
    QTimer timerShowData;

    BpiData lastBpiData {};
    DWORD   lastA053Data{};

    void invalidData();

public slots:
   void slotInputBpiData(BpiData data);
   void slotInputA053Data(DWORD data);
   void slotStateController();
   void slotShowData();
   void slotRecordController();

signals:
    void signalStopRecord();
    void signalStartRecord();
    void signalSwitchStateRecord(bool state);

};
#endif // MAINWINDOW_H
