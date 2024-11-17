#include <QDebug>
#include <QListWidget>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "quarantine.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileSystemWatcher>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , fileWatcher(new QFileSystemWatcher(this)) // Инициализация fileWatcher
    , fileMonitor(nullptr) // Инициализация fileMonitor
    , quarantine(nullptr)
{
    ui->setupUi(this);
    this->setWindowTitle("Антивирусный монитор");

    // Добавляем файл в QFileSystemWatcher
    fileWatcher->addPath("log.txt");
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::updateLogFile);

    // Загружаем записи из файла
    updateLogFile("log.txt");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::updateLogFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл логов.");
        return;
    }

    ui->listWidget->clear(); // Очистить предыдущие элементы

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.isEmpty()) { // Проверяем, что строка не пустая
            QListWidgetItem* item = new QListWidgetItem(line);
            ui->listWidget->addItem(item); // Добавить элемент в QListWidget
        }
    }
    ui->listWidget->scrollToBottom();

    file.close(); // Закрыть файл
}


void MainWindow::on_selectFolderButton_clicked() {
    selectedFolder = QFileDialog::getExistingDirectory(this, "Select Folder", QString());

    if (!selectedFolder.isEmpty()) {
        if (fileMonitor) {
            delete fileMonitor; // Удаляем предыдущий монитор, если он существует
        }
        fileMonitor = new FileMonitor(selectedFolder, this);
        connect(fileMonitor, &FileMonitor::fileChanged, this, &MainWindow::on_fileChanged);
        fileMonitor->startMonitoring();
        QMessageBox::information(this, "Начался мониторинг", "Мониторинг начался для папки: " + selectedFolder);
    }
}

void MainWindow::on_fileChanged(const QString& filePath) {
    //QMessageBox::warning(this, "Warning", "File was changed: " + filePath);
    // Здесь можно обновить интерфейс или отобразить список зараженных файлов
}

void MainWindow::on_stopButton_clicked()
{
    if (fileMonitor) {
        fileMonitor->stopMonitoring(); // Предполагается, что у вас есть метод stopMonitoring в классе FileMonitor
        delete fileMonitor; // Удаляем объект, если он больше не нужен
        fileMonitor = nullptr; // Устанавливаем указатель в nullptr
        QMessageBox::information(this, "Мониторинг остановлен", "Мониторинг файловой системы был успешно остановлен.");
    } else {
        QMessageBox::warning(this, "Ошибка", "Мониторинг не запущен.");
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString quarantineFolderPath = "/home/vladimir/Загрузки/Anne/Карантин"; // Укажите путь к папке карантин
        QDesktopServices::openUrl(QUrl::fromLocalFile(quarantineFolderPath));
    /* //Создаем новый экземпляр диалога каждый раз при нажатии на кнопку
    Quarantine *quarantine = new Quarantine(this);
    quarantine->setAttribute(Qt::WA_DeleteOnClose); // Автоматическое удаление при закрытии
    quarantine->move(100, 100); // Устанавливаем позицию окна
    quarantine->show(); // Показываем окно
    quarantine->raise(); // Поднимаем окно на передний план
    quarantine->activateWindow(); // Активируем окно*/
}
