#include "mainwindow.h"
#include "ui_mainwindow.h"

#define DT_PASS QString("randomPass123123")

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QByteArray>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QStringList>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QInputDialog>
#include <QDir>
#include <QDirIterator>

#include "lfsrencoder.h"
#include "notherfilesystem.h"
#include "teledsencoder.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(onReplyFinished(QNetworkReply*)));
    loadPreferences();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadPreferences()
{
    QFile f("config.dat");
    QFileInfo fInfo("config.dat");
    if (fInfo.exists())
    {
        if (f.open(QFile::ReadOnly))
        {
            QByteArray data = f.readAll();
            LFSR::Encoder::encode(data, DT_PASS);
            QJsonDocument doc = QJsonDocument::fromJson(data);
            setupUiFromJson(doc.object());
        }
    }
    f.close();
}

QStringList scanDir(QDir dir)
{
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    qDebug() << "Scanning: " << dir.path();

    QStringList fileList = dir.entryList();
    for (int i = 0; i < fileList.count(); i++)
    {
        fileList[i] = dir.path() + "/" + fileList[i];
    }

    dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList dirList = dir.entryList();
    for (int i=0; i<dirList.size(); ++i)
    {
        QString newPath = QString("%1/%2").arg(dir.absolutePath()).arg(dirList.at(i));
        fileList.append(scanDir(QDir(newPath)));
    }
    return fileList;
}

void MainWindow::setupUiFromJson(QJsonObject root)
{
    ui->updater_path->setText(root["updater_path"].toString());
    ui->player_path->setText(root["player_path"].toString());
    ui->version_path->setText(root["version_path"].toString());
    if (!ui->version_path->text().isEmpty())
        processVersionHeader(ui->version_path->text());
    ui->project_location->setText(root["project_location"].toString());
    ui->login->setText(root["login"].toString());
    ui->password->setText(root["password"].toString());

    ui->platformCombobox->setCurrentIndex(ui->platformCombobox->findText(root["platform"].toString()));
    ui->deploy_player->setChecked(root["deploy_player"].toBool());
    ui->deploy_updater->setChecked(root["deploy_updater"].toBool());
    ui->prev_version_folder->setText(root["deploy_prev_path"].toString());
    ui->next_version_folder->setText(root["deploy_current_path"].toString());
}

void MainWindow::saveConfigToJson()
{
    QJsonObject json;
    json["updater_path"] = ui->updater_path->text();
    json["player_path"] = ui->player_path->text();
    json["version_path"] = ui->version_path->text();
    json["project_location"] = ui->project_location->text();
    json["login"] = ui->login->text();
    json["password"] = ui->password->text();

    json["platform"] = ui->platformCombobox->currentText();
    json["deploy_player"] = ui->deploy_player->isChecked();
    json["deploy_updater"] = ui->deploy_updater->isChecked();

    json["deploy_prev_path"] = ui->prev_version_folder->text();
    json["deploy_current_path"] = ui->next_version_folder->text();

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    LFSR::Encoder::encode(data, DT_PASS);

    QFile f("config.dat");
    if (f.open(QFile::WriteOnly))
    {
        f.write(data);
        f.flush();
        f.close();
    }
    else
    {
        QMessageBox::critical(this,"ERROR","Cant save to config!");
    }
}

void MainWindow::processVersionHeader(QString filename)
{
    QFile f(filename);
    if (f.open(QFile::ReadOnly))
    {
        QString data = f.readAll();
        QStringList strings = data.split("\n");
        VersionInfo versionInfo;
        for (int i = 0; i < strings.count(); ++i)
        {
            QString currentString = strings[i];
            if (currentString.indexOf("static const int BUILD") != -1)
            {
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    versionInfo.version_build = tokens[5].toInt();
                }
            }
            if (currentString.indexOf("static const int RELEASE") != -1)
            {
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    versionInfo.version_release = tokens[5].toInt();
                }
            }
            if (currentString.indexOf("static const int MINOR") != -1)
            {
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    versionInfo.version_minor = tokens[5].toInt();
                }
            }
            if (currentString.indexOf("static const int MAJOR") != -1)
            {
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    versionInfo.version_major = tokens[5].toInt();
                }
            }
        }
        if (versionInfo.isValid())
            ui->versionString->setText(versionInfo.toString());
        else
            QMessageBox::warning(this, "ERROR", "Not valid version Header");
        f.close();
    }
    else QMessageBox::warning(this, "ERROR", "Cant open File");
}

void MainWindow::onReplyFinished(QNetworkReply *reply)
{
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    if ( statusCode.isValid() )
    {
        if (statusCode.toInt() == 200)
        {
            ui->deploy_log->appendPlainText("File uploaded successfully!");
        }
        else
            ui->deploy_log->appendPlainText("File uploading FAILED: " + reply->readAll());
    }
    reply->deleteLater();
    ui->deploy->setEnabled(true);
}

void MainWindow::on_updater_path_editingFinished()
{
    saveConfigToJson();
}

void MainWindow::on_player_path_editingFinished()
{
    saveConfigToJson();
}

void MainWindow::on_version_path_editingFinished()
{
    saveConfigToJson();
}

void MainWindow::on_platformCombobox_currentTextChanged(const QString &)
{
    saveConfigToJson();
}

void MainWindow::on_deploy_player_toggled(bool)
{
    saveConfigToJson();
}

void MainWindow::on_deploy_updater_toggled(bool)
{
    saveConfigToJson();
}

void MainWindow::on_project_location_editingFinished()
{
    saveConfigToJson();
}

void MainWindow::on_login_editingFinished()
{
    saveConfigToJson();
}

void MainWindow::on_password_editingFinished()
{
    saveConfigToJson();
}

VersionInfo VersionInfo::parse(QString s)
{
    VersionInfo result;
    result.version_build = result.version_major = result.version_minor = result.version_release = -1;

    QStringList tokens = s.split(".");
    if (tokens.count() != 4)
        return result;
    result.version_major = tokens.at(0).toInt();
    result.version_minor = tokens.at(1).toInt();
    result.version_release = tokens.at(2).toInt();
    result.version_build = tokens.at(3).toInt();
    return result;
}

bool VersionInfo::isValid()
{
    if (version_major < 0)
        return false;
    if (version_minor < 0)
        return false;
    if (version_release < 0)
        return false;
    if (version_build < 0)
        return false;
    if ((version_build | version_release | version_minor | version_major) == 0)
        return false;
    return true;
}

QString VersionInfo::toString()
{
    return QString::number(version_major) + "." +
           QString::number(version_minor) + "." +
           QString::number(version_release) + "." +
           QString::number(version_build);
}

void MainWindow::on_pushButton_3_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Updater Executable"), qApp->applicationDirPath());
    if (!fileName.isEmpty())
    {
        ui->updater_path->setText(fileName);
        saveConfigToJson();
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Player Executable"), qApp->applicationDirPath());
    if (!fileName.isEmpty())
    {
        ui->player_path->setText(fileName);
        saveConfigToJson();
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Player Project Version Header"), qApp->applicationDirPath());
    if (!fileName.isEmpty())
    {
        ui->version_path->setText(fileName);
        saveConfigToJson();
        processVersionHeader(fileName);
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Root Directory"),
                                                 qApp->applicationDirPath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
        ui->project_location->setText(dir);
}

void MainWindow::on_addFileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Player Project Version Header"), ui->project_location->text(),"*.*");
    if (!fileName.isEmpty())
    {
        QFileInfo fInfo(fileName);
        fileName = fInfo.absoluteFilePath();
        QString relativePath = fileName;
        if (ui->project_location->text() != "")
            relativePath.replace(ui->project_location->text()+"/","");
        if (!fileName.isEmpty())
        {
            ui->file_list->addItem(fileName + "||" + relativePath);
        }
    }
}

void MainWindow::on_removeFileButton_clicked()
{
    qDeleteAll(ui->file_list->selectedItems());
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if (index == 2)
    {
        bool deployShouldBeEnabled = true;
        if (ui->deploy_player->isChecked() && ui->player_path->text().isEmpty())
        {
            ui->deploy->setEnabled(false);
            deployShouldBeEnabled = false;
        }
        if (ui->deploy_updater->isChecked() && ui->updater_path->text().isEmpty())
        {
            ui->deploy->setEnabled(false);
            deployShouldBeEnabled = false;
        }
        VersionInfo info = VersionInfo::parse(ui->versionString->text());
        if (!info.isValid())
        {
            ui->deploy->setEnabled(false);
            deployShouldBeEnabled = false;
        }
        if (ui->platformCombobox->currentIndex() < 0)
        {
            ui->deploy->setEnabled(false);
            deployShouldBeEnabled = false;
        }
        ui->deploy->setEnabled(deployShouldBeEnabled);
    }
}

void MainWindow::on_deploy_clicked()
{
    ui->deploy->setEnabled(false);
    ui->deploy_log->appendPlainText("---------------");
    ui->deploy_log->appendPlainText("Deploy started " + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    ui->deploy_log->appendPlainText("Building TDSU file...");
    qApp->processEvents();
    NotherFileSystem nfs;
    if (ui->deploy_player->isChecked())
        nfs.addFile(ui->player_path->text(), "system::player");
    if (ui->deploy_updater->isChecked())
        nfs.addFile(ui->updater_path->text(), "system::updater");
    for (int i = 0; i < ui->file_list->count(); i++)
    {
        QString item = ui->file_list->item(i)->text();
        QStringList tokens = item.split("||");

        nfs.addFile(tokens.first(), tokens.last());
    }
    QByteArray data = nfs.build();

    //sending to server
    QString authString = ui->login->text() + ":" + ui->password->text();
    QByteArray authBase64 = authString.toLocal8Bit().toBase64();
    QString authHeader = "Basic " + authBase64;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    VersionInfo versionInfo = VersionInfo::parse(ui->versionString->text());

    ui->deploy_log->appendPlainText("TDSU file ["+versionInfo.toString()+"] is ready ( "+ QString::number(data.size()) + " bytes) Uploading...");

    QHttpPart majorVersionPart, minorVersionPart, releaseVersionPart, buildVersionPart,
              sourcePart, platformPart, filePart;
    majorVersionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"version_major\""));
    majorVersionPart.setBody(QString::number(versionInfo.version_major).toLocal8Bit());

    minorVersionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"version_minor\""));
    minorVersionPart.setBody(QString::number(versionInfo.version_minor).toLocal8Bit());

    releaseVersionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"version_release\""));
    releaseVersionPart.setBody(QString::number(versionInfo.version_release).toLocal8Bit());

    buildVersionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"version_build\""));
    buildVersionPart.setBody(QString::number(versionInfo.version_build).toLocal8Bit());

    sourcePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"source\""));
    sourcePart.setBody(QString("TeleDS Deploy Tool/Qt").toLocal8Bit());

    platformPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"platform\""));
    platformPart.setBody(ui->platformCombobox->currentText().toLocal8Bit());

    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"TDSU\""));
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    filePart.setBody(data);

    multiPart->append(majorVersionPart);
    multiPart->append(minorVersionPart);
    multiPart->append(releaseVersionPart);
    multiPart->append(buildVersionPart);
    multiPart->append(sourcePart);
    multiPart->append(platformPart);
    multiPart->append(filePart);

    QUrl url("http://api.teleds.com/app/deploy");
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", authHeader.toLocal8Bit());

    QNetworkReply *reply = manager->post(request, multiPart);
    multiPart->setParent(reply); // delete the multiPart with the reply
}

void MainWindow::on_pushButton_2_clicked()
{
    if (!ui->version_path->text().isEmpty())
        processVersionHeader(ui->version_path->text());
}

void MainWindow::on_file_list_itemDoubleClicked(QListWidgetItem *item)
{
    QStringList tokens = item->text().split("||");
    bool ok;
    QString text = QInputDialog::getText(this, tr("Set custom path"),
                                             tr("custom path: "), QLineEdit::Normal,tokens[1], &ok);
    if (ok)
        item->setText(tokens[0] + "||" + text);
}

void MainWindow::on_pushButton_6_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Root Directory"),
                                                 qApp->applicationDirPath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
        ui->prev_version_folder->setText(dir);
    saveConfigToJson();
}

void MainWindow::on_pushButton_7_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Root Directory"),
                                                 qApp->applicationDirPath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
        ui->next_version_folder->setText(dir);
    saveConfigToJson();
}

void MainWindow::on_pushButton_8_clicked()
{
    //test function
    QStringList prevDirFiles = scanDir(QDir(ui->prev_version_folder->text()));
    QStringList nextDirFiles = scanDir(QDir(ui->next_version_folder->text()));
    TeleDSPatch patch = TeleDSPatcher::createPatch(prevDirFiles,nextDirFiles, prevDirFiles,
                                                   "/home/nother/TeleDS/deploy/old", "/home/nother/TeleDS/deploy/current", ui->player_version->value(), ui->patch_version->value());

    qDebug() << patch.magic;

    QByteArray out;
    QDataStream ds(&out, QIODevice::WriteOnly);

    qDebug() << "Serialization Start" << QDateTime::currentDateTime();
    ds << patch.magic << patch.patchVersion << patch.playerVersion;
    ds << patch.totalFileCount << patch.updateFileCount;
    ds << patch.fileNames << patch.fileHashes;
    ds << patch.updateFiles.count();
    for (int i = 0; i < patch.updateFiles.count(); i++)
    {
        FilePatchInfo info = patch.updateFiles[i];
        ds << info.magic << info.patchHash << info.originalFileHash << info.filePath;
        ds << TeleDSPatcher::serializePatch(info.patch);
    }

    QFile f("/home/nother/1945patch.patch");
    if (f.open(QFile::WriteOnly))
    {
        f.write(out);
        f.flush();
        f.close();
    }
    qDebug() << "Serialization end " << QDateTime::currentDateTime();

    qDebug() << "Creating image ";
    TeleDSImage image = TeleDSImage::createImage(ui->player_version->value(), ui->patch_version->value(), nextDirFiles);
    qDebug() << "Serializing Image " << QDateTime::currentDateTime();
    QByteArray data = image.serialize();
    QFile imagef("/home/nother/1945patch.image");
    if (imagef.open(QFile::WriteOnly))
    {
        imagef.write(data);
        imagef.flush();
        imagef.close();
    }
    qDebug() << "Ready" << QDateTime::currentDateTime();
}

void MainWindow::on_prev_version_folder_editingFinished()
{
    saveConfigToJson();
}

void MainWindow::on_next_version_folder_editingFinished()
{
    saveConfigToJson();
}
