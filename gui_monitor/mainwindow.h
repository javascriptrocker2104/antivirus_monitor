#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "filemonitor.h" // Убедитесь, что filemonitor.h объявляет Action
#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QListWidgetItem>
#include <QMap>
#include <QBrush>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_selectFolderButton_clicked();
    void on_fileChanged(const QString& filePath);
    void on_stopButton_clicked();
    void on_pushButton_clicked();
    void Settings();
    void DeleteLog();

private:
    Ui::MainWindow *ui;
    QFileSystemWatcher *fileWatcher;
    Action defaultAction; // Объявление defaultAction как члена класса
    FileMonitor *fileMonitor; // Объявление fileMonitor
    QString selectedFolder; // Объявление selectedFolder
    void updateLogFile(const QString& filePath); // Объявление updateLogFile
};

#endif // MAINWINDOW_H
