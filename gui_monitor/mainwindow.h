#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "filemonitor.h"
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
    void on_stopButton_clicked();
    void on_pushButton_clicked();
    void Settings();
    void DeleteLog();

private:
    Ui::MainWindow *ui;
    FileMonitor::Action defaultAction;  // Указываем пространство имен FileMonitor::
    FileMonitor *fileMonitor;
    QFileSystemWatcher *fileWatcher;
    QString selectedFolder;
    void updateLogFile(const QString& filePath);
};

#endif // MAINWINDOW_H
