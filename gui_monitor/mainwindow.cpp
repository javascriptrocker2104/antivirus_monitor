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
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "actiondialog.h"
#include "filemonitor.h"
#include <QScrollBar>

// Конструктор класса MainWindow
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , defaultAction(FileMonitor::Heal) // Установка действия по умолчанию
    , fileMonitor(nullptr) // Инициализация указателя на FileMonitor
    , fileWatcher(new QFileSystemWatcher(this)) // Создание QFileSystemWatcher для отслеживания изменений в файле
{
    ui->setupUi(this); // Настройка пользовательского интерфейса
    this->setWindowTitle("Антивирусный монитор"); // Установка заголовка окна

    // Очищаем все стили для listWidget
    ui->listWidget->setStyleSheet("");

    // Добавляем файл в QFileSystemWatcher для отслеживания изменений
    fileWatcher->addPath("log.txt");
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::updateLogFile);

    // Загружаем записи из файла логов
    updateLogFile("log.txt");

    // Создание меню в строке меню
    QMenuBar *menuBar = this->menuBar();
    QMenu *fileMenu = menuBar->addMenu("Файл");

    // Добавление действий в меню
    QAction *settingsAction = fileMenu->addAction("Настройки");
    QAction *deleteLog = fileMenu->addAction("Очистить окно");
    QAction *exitAction = fileMenu->addAction("Выход");


    // Подключение сигналов к слотам
    connect(settingsAction, &QAction::triggered, this, &MainWindow::Settings);
    connect(deleteLog, &QAction::triggered, this, &MainWindow::DeleteLog);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
}

// Деструктор класса MainWindow
MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::updateLogFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл логов.";
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл логов.");
        return;
    }

    ui->listWidget->clear();
    qDebug() << "Очистка списка завершена.";

    // Яркие цвета для разных типов сообщений
    QMap<QString, QPair<QColor, QColor>> typeColors;
    typeColors["Ошибка"] = {QColor(255, 255, 255), QColor(255, 0, 0)};
    typeColors["Внимание"] = {QColor(255, 255, 255), QColor(255, 0, 0)};
    typeColors["Информация"] = {QColor(255, 255, 255), QColor(0, 150, 0)};

    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        QListWidgetItem* item = new QListWidgetItem(line);

        // Устанавливаем стандартный цвет
        item->setBackground(Qt::white);
        item->setForeground(Qt::black);

        // Проверяем на наличие ключевых слов
        for (const QString& type : typeColors.keys()) {
            if (line.contains(type, Qt::CaseInsensitive)) {
                item->setBackground(typeColors[type].first);
                item->setForeground(typeColors[type].second);
                break;
            }
        }

        ui->listWidget->addItem(item);
    }

    ui->listWidget->scrollToBottom();
    file.close();
}


// Метод для очистки логов
void MainWindow::DeleteLog() {
    ui->listWidget->clear(); // Очистка элементов в QListWidget
    QString logFilePath = "log.txt"; // Путь к файлу логов

    // Открываем файл для записи и очистки
    QFile logFile(logFilePath);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        logFile.close(); // Закрываем файл после очистки
        qDebug() << "Файл логов очищен:" << logFilePath;
    } else {
        qDebug() << "Не удалось открыть файл логов для очистки:" << logFilePath;
    }
}

// Метод для выбора папки
void MainWindow::on_selectFolderButton_clicked() {
    selectedFolder = QFileDialog::getExistingDirectory(this, "Select Folder", QString());

    if (!selectedFolder.isEmpty()) {
        if (fileMonitor) {
            delete fileMonitor; // Удаляем предыдущий монитор, если он существует
        }
        fileMonitor = new FileMonitor(selectedFolder, this); // Создание нового экземпляра FileMonitor
        fileMonitor->startMonitoring(); // Запуск мониторинга
        QMessageBox::information(this, "Начался мониторинг", "Мониторинг начался для папки: " + selectedFolder);
    }
}

// Метод для остановки мониторинга
void MainWindow::on_stopButton_clicked() {
    if (fileMonitor) {
        fileMonitor->stopMonitoring(); // Остановка мониторинга
        delete fileMonitor; // Удаляем объект
        fileMonitor = nullptr; // Устанавливаем указатель в nullptr
        QMessageBox::information(this, "Мониторинг остановлен", "Мониторинг файловой системы был успешно остановлен.");
    } else {
        QMessageBox::warning(this, "Ошибка", "Мониторинг не запущен.");
    }
}

// Метод для открытия папки карантина
void MainWindow::on_pushButton_clicked() {
    QString quarantineFolderPath = "/home/vladimir/Загрузки/Anne/Карантин"; // Путь к папке карантин
    QDesktopServices::openUrl(QUrl::fromLocalFile(quarantineFolderPath)); // Открытие папки в файловом менеджере
}

// Метод для открытия диалога настроек
void MainWindow::Settings() {
    ActionDialog dialog(this); // Создание экземпляра ActionDialog
    if (dialog.exec() == QDialog::Accepted) { // Проверка, было ли подтверждено действие
        QString selectedAction = dialog.selectedAction(); // Получение выбранного действия
        QMessageBox::information(this, "Выбор действия", "Вы выбрали: " + selectedAction);

        // Обновление defaultAction в зависимости от выбора пользователя
        if (selectedAction == "Удалить") {
            defaultAction = FileMonitor::Delete;
        } else if (selectedAction == "Переместить в карантин") {
            defaultAction = FileMonitor::Quarantine;
        } else if (selectedAction == "Игнорировать") {
            defaultAction = FileMonitor::Ignore;
        } else if (selectedAction == "Вылечить") {
            defaultAction = FileMonitor::Heal;
        }

        // Установка действия в FileMonitor
        if (fileMonitor) {
            fileMonitor->setDefaultAction(defaultAction);
        }
    }
}
