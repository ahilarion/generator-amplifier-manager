
#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QFileDialog>
#include <QTimeZone>
#include <QProcess>
#include <QInputDialog>
#include <QDesktopServices>

#include "login.h"
#include "Amplifier.h"
#include "Generator.h"
#include "XPort.h"
#include "settings.h"
#include "CSVWriter.h"

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class window; }
QT_END_NAMESPACE

class window : public QMainWindow

{
    Q_OBJECT

public:
    window(QWidget *parent = nullptr);
    int checkCycleAvailable(QTableWidget *table, quint64 timestamp);
    ~window();

private slots:
    void check();
    void cycleRefresh();
    void on_pushButton_auto_add_clicked();
    void on_pushButton_auto_delete_clicked();
    void on_pushButton_auto_export_clicked();
    void on_pushButton_auto_import_clicked();
    void on_spinBox_auto_frequency_valueChanged(int value);
    void on_pushButton_manual_clicked();
    void on_pushButton_auto_clicked();
    void on_spinBox_manual_frequency_valueChanged(int value);
    void on_pushButton_manual_diode_1_A_clicked();
    void on_pushButton_manual_diode_1_B_clicked();
    void on_pushButton_manual_diode_2_A_clicked();
    void on_pushButton_manual_diode_2_B_clicked();
    void on_pushButton_manual_send_clicked();
    void on_pushButton_manual_output_toggled(bool checked);
    void on_actionSe_reconnecter_triggered();
    void on_actionChanger_la_p_riode_de_sauvegarde_triggered();
    void on_pushButton_auto_play_stop_toggled(bool checked);
    void on_pushButton_auto_paste_clicked();
    void on_actionChanger_le_rep_rtoire_de_sauvegarde_triggered();
    void on_actionOuvrir_le_rep_rtoire_de_sauvegarde_triggered();
    void on_actionChanger_le_rep_rtoire_des_presets_triggered();

private:
    Ui::window *ui;
    QTimer *timer, *cycle;
    QFile *file;
    Login *login;
    Amplifier *amplifier4G, *amplifier5G;
    XPort *xport;
    Generator *generator;
    Settings *config;
    QDir *directory;
    CSVWriter *writer;
    QLabel *statusLabel;

    QList<QTableWidgetItem *> selectedItems;
    QTime currentTime, cycleDurationTime, newItemDuration, existingItemDuration;
    QDateTime cycleStartTime, newItemStart, newItemEnd, existingItemStart, existingItemEnd;
    QJsonArray itemsArray;
    QJsonObject itemObject;
    QJsonDocument itemsDoc;
    QString filePath, ipGenerator, ipAmplifier4G, ipAmplifier5G, ipXPort, saveURL, selectedDirectory, directoryPath, presetsURL, saveFilePath, errorLoginMsg;
    QByteArray jsonData;
    QMessageBox::StandardButton deleteReply;
    QUrl directoryUrl;

    int elapsedDuration, totalDuration, rowCount, remainingDuration, generatorFrequency, channelValue[6], generatorAmplitude, savingTime, currentSavingTime, cycleFrequency, cycleChannelValue[6], newSavingTime, indexCycleAvailable;
    bool cycleAlreadyRunning, isAccepted, isConnected, isGeneratorLaunched, isAutoLaunched, ok;
    quint64 cycleStartTimeStamp, cycleEndTimeStamp, currentTimeStamp, startTimeStamp, endTimeStamp;
    ushort portGenerator, portAmplifier4G, portAmplifier5G, portXPort;
};

#endif // WINDOW_H
