#include "filemonitor.h"
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QTimer>

FileMonitor::FileMonitor(const QString& path, QObject *parent)
    : QObject(parent), directoryPath(path), watcher(new QFileSystemWatcher(this)), timer(new QTimer(this)) {
    loadSignatures("/home/vladimir/Загрузки/Anne/gui_monitor/signatures.cvs"); // Загружаем сигнатуры
    loadPreviousFilesList(); // Загружаем список предыдущих файлов
    processingQueue = QQueue<QString>();
    connect(timer, &QTimer::timeout, this, &FileMonitor::scanDirectory); // Подключаем таймер к функции сканирования
    timer->start(1000); // Пересканировать каждую 1 секунду.
}

//Запуск мониторинга указанной директории.
void FileMonitor::startMonitoring() {
    log("Мониторинг папки начат: " + directoryPath);
    qDebug() << "Мониторинг папки начат:" << directoryPath;
    scanDirectory(); // Начинаем сканирование директории
    connect(watcher, &QFileSystemWatcher::fileChanged, this, &FileMonitor::onFileChanged); // Подключаем сигнал изменения файла
}

//Сканирование указанной директории на наличие новых или измененных файлов.
void FileMonitor::scanDirectory() {
    QDir dir(directoryPath);
    QStringList currentFiles = dir.entryList(QDir::Files | QDir::NoDotAndDotDot); // Получаем список текущих файлов

    // Добавление новых файлов в QFileSystemWatcher
    for (const QString& file : currentFiles) {
        QString fullPath = dir.absoluteFilePath(file);
        if (!previousFilesList.contains(fullPath)) { // Если файл новый
            watcher->addPath(fullPath); // Добавляем в монитор
            qDebug() << "Обнаружен новый файл:" << fullPath;
            log("[Информация] Обнаружен новый файл: " + fullPath);
            previousFilesList.append(fullPath); // Сохраняем файл в списке
            processingQueue.enqueue(fullPath); // Добавляем файл в очередь обработки
            if (processingQueue.size() == 1) {
                processNextFile(); // Начинаем обработку файла
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
            onFileRemoved(previousFile); // Обработка удаления файла
        }
    }

    savePreviousFilesList(); // Сохраняем список предыдущих файлов
}

//Обрабатка изменений в файле.
void FileMonitor::onFileChanged(const QString& path) {
    if (!QFile::exists(path)) {
        return; // Если файл не существует, выход
    }
    qDebug() << "Файл изменен. Добавление в очередь обработки:" << path;
    log("[Информация] Файл изменен: " + path);
    processingQueue.enqueue(path); // Добавляем измененный файл в очередь
    if (processingQueue.size() == 1) {
        processNextFile(); // Начинаем обработку файла
    }
}

//Обрабатка удаления файла.
void FileMonitor::onFileRemoved(const QString& path) {
    qDebug() << "Файл удален:" << path;
    log("[Информация] Файл удален: " + path);
    previousFilesList.removeOne(path); // Удаляем файл из списка
    emit fileDeleted(path); // Извещаем об удалении файла
}

//Проверка файла на наличие сигнатур и выполение соответствующих действий.
void FileMonitor::processNextFile() {
    if (processingQueue.isEmpty()) {
        return; // Выход, если нет файлов для обработки
    }

    QString filePath = processingQueue.dequeue(); // Получаем следующий файл для обработки
    qDebug() << "Обработка файла:" << filePath;

    // Логика анализа сигнатур
    if (isInfected(filePath)) {
        qDebug() << "Обнаружен зараженный файл:" << filePath;
        log("[Внимание] Обнаружен зараженный файл:" + filePath);
        switch (defaultAction) {
            case Heal:
                handleInfection(filePath); // Лечим файл
                break;
            case Delete:
                if (!QFile::remove(filePath)) {
                    qDebug() << "Ошибка при удалении файла:" << filePath;
                    log("Ошибка при удалении файла: " + filePath);
                }
                break;
            case Ignore:
                log("[Внимание] Зараженный файл был проигнорирован: " + filePath);
                break;
            case Quarantine:
                moveToQuarantine(filePath); // Перемещение в карантин
                break;
        }
    } else {
        log("[Информация] Файл чист: " + filePath);
    }

    // Продолжить обработку следующего файла
    if (!processingQueue.isEmpty()) {
        processNextFile();
    }
}

// Устанавка действия для обработки зараженных файлов.
void FileMonitor::setDefaultAction(Action action) {
    defaultAction = action; // Сохраняем выбранное действие
}

//Перемещение зараженного файла в карантин.
void FileMonitor::moveToQuarantine(const QString& filePath) {
    QString quarantineDir = "/home/vladimir/Загрузки/Anne/Карантин"; // Папка карантина
    QDir().mkpath(quarantineDir); // Создаем папку, если она не существует

    // Получаем имя файла и создаем новую папку с его названием
    QString fileName = QFileInfo(filePath).fileName();
    QString infectedFolder = QDir(quarantineDir).filePath(fileName); // Папка с названием зараженного файла
    QDir().mkpath(infectedFolder); // Создаем папку для зараженного файла

    // Создаем новые файлы для хранения первых двух строк и остального содержимого
    QString firstTwoLinesFilePath = QDir(infectedFolder).filePath("first_two_lines_" + fileName);
    QString remainingContentFilePath = QDir(infectedFolder).filePath("remaining_content_" + fileName);

    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл:" << filePath;
        return;
    }

    QFile firstTwoLinesFile(firstTwoLinesFilePath);
    if (!firstTwoLinesFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Не удалось создать файл:" << firstTwoLinesFilePath;
        inputFile.close();
        return;
    }

    QFile remainingContentFile(remainingContentFilePath);
    if (!remainingContentFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Не удалось создать файл:" << remainingContentFilePath;
        inputFile.close();
        firstTwoLinesFile.close();
        return;
    }

    QTextStream in(&inputFile);
    QTextStream outFirstTwo(&firstTwoLinesFile);
    QTextStream outRemaining(&remainingContentFile);

    QString line;
    int lineCount = 0;
    while (in.readLineInto(&line)) {
        if (lineCount < 2) {
            outFirstTwo << line << "\n"; // Записываем первые две строки
        } else {
            outRemaining << line << "\n"; // Записываем все остальные строки
        }
        lineCount++;
    }

    // Закрываем файлы
    inputFile.close();
    firstTwoLinesFile.close();
    remainingContentFile.close();

    // Удаляем исходный файл
    QFile::remove(filePath);
    log("[Информация] Файл перемещен в карантин: " + firstTwoLinesFilePath);
    qDebug() << "Файл перемещен в карантин:" << firstTwoLinesFilePath;

    // Заархивировать папку с файлами
    if (archiveWithPassword(infectedFolder)) {
        // Если архивирование прошло успешно, удаляем папку
        QDir dir(infectedFolder);
        dir.removeRecursively(); // Удаляем папку с зараженным файлом
        qDebug() << "Папка с зараженным файлом удалена:" << infectedFolder;
    }
}

// Архивация указанной папки с установленным паролем.
bool FileMonitor::archiveWithPassword(const QString& folderPath) {
    QString archiveFilePath = folderPath + ".zip"; // Путь к архиву
    QString command = QString("zip -r -P pass \"%1\" \"%2\"").arg(archiveFilePath, folderPath); // Команда для архивирования

    // Выполняем команду
    QProcess process;
    process.start(command);
    process.waitForFinished(); // Ждем завершения команды

    // Проверяем статус завершения
    if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
        qDebug() << "Папка успешно заархивирована:" << archiveFilePath;
        return true; // Успешно архивировано
    } else {
        log("Ошибка при архивировании папки: " + folderPath);
        qDebug() << "Ошибка при архивировании:" << process.readAllStandardError();
        return false; // Ошибка архивирования
    }
}

//Проверка, инфицирован ли файл.
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

// * Удаление строки с сигнатурами, если действие "Вылечить".
void FileMonitor::handleInfection(const QString& filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QStringList lines;
        QTextStream in(&file);
        QString line;
        while (in.readLineInto(&line)) {
            lines.append(line); // Сохраняем все строки файла
        }
        file.close();

        // Если действие "Вылечить", удаляем строки с сигнатурами
        if (defaultAction == Heal) {
            QFile outFile(filePath);
            if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&outFile);
                for (const QString& remainingLine : lines) {
                    bool isInfected = false; // Флаг для отслеживания зараженной строки
                    for (const Signature& signature : signatures) {
                        if (signature.isRegex) {
                            QRegularExpression regex(signature.pattern);
                            if (regex.match(remainingLine).hasMatch()) {
                                isInfected = true; // Устанавливаем флаг
                                break; // Прерываем цикл, если нашли заражение
                            }
                        } else {
                            if (remainingLine.contains(signature.pattern, Qt::CaseInsensitive)) {
                                isInfected = true; // Устанавливаем флаг
                                break; // Прерываем цикл, если нашли заражение
                            }
                        }
                    }
                    if (!isInfected) {
                        out << remainingLine << "\n"; // Записываем строку, если она не заражена
                    }
                }
                outFile.close();
                log("[Информация] Файл был вылечен: " + filePath);
                qDebug() << "Файл был вылечен: " << filePath;
            } else {
                log("Ошибка открытия файла " + filePath + " для записи.");
                qDebug() << "Ошибка открытия файла " << filePath + " для записи";
            }
        }
    } else {
        log("Ошибка открытия файла " + filePath + " для чтения.");
        qDebug() << "Ошибка открытия файла " << filePath + " для чтения";
    }
}

//Загрузка сигнатур из указанного файла.
void FileMonitor::loadSignatures(const QString& filename) {
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line;

        // Пропускаем первую строку (заголовки)
        if (!in.readLineInto(&line)) {
            log("Ошибка чтения заголовка из файла сигнатур: " + filename);
            return; // Выход, если не удалось прочитать заголовок
        }

        // Чтение остальных строк
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

// Логирование
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

//Останавка мониторинга директории.
void FileMonitor::stopMonitoring() {
    if (watcher) {
        watcher->removePaths(watcher->directories()); // Удаляем все пути из наблюдателя
        watcher = nullptr; // Устанавливаем указатель в nullptr
        log("Мониторинг был остановлен"); // Логируем остановку мониторинга
    }
}

//Сохранение списка предыдущих файлов в файл.
void FileMonitor::savePreviousFilesList() {
    QFile file("/home/vladimir/Загрузки/Anne/gui_monitor/previous_files.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString& filePath : previousFilesList) {
            out << filePath << "\n"; // Сохраняем каждый файл в списке
        }
        file.close();
    } else {
        qDebug() << "Ошибка открытия файла previous_files.txt для записи.";
    }
}

//Загрузка списка предыдущих файлов из файла.
void FileMonitor::loadPreviousFilesList() {
    QFile file("/home/vladimir/Загрузки/Anne/gui_monitor/previous_files.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                previousFilesList.append(line); // Добавляем файл в список
            }
        }
        file.close();
    } else {
        qDebug() << "Ошибка открытия файла previous_files.txt для чтения.";
    }
}
