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

// Основной класс пользовательского интерфейса антивирусного приложения
class MainWindow : public QMainWindow {
    Q_OBJECT // Макрос для поддержки сигналов и слотов в Qt

public:
    // Конструктор класса
    explicit MainWindow(QWidget *parent = nullptr);
    // Деструктор класса
    ~MainWindow();

private slots:
    // Слот, вызываемый при нажатии на кнопку выбора папки
    void on_selectFolderButton_clicked();
    // Слот, вызываемый при нажатии на кнопку остановки мониторинга
    void on_stopButton_clicked();
    // Слот, вызываемый при нажатии на кнопку открытия папки карантина
    void on_pushButton_clicked();
    // Слот для открытия окна настроек
    void Settings();
    // Слот для очистки лог-файла
    void DeleteLog();

private:
    Ui::MainWindow *ui;
    Action defaultAction;
    FileMonitor *fileMonitor;
    QFileSystemWatcher *fileWatcher;
    QString selectedFolder;
    // Метод для обновления лог-файла
    void updateLogFile(const QString& filePath);
};

#endif // MAINWINDOW_H
