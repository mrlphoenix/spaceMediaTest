#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QListWidgetItem>
namespace Ui {
class MainWindow;
}


struct VersionInfo
{
    static VersionInfo parse(QString s);

    int version_major;
    int version_minor;
    int version_release;
    int version_build;

    bool isValid();
    QString toString();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadPreferences();
    void setupUiFromJson(QJsonObject root);
    void saveConfigToJson();
    void processVersionHeader(QString filename);

private slots:
    void onReplyFinished(QNetworkReply * reply);
    void on_updater_path_editingFinished();

    void on_player_path_editingFinished();

    void on_version_path_editingFinished();

    void on_platformCombobox_currentTextChanged(const QString &);

    void on_deploy_player_toggled(bool);

    void on_deploy_updater_toggled(bool);

    void on_project_location_editingFinished();

    void on_login_editingFinished();

    void on_password_editingFinished();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_addFileButton_clicked();

    void on_removeFileButton_clicked();

    void on_tabWidget_tabBarClicked(int index);

    void on_deploy_clicked();

    void on_pushButton_2_clicked();

    void on_file_list_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
};

#endif // MAINWINDOW_H
