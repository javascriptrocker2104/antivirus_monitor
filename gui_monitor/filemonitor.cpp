#include "filemonitor.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QDateTime>
#include <QRegularExpression>

FileMonitor::FileMonitor(const QString& path, QObject *parent)
    : QObject(parent), directoryPath(path), watcher(new QFileSystemWatcher(this)) {
    loadSignatures("/home/vladimir/Загрузки/Anne/gui_monitor/signatures.cvs");
    previousFilesList.clear();
}

void FileMonitor::startMonitoring() {
    log("Начинаем мониторинг папки: " + directoryPath);
    qDebug() << "Начинаем мониторинг папки:" << directoryPath;
    scanDirectory(directoryPath);
    connect(watcher, &QFileSystemWatcher::fileChanged, this, &FileMonitor::onFileChanged);
}

void FileMonitor::scanDirectory(const QString& dirPath) {
    QDir dir(dirPath);
    QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

    if (files.isEmpty()) {
        log("Нет файлов для мониторинга в папке: " + dirPath);
        qDebug() << "Нет файлов для мониторинга в папке:" << dirPath;
    }

    // Добавляем новые файлы в watcher
    for (const QString &file : files) {
        QString fullPath = dir.filePath(file);
        if (!previousFilesList.contains(fullPath)) { // Проверяем, не добавлен ли файл ранее
            if (watcher->addPath(fullPath)) { // Используем оператор ->
                log("Добавлен для мониторинга: " + fullPath);
                qDebug() << "Добавлен для мониторинга" << fullPath;
            } else {
                log("Ошибка добавления файла " + fullPath + " для мониторинга.");
                qDebug() << "Ошибка добавления файла для мониторинга";
            }
        }
    }

    // Рекурсивный обход поддиректорий
    QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &subDir : subDirs) {
        scanDirectory(dir.filePath(subDir)); // Рекурсивный вызов для каждой поддиректории
    }

    // Обновляем список файлов
    previousFilesList = files;
}

void FileMonitor::onFileChanged(const QString& path) {
    // Проверяем, существует ли файл перед обработкой
    if (!QFile::exists(path)) {
        log("Файл был удален" + path);
        qDebug() << "Файл был удален" << path;
        return; // Прекращаем выполнение, если файл не существует
    }

    log("Изменение файла: " + path);
    qDebug() << "Изменение файла:" << path;

    if (isInfected(path)) {
        handleInfection(path);
        emit fileChanged(path);
    } else {
        qDebug() << "Файл не инфицирован:" << path;
    }
}

bool FileMonitor::isInfected(const QString& filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line;
        while (in.readLineInto(&line)) {
            for (const Signature& signature : signatures) {
                if (signature.isRegex) {
                    QRegularExpression regex(signature.pattern);
                    if (regex.match(line).hasMatch()) {
                        return true; // Файл инфицирован
                    }
                } else {
                    if (line.contains(signature.pattern, Qt::CaseInsensitive)) {
                        return true; // Файл инфицирован
                    }
                }
            }
        }
    }
    return false; // Файл не инфицирован
}

void FileMonitor::handleInfection(const QString& filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QStringList lines;
        QTextStream in(&file);
        QString line;
        while (in.readLineInto(&line)) {
            lines.append(line);
        }
        file.close();

        // Записываем две строки в отдельный файл
        QFile outFile("extracted_lines.txt");
        if (outFile.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&outFile);
            if (!lines.isEmpty()) {
                            // Добавляем информацию о файле, из которого были извлечены строки
                            out << filePath << "\n";
                            out << lines.takeFirst() << "\n"; // Записываем первую строку
                        }
                        if (!lines.isEmpty()) {
                            out << lines.takeFirst() << "\n"; // Записываем вторую строку
                        }
                        outFile.close();
                    } else {
                        log("Ошибка открытия файла extracted_lines.txt для записи.");
                        qDebug() << "Ошибка открытия файла для записи извлеченных строк";
                    }

                    // Удаляем строки из исходного файла
                    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QTextStream out(&file);
                        for (const QString& remainingLine : lines) {
                            out << remainingLine << "\n";
                        }
                        log("Обработан инфицированный файл: " + filePath);
                        qDebug() << "Обработан инфицированный файл: " << filePath;
                    } else {
                        log("Ошибка открытия файла " + filePath + " для записи.");
                        qDebug() << "Ошибка открытия файла " << filePath << " для записи";
                    }
                } else {
                    log("Ошибка открытия файла " + filePath + " для чтения.");
                    qDebug() << "Ошибка открытия файла " << filePath << " для чтения";
                }
            }

            void FileMonitor::loadSignatures(const QString& filename) {
                QFile file(filename);
                if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&file);
                    QString line;
                    while (in.readLineInto(&line)) {
                        if (!line.trimmed().isEmpty()) {
                            QStringList fields = line.split(',', QString::SkipEmptyParts);
                            if (fields.size() == 5) { // Убедимся, что есть 5 полей
                                Signature sig;
                                sig.id = fields[0].trimmed();
                                sig.type = fields[1].trimmed();
                                sig.description = fields[2].trimmed();
                                sig.pattern = fields[3].trimmed();
                                sig.isRegex = (fields[4].trimmed().toLower() == "true");
                                signatures.append(sig); // Добавляем сигнатуру в вектор
                                log("Загружена сигнатура: " + sig.description + "--->"+sig.pattern); // Логируем загруженные сигнатуры
                                qDebug() << "Загружена сигнатура:" << sig.description<< "--->"<<sig.pattern;
                            }
                        }
                    }
                    file.close();
                } else {
                    log("Ошибка открытия файла сигнатур: " + filename);
                    qDebug() << "Ошибка открытия файла сигнатур";
                }
            }

            void FileMonitor::log(const QString& message) {
                QFile logFile("log.txt");
                if (logFile.open(QIODevice::Append | QIODevice::Text)) {
                    QTextStream out(&logFile);
                    out << QDateTime::currentDateTime().toString() << ": " << message << "\n";
                    logFile.close();
                } else {
                    qDebug() << "Ошибка открытия файла логов log.txt для записи.";
                }
            }

            QStringList FileMonitor::getInfectedFiles() const {
                return infectedFiles; // Возвращаем список инфицированных файлов
            }
            void FileMonitor::stopMonitoring() {
                if (watcher) {
                    watcher->removePaths(watcher->directories()); // Удаляем все пути из наблюдателя
                    watcher = nullptr; // Устанавливаем указатель в nullptr
                    log("Мониторинг был остановлен");
                }
            }
