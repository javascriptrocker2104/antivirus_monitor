#include "filemonitor.h"
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QTimer>
#include <QRegularExpression>
#include <QProcess>
#include <QMessageBox>
#include <QDateTime>

FileMonitor::FileMonitor(const QString& path, QObject *parent)
    : QObject(parent), directoryPath(path), watcher(new QFileSystemWatcher(this)), timer(new QTimer(this)) {
    loadSignatures("/home/vladimir/Загрузки/Anne/gui_monitor/signatures.cvs");
    loadPreviousFilesList();
    processingQueue = QQueue<QString>();
    connect(timer, &QTimer::timeout, this, &FileMonitor::scanDirectory); // Исправлено
    timer->start(1000); // Пересканировать каждую 1 секунду. Настройте по необходимости.
}

void FileMonitor::startMonitoring() {
    log("Мониторинг папки начат: " + directoryPath);
    qDebug() << "Мониторинг папки начат:" << directoryPath;
    scanDirectory();
    connect(watcher, &QFileSystemWatcher::fileChanged, this, &FileMonitor::onFileChanged);
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, &FileMonitor::onDirectoryChanged);
}

void FileMonitor::scanDirectory() {
    QDir dir(directoryPath);
    QStringList currentFiles = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

    // Добавление уже существующих файлов в QFileSystemWatcher
    for (const QString& file : currentFiles) {
        QString fullPath = dir.absoluteFilePath(file);
        if (!previousFilesList.contains(fullPath)) {
            watcher->addPath(fullPath);
            qDebug() << "Обнаружен новый файл:" << fullPath;
            log("Обнаружен новый файл: " + fullPath);
            previousFilesList.append(fullPath);
            processingQueue.enqueue(fullPath);
            if (processingQueue.size() == 1) {
                processNextFile();
            }
        } else {
            // Если файл уже существует, добавляем его в watcher, если он еще не добавлен
            if (!watcher->files().contains(fullPath)) {
                watcher->addPath(fullPath);
                qDebug() << "Добавление существующего файла в монитор:" << fullPath;
            }
        }
    }

    // Проверка на удаленные файлы
    for (const QString& previousFile : previousFilesList) {
        if (!currentFiles.contains(QFileInfo(previousFile).fileName())) {
            onFileRemoved(previousFile);
        }
    }

    savePreviousFilesList();
}

void FileMonitor::onFileChanged(const QString& path) {
    if (!QFile::exists(path)) {
        return; // Если файл не существует, выход
    }
    qDebug() << "Файл изменен. Добавление в очередь обработки:" << path;
    log("Внимание: Файл изменен: " + path);
    processingQueue.enqueue(path);
    if (processingQueue.size() == 1) {
        processNextFile();
    }
}

void FileMonitor::onFileRemoved(const QString& path) {
    qDebug() << "Файл удален:" << path;
    log("Внимание: Файл удален: " + path);
    previousFilesList.removeOne(path);
    emit fileDeleted(path);
}

void FileMonitor::onDirectoryChanged(const QString& path) {
    qDebug() << "Директория изменена:" << path;
    scanDirectory();
}

void FileMonitor::processNextFile() {
    if (processingQueue.isEmpty()) {
        return; // Выход, если нет файлов для обработки
    }

    QString filePath = processingQueue.dequeue();
    qDebug() << "Обработка файла:" << filePath;

    // Здесь можно добавить вашу логику анализа сигнатур
    if (isInfected(filePath)) {
        qDebug() << "Обнаружен зараженный файл:" << filePath;
        switch (defaultAction) {
            case Heal:
                handleInfection(filePath);
                break;
            case Delete:
                if (!QFile::remove(filePath)) {
                    qDebug() << "Ошибка при удалении файла:" << filePath;
                    log("Ошибка при удалении файла: " + filePath);
                }
                break;
            case Ignore:
                log("Внимание: Зараженный файл игнорирован: " + filePath);
                break;
            case Quarantine:
                moveToQuarantine(filePath);
                break;
        }
    } else {
        log("Файл чист: " + filePath); // Логирование, если файл чист
    }

    // Продолжить обработку следующего файла
    if (!processingQueue.isEmpty()) {
        processNextFile();
    }
}

void FileMonitor::setDefaultAction(Action action) {
    defaultAction = action; // Сохраняем выбранное действие
}

void FileMonitor::moveToQuarantine(const QString& filePath) {
    QString quarantineDir = "/home/vladimir/Загрузки/Anne/Карантин"; // Укажите путь к папке карантина
    QDir().mkpath(quarantineDir); // Создаем папку, если она не существует

    // Получаем имя файла и создаем новую папку с его названием
    QString fileName = QFileInfo(filePath).fileName();
    QString infectedFolder = QDir(quarantineDir).filePath(fileName); // Папка с названием зараженного файла
    QDir().mkpath(infectedFolder); // Создаем папку для зараженного файла

    // Создаем новые файлы для хранения чистого и зараженного содержимого
    QString cleanFilePath = QDir(infectedFolder).filePath("clean_" + fileName);
    QString infectedSignaturesFilePath = QDir(infectedFolder).filePath("infected_signatures_" + fileName);

    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл:" << filePath;
        return;
    }

    QFile cleanFile(cleanFilePath);
    if (!cleanFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Не удалось создать файл:" << cleanFilePath;
        inputFile.close();
        return;
    }

    QFile infectedSignaturesFile(infectedSignaturesFilePath);
    if (!infectedSignaturesFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Не удалось создать файл:" << infectedSignaturesFilePath;
        inputFile.close();
        cleanFile.close();
        return;
    }

    QTextStream in(&inputFile);
    QTextStream outClean(&cleanFile);
    QTextStream outInfected(&infectedSignaturesFile);

    QString line;
    while (in.readLineInto(&line)) {
        bool isInfected = false; // Флаг для отслеживания зараженной строки

        for (const Signature& signature : signatures) {
            if (signature.isRegex) {
                QRegularExpression regex(signature.pattern);
                if (regex.match(line).hasMatch()) {
                    outInfected << line << "\n"; // Записываем в файл с зараженными сигнатурами
                    isInfected = true; // Устанавливаем флаг
                    break; // Прерываем цикл, если нашли заражение
                }
            } else {
                if (line.contains(signature.pattern, Qt::CaseInsensitive)) {
                    outInfected << line << "\n"; // Записываем в файл с зараженными сигнатурами
                    isInfected = true; // Устанавливаем флаг
                    break; // Прерываем цикл, если нашли заражение
                }
            }
        }

        if (!isInfected) {
            outClean << line << "\n"; // Записываем в чистый файл, если строка не заражена
        }
    }

    // Закрываем файлы
    inputFile.close();
    cleanFile.close();
    infectedSignaturesFile.close();

    // Удаляем исходный файл, если он больше не нужен
    QFile::remove(filePath);
    log("Warning: Файл перемещен в карантин: " + cleanFilePath);
    qDebug() << "Файл перемещен в карантин:" << cleanFilePath;

    // Заархивировать папку с файлами
    if (archiveWithPassword(infectedFolder)) {
        // Если архивирование прошло успешно, удаляем папку
        QDir dir(infectedFolder);
        dir.removeRecursively(); // Удаляем папку с зараженным файлом
        qDebug() << "Папка с зараженным файлом удалена:" << infectedFolder;
    }
}

bool FileMonitor::archiveWithPassword(const QString& folderPath) {
    QString archiveFilePath = folderPath + ".zip"; // Путь к архиву
    QString command = QString("zip -r -P pass \"%1\" \"%2\"").arg(archiveFilePath, folderPath); // Команда для архивирования

    // Выполняем команду
    QProcess process;
    process.start(command);
    process.waitForFinished(); // Ждем завершения команды

    // Проверяем статус завершения
    if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
        log("Warning: Папка успешно заархивирована с паролем: " + archiveFilePath);
        qDebug() << "Папка успешно заархивирована:" << archiveFilePath;
        return true; // Возвращаем true для успешного архивирования
    } else {
        log("Warning: Ошибка при архивировании папки: " + folderPath);
        qDebug() << "Ошибка при архивировании:" << process.readAllStandardError();
        return false; // Возвращаем false в случае ошибки
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

        QFile outFile("extracted_lines.txt");
        if (outFile.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&outFile);
            if (!lines.isEmpty()) {
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

        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (const QString& remainingLine : lines) {
                out << remainingLine << "\n";
            }
            log("Error: Обработан инфицированный файл: " + filePath);
            qDebug() << "Обработан инфицированный файл: " << filePath;
        } else {
            log("Ошибка открытия файла " + filePath + " для записи.");
            qDebug() << "Ошибка открытия файла " << filePath + " для записи";
        }
    } else {
        log("Ошибка открытия файла " + filePath + " для чтения.");
        qDebug() << "Ошибка открытия файла " << filePath + " для чтения";
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
                    qDebug() << "Загружена сигнатура:" << sig.description << "--->" << sig.pattern;
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
        QMessageBox::warning(nullptr, "Ошибка", "Не удалось открыть файл логов для записи.");
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

void FileMonitor::savePreviousFilesList() {
    QFile file("/home/vladimir/Загрузки/Anne/gui_monitor/previous_files.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString& filePath : previousFilesList) {
            out << filePath << "\n";
        }
        file.close();
    } else {
        qDebug() << "Ошибка открытия файла previous_files.txt для записи.";
    }
}

void FileMonitor::loadPreviousFilesList() {
    QFile file("/home/vladimir/Загрузки/Anne/gui_monitor/previous_files.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                previousFilesList.append(line);
            }
        }
        file.close();
    } else {
        qDebug() << "Ошибка открытия файла previous_files.txt для чтения.";
    }
}
