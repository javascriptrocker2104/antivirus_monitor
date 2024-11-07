#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QString>
#include "filemonitor.h"
#include "quarantine.h"

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
    void updateLogFile(const QString& filePath);
    void on_stopButton_clicked();
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QFileSystemWatcher *fileWatcher;
    qint64 lastReadPosition; // Объявление переменной для хранения позиции
    QString selectedFolder;
    FileMonitor *fileMonitor;
    Quarantine *quarantine;
};

#endif // MAINWINDOW_H
