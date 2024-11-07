#include "quarantine.h"
#include "ui_quarantine.h"
#include <QFileSystemWatcher>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QDebug>

Quarantine::Quarantine(QWidget *parent) :
    QDialog(parent), // Теперь это правильно
    ui(new Ui::Quarantine),
    fileWatcher(new QFileSystemWatcher(this)) // Инициализация fileWatcher
{
    ui->setupUi(this);
    this->setWindowTitle("Карантин");

    // Добавляем файл в QFileSystemWatcher
    fileWatcher->addPath("extracted_lines.txt");
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &Quarantine::removedLines);

    // Загружаем записи из файла
    removedLines("extracted_lines.txt");
}

Quarantine::~Quarantine()
{
    // Удаляем все пути из QFileSystemWatcher, если они есть
    if (!fileWatcher->files().isEmpty()) {
        fileWatcher->removePaths(fileWatcher->files());
    }

    // Отключаем сигнал перед удалением объекта
    disconnect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &Quarantine::removedLines);

    delete ui; // Освобождаем интерфейс
}

    void Quarantine::removedLines(const QString& filePath) {
        qDebug() << "Файл изменен:" << filePath; // Отладочное сообщение
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл с удаленными строчками.");
            return;
        }

        ui->listWidget->clear(); // Очистить предыдущие элементы

        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QListWidgetItem* item = new QListWidgetItem(line);
            ui->listWidget->addItem(item); // Добавить элемент в QListWidget
        }

        file.close(); // Закрыть файл

        // Проверяем текущие файлы в QFileSystemWatcher
        qDebug() << "Текущие файлы в QFileSystemWatcher:" << fileWatcher->files();
    }
