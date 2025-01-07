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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , defaultAction(Heal) // Сначала инициализируем defaultAction
    , fileMonitor(nullptr) // Затем fileMonitor
    , fileWatcher(new QFileSystemWatcher(this)) // И затем fileWatcher
{
    ui->setupUi(this);
    this->setWindowTitle("Антивирусный монитор");

    // Добавляем файл в QFileSystemWatcher
    fileWatcher->addPath("log.txt");
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::updateLogFile);

    // Загружаем записи из файла
    updateLogFile("log.txt");

    // Создание меню
    QMenuBar *menuBar = this->menuBar();
    QMenu *fileMenu = menuBar->addMenu("Файл");

    // Добавление действий
    QAction *settingsAction = fileMenu->addAction("Настройки");
    QAction *deleteLog = fileMenu->addAction("Очистить окно");
    QAction *exitAction = fileMenu->addAction("Выход");

    // Подключение сигналов
    connect(settingsAction, &QAction::triggered, this, &MainWindow::Settings);
    connect(deleteLog, &QAction::triggered, this, &MainWindow::DeleteLog); // Исправлено здесь
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
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

    QMap<QString, QBrush> typeBrushes;
    typeBrushes["Error"] = QBrush(Qt::red);
    typeBrushes["Warning"] = QBrush(Qt::yellow);
    typeBrushes["Info"] = QBrush(Qt::blue);
    QRegExp typeExp("^[а-яА-Я]{2,3} \\w+ \\d{1,2} \\d{2}:\\d{2}:\\d{2} \\d{4}: (\\w+):"); // Регулярное выражение для поиска типа
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.isEmpty()) {
            // Применяем регулярное выражение к строке
            if (typeExp.indexIn(line) != -1) {
                QString type = typeExp.cap(1); // Получаем тип
                QBrush brush = typeBrushes.value(type, QBrush(Qt::black)); // Получение соответствующего цвета

                // Отладочный вывод
                qDebug() << "Found type:" << type << "Color:" << brush.color();

                // Создание и настройка элемента списка
                QListWidgetItem* item = new QListWidgetItem(line);
                item->setForeground(brush);
                ui->listWidget->addItem(item);
            } else {
                // Если тип не найден, можно добавить элемент с черным текстом
                QListWidgetItem* item = new QListWidgetItem(line);
                item->setForeground(QBrush(Qt::black));
                ui->listWidget->addItem(item);
            }
        }
    }
    ui->listWidget->scrollToBottom();
    file.close(); // Закрыть файл
}

void MainWindow::DeleteLog() {
    ui->listWidget->clear(); // Очистить элементы в QListWidget
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
    Q_UNUSED(filePath);
    // Здесь можно обновить интерфейс или отобразить список зараженных файлов
    // Удалено: showActionDialog();
}

void MainWindow::on_stopButton_clicked() {
    if (fileMonitor) {
        fileMonitor->stopMonitoring(); // Предполагается, что у вас есть метод stopMonitoring в классе FileMonitor
        delete fileMonitor; // Удаляем объект, если он больше не нужен
        fileMonitor = nullptr; // Устанавливаем указатель в nullptr
        QMessageBox::information(this, "Мониторинг остановлен", "Мониторинг файловой системы был успешно остановлен.");
    } else {
        QMessageBox::warning(this, "Ошибка", "Мониторинг не запущен.");
    }
}

void MainWindow::on_pushButton_clicked() {
    QString quarantineFolderPath = "/home/vladimir/Загрузки/Anne/Карантин"; // Укажите путь к папке карантин
    QDesktopServices::openUrl(QUrl::fromLocalFile(quarantineFolderPath));
}

void MainWindow::Settings() {
    ActionDialog dialog(this); // Создаем экземпляр ActionDialog
    if (dialog.exec() == QDialog::Accepted) {
        QString selectedAction = dialog.selectedAction();
        QMessageBox::information(this, "Выбор действия", "Вы выбрали: " + selectedAction);

        // Обновите defaultAction в зависимости от выбора пользователя
        if (selectedAction == "Удалить файл") {
            defaultAction = Delete;
        } else if (selectedAction == "Карантин") {
            defaultAction = Quarantine;
        } else if (selectedAction == "Игнорировать") {
            defaultAction = Ignore;
        } else if (selectedAction == "Вылечить") {
            defaultAction = Heal;
        }

        // Устанавливаем действие в FileMonitor
        if (fileMonitor) {
            fileMonitor->setDefaultAction(defaultAction);
        }
    }
}
