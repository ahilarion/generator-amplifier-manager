#include "window.h"
#include "ui_window.h"


window::window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::window)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    cycle = new QTimer(this);
    generator = new Generator();
    amplifier4G = new Amplifier();
    amplifier5G = new Amplifier();
    xport = new XPort();
    login = new Login();
    config = new Settings();
    writer = new CSVWriter();

    connect(timer, SIGNAL(timeout()), this, SLOT(check()));
    connect(cycle, SIGNAL(timeout()), this, SLOT(cycleRefresh()));

    cycleAlreadyRunning = false;
    isAccepted = false;
    isConnected = false;
    isGeneratorLaunched = false;
    isAutoLaunched = false;
    ok = false;
    remainingDuration = 0;
    elapsedDuration = 0;
    totalDuration = 0;
    indexCycleAvailable = 0;
    generatorFrequency = 900;
    generatorAmplitude = 10;
    savingTime = 10;
    newSavingTime = 0;
    currentSavingTime = savingTime;
    saveURL = "./saves/";
    presetsURL = "./presets/";
    errorLoginMsg = "";

    do {
        isAccepted = login->exec();

        if(isAccepted == QDialog::Accepted) {
            ipGenerator = login->getGeneratorIp();
            ipAmplifier4G = login->getAmplifier4GIp();
            ipAmplifier5G = login->getAmplifier5GIp();
            ipXPort = login->getXPortIp();

            portGenerator = login->getGeneratorPort();
            portAmplifier4G = login->getAmplifier4GPort();
            portAmplifier5G = login->getAmplifier5GPort();
            portXPort = login->getXPortPort();

            isConnected = generator->join(ipGenerator, portGenerator);
            isConnected &= amplifier4G->join(ipAmplifier4G, portAmplifier4G);
            isConnected &= amplifier5G->join(ipAmplifier5G, portAmplifier5G);
            isConnected &= xport->join(ipXPort, portXPort);

            if(!isConnected) {
                errorLoginMsg.clear();

                !generator->getConnectionStatus() ? errorLoginMsg += "générateur \n" : errorLoginMsg;
                !amplifier4G->getConnectionStatus() ? errorLoginMsg += "amplificateur 4G \n" : errorLoginMsg;
                !amplifier5G->getConnectionStatus() ? errorLoginMsg += "amplificateur 5G\n" : errorLoginMsg;
                !xport->getConnectionStatus() ? errorLoginMsg += "module xport \n" : errorLoginMsg;

                generator->disconnect();
                amplifier4G->disconnect();
                amplifier5G->disconnect();
                xport->disconnect();

                QMessageBox::warning(this, "Erreur", "La connexion a échouée avec les éléments suivants : \n" + errorLoginMsg, "Réessayer");
            }
        } else if (isAccepted == QDialog::Rejected)
            exit(0);
    } while(!isConnected);

    config->setValue("ip_generator", ipGenerator);
    config->setValue("ip_amplifier_4G", ipAmplifier4G);
    config->setValue("ip_amplifier_5G", ipAmplifier5G);
    config->setValue("ip_xport", ipXPort);
    config->setValue("port_generator", portGenerator);
    config->setValue("port_amplifier_4G", portAmplifier4G);
    config->setValue("port_amplifier_5G", portAmplifier5G);
    config->setValue("port_xport", portXPort);

    generatorFrequency = generator->getGeneratorFrequency();
    generatorAmplitude = generator->getGeneratorAmplitude();
    isGeneratorLaunched = generator->getGeneratorOutput();
    saveURL = config->getValue("saves_url", "./saves");
    presetsURL = config->getValue("presets_url", "./presets");

    if(config->getValue("diode_1", "A") == "B")
        ui->pushButton_manual_diode_1_B->click();
    if(config->getValue("diode_2", "A") == "B")
        ui->pushButton_manual_diode_2_B->click();

    ui->spinBox_manual_channel_1_4G->setValue(config->getValue("channel_1_4G", 0).toInt());
    ui->spinBox_manual_channel_2_4G->setValue(config->getValue("channel_2_4G", 0).toInt());
    ui->spinBox_manual_channel_3_4G->setValue(config->getValue("channel_3_4G", 0).toInt());
    ui->spinBox_manual_channel_4_4G->setValue(config->getValue("channel_4_4G", 0).toInt());
    ui->spinBox_manual_channel_5_4G->setValue(config->getValue("channel_5_4G", 0).toInt());
    ui->spinBox_manual_channel_6_4G->setValue(config->getValue("channel_6_4G", 0).toInt());
    ui->spinBox_manual_channel_1_5G->setValue(config->getValue("channel_1_5G", 0).toInt());
    ui->spinBox_manual_channel_2_5G->setValue(config->getValue("channel_2_5G", 0).toInt());
    ui->spinBox_manual_channel_3_5G->setValue(config->getValue("channel_3_5G", 0).toInt());
    ui->spinBox_manual_channel_4_5G->setValue(config->getValue("channel_4_5G", 0).toInt());
    ui->pushButton_manual_output->setChecked(isGeneratorLaunched);
    ui->spinBox_manual_frequency->setValue(generatorFrequency);
    ui->spinBox_manual_amplitude->setValue(generatorAmplitude);
    ui->tableWidget_auto_cycleList->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->lcdNumber_clock->display(QTime::currentTime().toString());

    ui->pushButton_manual->setIcon(QIcon(":/img/manual.png"));
    ui->pushButton_auto->setIcon(QIcon(":/img/automatique.png"));
    ui->pushButton_manual->setIconSize(QSize(50,50));
    ui->pushButton_auto->setIconSize(QSize(50,50));

    ui->pushButton_auto_play_stop->setIcon(QIcon(":/img/start.png"));
    ui->pushButton_auto_play_stop->setIconSize(QSize(50,50));

    statusBar()->removeWidget(statusLabel);

    statusLabel = new QLabel("Clients connectés | Générateur : [" + ipGenerator + ":" + QString::number(portGenerator) + "] - Amplificateur 4G : [" + ipAmplifier4G + ":" + QString::number(portAmplifier4G) + "] - Amplificateur 5G : [" + ipAmplifier5G + ":" + QString::number(portAmplifier5G) + "] - XPort : [" + ipXPort + ":" + QString::number(portXPort)+ "]");
    statusBar()->addWidget(statusLabel);

    statusBar()->resize(1000,15);
    statusBar()->setStyleSheet("font: 500; color: #666; background-color : lightgrey");


    timer->start(1000);
}

window::~window()
{
    generator->setGeneratorOutput(0);

    generator->disconnect();
    amplifier4G->disconnect();
    amplifier5G->disconnect();
    xport->disconnect();

    delete config;
    delete statusLabel;
    delete directory;
    delete writer;
    delete generator;
    delete xport;
    delete amplifier4G;
    delete amplifier5G;
    delete login;
    delete cycle;
    delete timer;
    delete file;
    delete ui;

    exit(0);
}

void window::check() {
    if(!(generator->getConnectionStatus() && amplifier4G->getConnectionStatus() && amplifier5G->getConnectionStatus() && xport->getConnectionStatus())) {
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        exit(0);
    }


    ui->lcdNumber_clock->display(QTime::currentTime().toString());

    if(isGeneratorLaunched) {
        generatorFrequency >= 2600 ? amplifier5G->init() : amplifier4G->init();

        for (int i = 0; i < (generatorFrequency >= 2600 ? 4: 6); ++i) {
            if(generatorFrequency >= 2600) {
                amplifier5G->sendRequestToGetChannelValue(i + 1);
            } else {
                amplifier4G->sendRequestToGetChannelValue(i + 1);
            }
        }
    }

    if(generatorFrequency >= 2600 && isGeneratorLaunched) {
        ui->spinBox_manual_channel_1_5G_display->setValue(amplifier5G->getChannelValue(0));
        ui->spinBox_manual_channel_2_5G_display->setValue(amplifier5G->getChannelValue(1));
        ui->spinBox_manual_channel_3_5G_display->setValue(amplifier5G->getChannelValue(2));
        ui->spinBox_manual_channel_4_5G_display->setValue(amplifier5G->getChannelValue(3));

        ui->spinBox_manual_channel_1_4G_display->setValue(0);
        ui->spinBox_manual_channel_2_4G_display->setValue(0);
        ui->spinBox_manual_channel_3_4G_display->setValue(0);
        ui->spinBox_manual_channel_4_4G_display->setValue(0);
        ui->spinBox_manual_channel_5_4G_display->setValue(0);
        ui->spinBox_manual_channel_6_4G_display->setValue(0);
    } else if (generatorFrequency < 2600 && isGeneratorLaunched){
        ui->spinBox_manual_channel_1_4G_display->setValue(amplifier4G->getChannelValue(0));
        ui->spinBox_manual_channel_2_4G_display->setValue(amplifier4G->getChannelValue(1));
        ui->spinBox_manual_channel_3_4G_display->setValue(amplifier4G->getChannelValue(2));
        ui->spinBox_manual_channel_4_4G_display->setValue(amplifier4G->getChannelValue(3));
        ui->spinBox_manual_channel_5_4G_display->setValue(amplifier4G->getChannelValue(4));
        ui->spinBox_manual_channel_6_4G_display->setValue(amplifier4G->getChannelValue(5));

        ui->spinBox_manual_channel_1_5G_display->setValue(0);
        ui->spinBox_manual_channel_2_5G_display->setValue(0);
        ui->spinBox_manual_channel_3_5G_display->setValue(0);
        ui->spinBox_manual_channel_4_5G_display->setValue(0);
    } else {
        ui->spinBox_manual_channel_1_4G_display->setValue(0);
        ui->spinBox_manual_channel_2_4G_display->setValue(0);
        ui->spinBox_manual_channel_3_4G_display->setValue(0);
        ui->spinBox_manual_channel_4_4G_display->setValue(0);
        ui->spinBox_manual_channel_5_4G_display->setValue(0);
        ui->spinBox_manual_channel_6_4G_display->setValue(0);

        ui->spinBox_manual_channel_1_5G_display->setValue(0);
        ui->spinBox_manual_channel_2_5G_display->setValue(0);
        ui->spinBox_manual_channel_3_5G_display->setValue(0);
        ui->spinBox_manual_channel_4_5G_display->setValue(0);
    }

    if(isAutoLaunched) {
        currentTimeStamp = QDateTime::currentDateTime().toSecsSinceEpoch();
        indexCycleAvailable = checkCycleAvailable(ui->tableWidget_auto_cycleList, currentTimeStamp);

        if(indexCycleAvailable != -1 && cycleAlreadyRunning == false) {
            cycleStartTime = QDateTime::currentDateTime();
            cycleDurationTime = QTime::fromString(ui->tableWidget_auto_cycleList->item(indexCycleAvailable, 1)->text());

            cycleStartTime.setTime(QTime::fromString(ui->tableWidget_auto_cycleList->item(indexCycleAvailable, 0)->text()));

            cycleFrequency = ui->tableWidget_auto_cycleList->item(indexCycleAvailable, 2)->text().toInt();

            if(cycleFrequency >= 2600) {
                xport->setXPortOutput(1);
                amplifier5G->init();

                if(generatorFrequency >= 2600 && generatorFrequency < 3350)
                    amplifier5G->setFrequencyBand(1);
                else if (generatorFrequency >= 3350 && generatorFrequency < 3650)
                    amplifier5G->setFrequencyBand(2);
                else if (generatorFrequency >= 3650 && generatorFrequency <= 4000)
                    amplifier5G->setFrequencyBand(3);
            } else {
                xport->setXPortOutput(0);
                amplifier4G->init();

                if(cycleFrequency <= 800)
                    amplifier4G->setFrequencyBand(1);
                else if (cycleFrequency <= 1350)
                    amplifier4G->setFrequencyBand(2);
                else if (cycleFrequency <= 1985)
                    amplifier4G->setFrequencyBand(3);
                else if (cycleFrequency <= 2310)
                    amplifier4G->setFrequencyBand(4);
                else if (cycleFrequency <= 2525)
                    amplifier4G->setFrequencyBand(5);
                else if (cycleFrequency < 2600)
                    amplifier4G->setFrequencyBand(6);
            }

            for (int i = 0; i < (cycleFrequency >= 2600 ? 4: 6); ++i) {
                cycleChannelValue[i] = ui->tableWidget_auto_cycleList->item(indexCycleAvailable, 3 + i)->text().toInt();
                if(cycleFrequency >= 2600) {
                    amplifier5G->setChannelValue(1+i, cycleChannelValue[i]);
                } else {
                    amplifier4G->setChannelValue(1+i, cycleChannelValue[i]);
                }
            }

            cycleStartTimeStamp = cycleStartTime.toSecsSinceEpoch();
            cycleEndTimeStamp = cycleStartTime.toSecsSinceEpoch() + (cycleDurationTime.hour()*3600) + cycleDurationTime.minute() * 60;

            ui->progressBar_auto_cycle->setMaximum((cycleDurationTime.hour()*3600 + cycleDurationTime.minute() * 60) - 1);

            elapsedDuration = (cycleDurationTime.hour()*3600 + cycleDurationTime.minute() * 60) - (cycleEndTimeStamp - currentTimeStamp);

            writer->close();
            saveFilePath =  saveURL + "/" + QDate::currentDate().toString("yyyy/MMMM");
            writer->create(saveFilePath, "Sauvegarde du " + QDate::currentDate().toString("dddd dd") + " de " + cycleStartTime.toString("hh'h'mm") + " à " + QDateTime::fromSecsSinceEpoch(cycleEndTimeStamp).toString("hh'h'mm") + ".csv");

            if(cycleFrequency >= 2600)
                writer->write({"Heure de début", "Durée", "Fréquence", "Voie 1", "Voie 2", "Voie 3", "Voie 4", "Puissance V1", "Puissance V2", "Puissance V3", "Puissance V4"});
            else
                writer->write({"Heure de début", "Durée", "Fréquence", "Voie 1", "Voie 2", "Voie 3", "Voie 4", "Voie 5", "Voie 6", "Puissance V1", "Puissance V2", "Puissance V3", "Puissance V4", "Puissance V5", "Puissance V6"});

            cycleAlreadyRunning = true;
            generator->setGeneratorOutput(1);
            generator->setGeneratorFrequency(cycleFrequency);
            cycle->start(1000);
        }
    }
}

void window::cycleRefresh() {
    if (currentTimeStamp >= cycleEndTimeStamp || !isAutoLaunched) {
        ui->progressBar_auto_cycle->setValue(0);
        cycleAlreadyRunning = false;
        elapsedDuration = 0;
        generator->setGeneratorOutput(0);
        writer->close();
        cycle->stop();
    } else {
        cycleFrequency >= 2600 ? amplifier5G->init() : amplifier4G->init();

        for (int i = 0; i < (cycleFrequency >= 2600 ? 4: 6); ++i) {
            if(cycleFrequency >= 2600) {
                amplifier5G->sendRequestToGetChannelValue(i + 1);
                channelValue[i] = amplifier5G->getChannelValue(i);
            } else {
                amplifier4G->sendRequestToGetChannelValue(i + 1);
                channelValue[i] = amplifier4G->getChannelValue(i);
            }
        }
        if(currentSavingTime <= 0) {
            if(cycleFrequency >= 2600) {
                writer->write({cycleStartTime.toString("hh:mm"), cycleDurationTime.toString("hh:mm"), QString::number(cycleFrequency), QString::number(cycleChannelValue[0]), QString::number(cycleChannelValue[1]), QString::number(cycleChannelValue[2]), QString::number(cycleChannelValue[3]), QString::number(channelValue[0]), QString::number(channelValue[1]), QString::number(channelValue[2]), QString::number(channelValue[3])});
            } else {
                writer->write({cycleStartTime.toString("hh:mm"), cycleDurationTime.toString("hh:mm"), QString::number(cycleFrequency), QString::number(cycleChannelValue[0]), QString::number(cycleChannelValue[1]), QString::number(cycleChannelValue[2]), QString::number(cycleChannelValue[3]), QString::number(cycleChannelValue[4]), QString::number(cycleChannelValue[5]), QString::number(channelValue[0]), QString::number(channelValue[1]), QString::number(channelValue[2]), QString::number(channelValue[3]), QString::number(channelValue[4]), QString::number(channelValue[5])});
            }
            currentSavingTime = savingTime;
        }
        ui->progressBar_auto_cycle->setValue(elapsedDuration);
        elapsedDuration++;
        currentSavingTime--;
    }
}

int window::checkCycleAvailable(QTableWidget *table, quint64 timestamp) {
    if(table->rowCount() >= 1) {
        for(int i = 0; i < table->rowCount(); i++) {
            cycleStartTime = QDateTime::currentDateTime();
            cycleDurationTime = QTime::fromString(table->item(i, 1)->text());

            cycleStartTime.setTime(QTime::fromString(table->item(i, 0)->text()));

            startTimeStamp = cycleStartTime.toSecsSinceEpoch();
            endTimeStamp = cycleStartTime.toSecsSinceEpoch() + (cycleDurationTime.hour()*3600) + cycleDurationTime.minute() * 60;

            if(timestamp >= startTimeStamp && timestamp < endTimeStamp)
                return i;
        }
    }

    return -1;
}

void window::on_pushButton_auto_add_clicked()
{
    newItemDuration = QTime::fromString(ui->timeEdit_auto_durationTime->text().replace(" h ", ":").replace(" m", ""));
    newItemStart = QDateTime::currentDateTime();

    newItemStart.setTime(QTime::fromString(ui->timeEdit_auto_startTime->text().replace(" h ", ":").replace(" m", "")));

    if(newItemDuration == QTime(0, 0)) {
        QMessageBox::warning(this, "Erreur", "La durée ne peut pas être égale à 00h00.");
        return;
    }

    newItemEnd = newItemStart.addSecs(newItemDuration.hour()*3600 + newItemDuration.minute()*60);

    for(int i = 0; i < ui->tableWidget_auto_cycleList->rowCount(); i++) {
        existingItemStart = QDateTime::currentDateTime();
        existingItemStart.setTime(QTime::fromString(ui->tableWidget_auto_cycleList->item(i, 0)->text()));
        existingItemDuration = QTime::fromString(ui->tableWidget_auto_cycleList->item(i, 1)->text().replace(" h ", ":").replace(" m", ""));
        existingItemEnd = existingItemStart.addSecs(existingItemDuration.hour()*3600 + existingItemDuration.minute()*60);

        if(newItemStart < existingItemEnd && existingItemStart < newItemEnd) {
            QMessageBox::warning(this, "Erreur", "Les horaires se chevauchent.");
            return;
        }
    }


    ui->tableWidget_auto_cycleList->insertRow(ui->tableWidget_auto_cycleList->rowCount());
    ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 0, new QTableWidgetItem(ui->timeEdit_auto_startTime->text().replace(" h ", ":").replace(" m", "")));
    ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 1, new QTableWidgetItem(ui->timeEdit_auto_durationTime->text().replace(" h ", ":").replace(" m", "")));
    ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 2, new QTableWidgetItem(QString::number(ui->spinBox_auto_frequency->value())));

    if(ui->spinBox_auto_frequency->value() >= 2600) {
        ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 3, new QTableWidgetItem(QString::number(ui->spinBox_auto_channel_1_5G->value())));
        ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 4, new QTableWidgetItem(QString::number(ui->spinBox_auto_channel_2_5G->value())));
        ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 5, new QTableWidgetItem(QString::number(ui->spinBox_auto_channel_3_5G->value())));
        ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 6, new QTableWidgetItem(QString::number(ui->spinBox_auto_channel_4_5G->value())));
        ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 7, new QTableWidgetItem("N/A"));
        ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 8, new QTableWidgetItem("N/A"));
    } else {
        ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 3, new QTableWidgetItem(QString::number(ui->spinBox_auto_channel_1_4G->value())));
        ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 4, new QTableWidgetItem(QString::number(ui->spinBox_auto_channel_2_4G->value())));
        ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 5, new QTableWidgetItem(QString::number(ui->spinBox_auto_channel_3_4G->value())));
        ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 6, new QTableWidgetItem(QString::number(ui->spinBox_auto_channel_4_4G->value())));
        ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 7, new QTableWidgetItem(QString::number(ui->spinBox_auto_channel_5_4G->value())));
        ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 8, new QTableWidgetItem(QString::number(ui->spinBox_auto_channel_6_4G->value())));
    }
    
    ui->tableWidget_auto_cycleList->sortByColumn(0, Qt::AscendingOrder);
}


void window::on_pushButton_auto_delete_clicked()
{
    selectedItems = ui->tableWidget_auto_cycleList->selectedItems();
    deleteReply = QMessageBox::information(this, "Confirmation", "Êtes-vous sûr de vouloir supprimer les lignes selectionnés ?", QMessageBox::Yes | QMessageBox::No);

    if(deleteReply == QMessageBox::Yes) {
        foreach(QTableWidgetItem *item, selectedItems) {
            if(item->row() != indexCycleAvailable)
                ui->tableWidget_auto_cycleList->removeRow(item->row());
        }
    }
}



void window::on_pushButton_auto_export_clicked()
{
    for (int i = 0; i < ui->tableWidget_auto_cycleList->rowCount(); i++) {
        itemObject["cycle_start"] = ui->tableWidget_auto_cycleList->item(i, 0)->text();
        itemObject["cycle_duration"] = ui->tableWidget_auto_cycleList->item(i, 1)->text();
        itemObject["cycle_frequency"] = ui->tableWidget_auto_cycleList->item(i, 2)->text();
        itemObject["cycle_channel_1"] = ui->tableWidget_auto_cycleList->item(i, 3)->text();
        itemObject["cycle_channel_2"] = ui->tableWidget_auto_cycleList->item(i, 4)->text();
        itemObject["cycle_channel_3"] = ui->tableWidget_auto_cycleList->item(i, 5)->text();
        itemObject["cycle_channel_4"] = ui->tableWidget_auto_cycleList->item(i, 6)->text();
        itemObject["cycle_channel_5"] = ui->tableWidget_auto_cycleList->item(i, 7)->text();
        itemObject["cycle_channel_6"] = ui->tableWidget_auto_cycleList->item(i, 8)->text();

        itemsArray.append(itemObject);
    }

    itemsDoc = QJsonDocument(itemsArray);

    QDir folder(presetsURL);

    if (!folder.exists()) {
        folder.mkpath(".");
    }

    filePath = QFileDialog::getSaveFileName(this, "Enregistrer le fichier JSON", presetsURL, "Fichier JSON (*.json)");

    if (!filePath.isEmpty()) {
        file = new QFile(filePath);

        if (file->open(QIODevice::WriteOnly)) {
            file->write(itemsDoc.toJson());
            filePath.clear();
            file->close();
        }
    }
}


void window::on_pushButton_auto_import_clicked()
{
    QDir folder(presetsURL);

    if (!folder.exists()) {
        folder.mkpath(".");
    }

    filePath = QFileDialog::getOpenFileName(this, "Ouvrir le fichier JSON", presetsURL, "Fichier JSON (*.json)");

    if (!filePath.isEmpty()) {
        file = new QFile(filePath);
        if (file->open(QIODevice::ReadOnly)) {
            jsonData = file->readAll();
            itemsDoc = QJsonDocument::fromJson(jsonData);
            
            rowCount = ui->tableWidget_auto_cycleList->rowCount();
            for (int i = rowCount - 1; i >= 0; i--) {
                ui->tableWidget_auto_cycleList->removeRow(i);
            }

            itemsArray = itemsDoc.array();

            for (int i = 0; i < itemsArray.size(); i++) {
                itemObject = itemsArray[i].toObject();
                
                ui->tableWidget_auto_cycleList->insertRow(ui->tableWidget_auto_cycleList->rowCount());
                ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 0, new QTableWidgetItem(itemObject["cycle_start"].toString()));
                ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 1, new QTableWidgetItem(itemObject["cycle_duration"].toString()));
                ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 2, new QTableWidgetItem(itemObject["cycle_frequency"].toString()));
                ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 3, new QTableWidgetItem(itemObject["cycle_channel_1"].toString()));
                ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 4, new QTableWidgetItem(itemObject["cycle_channel_2"].toString()));
                ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 5, new QTableWidgetItem(itemObject["cycle_channel_3"].toString()));
                ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 6, new QTableWidgetItem(itemObject["cycle_channel_4"].toString()));
                ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 7, new QTableWidgetItem(itemObject["cycle_channel_5"].toString()));
                ui->tableWidget_auto_cycleList->setItem(ui->tableWidget_auto_cycleList->rowCount()-1, 8, new QTableWidgetItem(itemObject["cycle_channel_6"].toString()));
            }

            filePath.clear();
            file->close();
        }
    }
}


void window::on_spinBox_auto_frequency_valueChanged(int value)
{
    if(value >= 2600)
        ui->stackedWidget_auto_frequency->setCurrentIndex(1);
    else
        ui->stackedWidget_auto_frequency->setCurrentIndex(0);
}


void window::on_pushButton_manual_clicked()
{
    ui->stackedWidget_main->setCurrentIndex(0);
    ui->stackedWidget_header->setCurrentIndex(0);
}


void window::on_pushButton_auto_clicked()
{
    ui->stackedWidget_main->setCurrentIndex(1);
    ui->stackedWidget_header->setCurrentIndex(1);
}


void window::on_spinBox_manual_frequency_valueChanged(int value)
{
    if(value >= 2600)
        ui->tabWidget->setCurrentIndex(1);
    else
        ui->tabWidget->setCurrentIndex(0);
}


void window::on_pushButton_manual_diode_1_A_clicked()
{
    ui->label_manual_diode_1_display->setText("A");
    ui->pushButton_manual_diode_1_B->setEnabled(true);
    ui->pushButton_manual_diode_1_A->setEnabled(false);
}


void window::on_pushButton_manual_diode_1_B_clicked()
{
    ui->label_manual_diode_1_display->setText("B");
    ui->pushButton_manual_diode_1_B->setEnabled(false);
    ui->pushButton_manual_diode_1_A->setEnabled(true);
}

void window::on_pushButton_manual_diode_2_A_clicked()
{
    ui->label_manual_diode_2_display->setText("A");
    ui->pushButton_manual_diode_2_B->setEnabled(true);
    ui->pushButton_manual_diode_2_A->setEnabled(false);
}

void window::on_pushButton_manual_diode_2_B_clicked()
{
    ui->label_manual_diode_2_display->setText("B");
    ui->pushButton_manual_diode_2_B->setEnabled(false);
    ui->pushButton_manual_diode_2_A->setEnabled(true);
}

void window::on_pushButton_manual_send_clicked()
{
    generatorFrequency = ui->spinBox_manual_frequency->value();

    generator->setGeneratorFrequency(generatorFrequency);
    generator->setGeneratorAmplitude(ui->spinBox_manual_amplitude->value());

    if(generatorFrequency >= 2600) {
        xport->setXPortOutput(1);
        amplifier5G->init();

        if(generatorFrequency >= 2600 && generatorFrequency < 3350)
            amplifier5G->setFrequencyBand(1);
        else if (generatorFrequency >= 3350 && generatorFrequency < 3650)
            amplifier5G->setFrequencyBand(2);
        else if (generatorFrequency >= 3650 && generatorFrequency <= 4000)
            amplifier5G->setFrequencyBand(3);

        amplifier5G->setChannelValue(1, ui->spinBox_manual_channel_1_5G->value());
        amplifier5G->setChannelValue(2, ui->spinBox_manual_channel_2_5G->value());
        amplifier5G->setChannelValue(3, ui->spinBox_manual_channel_3_5G->value());
        amplifier5G->setChannelValue(4, ui->spinBox_manual_channel_4_5G->value());

        config->setValue("channel_1_5G", ui->spinBox_manual_channel_1_5G->value());
        config->setValue("channel_2_5G", ui->spinBox_manual_channel_2_5G->value());
        config->setValue("channel_3_5G", ui->spinBox_manual_channel_3_5G->value());
        config->setValue("channel_4_5G", ui->spinBox_manual_channel_4_5G->value());
    } else {
        xport->setXPortOutput(0);
        amplifier4G->init();

        amplifier4G->setDiode1(ui->label_manual_diode_1_display->text());
        amplifier4G->setDiode2(ui->label_manual_diode_2_display->text());

        if(generatorFrequency <= 800)
            amplifier4G->setFrequencyBand(1);
        else if (generatorFrequency <= 1350)
            amplifier4G->setFrequencyBand(2);
        else if (generatorFrequency <= 1985)
            amplifier4G->setFrequencyBand(3);
        else if (generatorFrequency <= 2310)
            amplifier4G->setFrequencyBand(4);
        else if (generatorFrequency <= 2525)
            amplifier4G->setFrequencyBand(5);
        else if (generatorFrequency < 2600)
            amplifier4G->setFrequencyBand(6);

        amplifier4G->setChannelValue(1, ui->spinBox_manual_channel_1_4G->value());
        amplifier4G->setChannelValue(2, ui->spinBox_manual_channel_2_4G->value());
        amplifier4G->setChannelValue(3, ui->spinBox_manual_channel_3_4G->value());
        amplifier4G->setChannelValue(4, ui->spinBox_manual_channel_4_4G->value());
        amplifier4G->setChannelValue(5, ui->spinBox_manual_channel_5_4G->value());
        amplifier4G->setChannelValue(6, ui->spinBox_manual_channel_6_4G->value());

        config->setValue("channel_1_4G", ui->spinBox_manual_channel_1_4G->value());
        config->setValue("channel_2_4G", ui->spinBox_manual_channel_2_4G->value());
        config->setValue("channel_3_4G", ui->spinBox_manual_channel_3_4G->value());
        config->setValue("channel_4_4G", ui->spinBox_manual_channel_4_4G->value());
        config->setValue("channel_5_4G", ui->spinBox_manual_channel_5_4G->value());
        config->setValue("channel_6_4G", ui->spinBox_manual_channel_6_4G->value());
        config->setValue("diode_1", ui->label_manual_diode_1_display->text());
        config->setValue("diode_2", ui->label_manual_diode_2_display->text());
    }
}


void window::on_pushButton_manual_output_toggled(bool checked)
{
    if(checked) {
        ui->pushButton_manual_output->setText("Éteindre");
        generator->setGeneratorOutput(1);
        isGeneratorLaunched = true;
    } else {
        ui->pushButton_manual_output->setText("Allumer");
        generator->setGeneratorOutput(0);
        isGeneratorLaunched = false;
    }
}

void window::on_actionSe_reconnecter_triggered()
{
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    exit(0);
}

void window::on_actionChanger_la_p_riode_de_sauvegarde_triggered()
{
    newSavingTime = QInputDialog::getInt(this, tr("Changer la période de sauvegarde"), tr("Entrez la nouvelle période en secondes :"), savingTime, 1, 3600, 1, &ok);
    if (ok) {
        savingTime = newSavingTime;
        currentSavingTime = savingTime;
    }
}

void window::on_pushButton_auto_play_stop_toggled(bool checked)
{
    if(checked) {
        ui->pushButton_auto_play_stop->setIcon(QIcon(":/img/stop.png"));
        ui->pushButton_auto_play_stop->setIconSize(QSize(50,50));
        ui->label_auto_play_stop->setText("Arrêter");
        ui->pushButton_manual->setEnabled(false);
        generator->setGeneratorOutput(0);
        isAutoLaunched = true;
    } else {
        ui->pushButton_auto_play_stop->setIcon(QIcon(":/img/start.png"));
        ui->pushButton_auto_play_stop->setIconSize(QSize(50,50));
        ui->label_auto_play_stop->setText("Lancer");
        ui->pushButton_manual->setEnabled(true);
        generator->setGeneratorOutput(0);
        indexCycleAvailable = -1;
        cycleAlreadyRunning = false;
        isAutoLaunched = false;
    }
}

void window::on_pushButton_auto_paste_clicked()
{
    if(ui->spinBox_auto_frequency->value() >= 2600) {
        ui->spinBox_auto_channel_1_5G->setValue(ui->spinBox_manual_channel_1_5G->value());
        ui->spinBox_auto_channel_2_5G->setValue(ui->spinBox_manual_channel_2_5G->value());
        ui->spinBox_auto_channel_3_5G->setValue(ui->spinBox_manual_channel_3_5G->value());
        ui->spinBox_auto_channel_4_5G->setValue(ui->spinBox_manual_channel_4_5G->value());
    } else {
        ui->spinBox_auto_channel_1_4G->setValue(ui->spinBox_manual_channel_1_4G->value());
        ui->spinBox_auto_channel_2_4G->setValue(ui->spinBox_manual_channel_2_4G->value());
        ui->spinBox_auto_channel_3_4G->setValue(ui->spinBox_manual_channel_3_4G->value());
        ui->spinBox_auto_channel_4_4G->setValue(ui->spinBox_manual_channel_4_4G->value());
        ui->spinBox_auto_channel_5_4G->setValue(ui->spinBox_manual_channel_5_4G->value());
        ui->spinBox_auto_channel_6_4G->setValue(ui->spinBox_manual_channel_6_4G->value());
    }
}

void window::on_actionChanger_le_rep_rtoire_de_sauvegarde_triggered()
{
    directoryPath = saveURL;
    directory = new QDir(directoryPath);

    if (!directory->exists()) {
        if (!directory->mkpath(".")) {
            qDebug() << "Impossible de créer le répertoire :" << directoryPath;
            return;
        }
    }

    selectedDirectory = QFileDialog::getExistingDirectory(this, tr("Sélectionner un répertoire"), saveURL);

    if (!selectedDirectory.isEmpty()) {
        saveURL = selectedDirectory;
        config->setValue("saves_url", saveURL);
    }
}

void window::on_actionOuvrir_le_rep_rtoire_de_sauvegarde_triggered()
{
    directoryPath = saveURL;
    directory = new QDir(directoryPath);

    if (!directory->exists()) {
        if (!directory->mkpath(".")) {
            qDebug() << "Impossible de créer le répertoire :" << directoryPath;
                return;
        }
    }

    directoryUrl = QUrl::fromLocalFile(directoryPath);
    QDesktopServices::openUrl(directoryUrl);
}


void window::on_actionChanger_le_rep_rtoire_des_presets_triggered()
{
    directoryPath = presetsURL;
    directory = new QDir(directoryPath);

    if (!directory->exists()) {
        if (!directory->mkpath(".")) {
            qDebug() << "Impossible de créer le répertoire :" << directoryPath;
            return;
        }
    }

    selectedDirectory = QFileDialog::getExistingDirectory(this, tr("Sélectionner un répertoire"), presetsURL);

    if (!selectedDirectory.isEmpty()) {
        presetsURL = selectedDirectory;
        config->setValue("presets_url", presetsURL);
    }
}
