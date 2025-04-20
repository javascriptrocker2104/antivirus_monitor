#include "filemonitor.h"
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QDateTime>
#include <QProcess>
#include <QRegularExpression>

FileMonitor::FileMonitor(const QString& path, QObject *parent)
    : QObject(parent), directoryPath(path), watcher(new QFileSystemWatcher(this)),defaultAction(Heal)  {
    loadSignatures("/home/vladimir/Загрузки/Anne/gui_monitor/signatures.cvs");
    loadPreviousFilesList();
}

void FileMonitor::startMonitoring() {
    log("Мониторинг папки начат: " + directoryPath);
    qDebug() << "Мониторинг папки начат:" << directoryPath;

    // Добавляем саму директорию и все существующие файлы
    watcher->addPath(directoryPath);

    QDir dir(directoryPath);
    QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

    // Очищаем предыдущий список и добавляем текущие файлы
    previousFilesList.clear();
    for (const QString &file : files) {
        QString fullPath = dir.absoluteFilePath(file);
        watcher->addPath(fullPath);
        previousFilesList.append(fullPath);
        qDebug() << "Добавлен в watcher:" << fullPath;
    }

    savePreviousFilesList();

    connect(watcher, &QFileSystemWatcher::fileChanged,
            this, &FileMonitor::onFileChanged);
    connect(watcher, &QFileSystemWatcher::directoryChanged,
            this, &FileMonitor::scanDirectory);
}

void FileMonitor::scanDirectory() {
    QDir dir(directoryPath);
    QStringList currentFiles = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

    // Обновляем список файлов в watcher
    QStringList watchedFiles = watcher->files();
    qDebug() << "Текущие файлы в директории:" << currentFiles;
    qDebug() << "Отслеживаемые файлы:" << watcher->files();
    // Проверяем новые файлы
    for (const QString& file : currentFiles) {
        QString fullPath = dir.absoluteFilePath(file);

        if (!watchedFiles.contains(fullPath)) {
            watcher->addPath(fullPath);
            if (!previousFilesList.contains(fullPath)) {
                qDebug() << "Обнаружен новый файл:" << fullPath;
                log("[Информация] Обнаружен новый файл: " + fullPath);
                previousFilesList.append(fullPath);
                processFile(fullPath);
            }
        }
    }

    // Проверяем удаленные файлы
    for (const QString& watchedFile : watchedFiles) {
        if (!currentFiles.contains(QFileInfo(watchedFile).fileName())) {
            watcher->removePath(watchedFile);
            onFileRemoved(watchedFile);
        }
    }

    savePreviousFilesList();
}

void FileMonitor::onFileChanged(const QString &path) {
    qDebug() << "Файл изменен:" << path;

    // Если файл временно недоступен (например, при перезаписи)
    if (!QFile::exists(path)) {
        QTimer::singleShot(100, [this, path]() {
            if (QFile::exists(path)) {
                watcher->addPath(path);
                processFile(path);
            }
        });
        return;
    }

    // Переподписываемся на файл
    watcher->addPath(path);
    processFile(path);
}

void FileMonitor::onFileRemoved(const QString& path) {
    qDebug() << "Файл удален:" << path;
    log("[Информация] Файл удален: " + path);
    previousFilesList.removeOne(path);
    savePreviousFilesList();
    emit fileDeleted(path);
}

void FileMonitor::processFile(const QString &filePath) {
    if (!QFile::exists(filePath)) {
        qDebug() << "Файл не существует:" << filePath;
        return;
    }

    if (isInfected(filePath)) {
        qDebug() << "Обнаружен зараженный файл:" << filePath;
        log("[Внимание] Обнаружен зараженный файл:" + filePath);
        handleInfection(filePath);
    } else {
        log("[Информация] Файл чист: " + filePath);
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

// Проверка, инфицирован ли файл.
bool FileMonitor::isInfected(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Ошибка открытия файла для сканирования:" << filePath;
        return false;
    }

    for (const Signature& signature : signatures) {
        if (signature.pattern.isEmpty()) continue;

        // Определяем область сканирования
        qint64 scanSize = qMin<qint64>(file.size(), signature.maxScanSize > 0 ?
                                      signature.maxScanSize : file.size());
        qint64 startPos = qMax<qint64>(0, signature.offset);

        // Для больших файлов читаем блоками
        const qint64 bufferSize = 4096;
        QByteArray buffer;
        qint64 bytesRead = 0;

        file.seek(startPos);
        while (bytesRead < scanSize) {
            qint64 bytesToRead = qMin(bufferSize, scanSize - bytesRead);
            buffer = file.read(bytesToRead);
            bytesRead += buffer.size();

            if (buffer.isEmpty()) break;

            // Поиск сигнатуры в буфере
            if (buffer.contains(signature.pattern)) {
                qDebug() << "Найдена сигнатура" << signature.id
                         << "в файле" << filePath;
                file.close();
                return true;
            }

            // Для regex проверяем как строку
            if (signature.isRegex) {
                QString data = QString::fromLatin1(buffer);
                QRegularExpression regex(QString::fromLatin1(signature.pattern));
                if (regex.match(data).hasMatch()) {
                    file.close();
                    return true;
                }
            }
        }
    }

    file.close();
    return false;
}

// * Удаление строки с сигнатурами, если действие "Вылечить".
void FileMonitor::handleInfection(const QString& filePath) {
    switch(defaultAction) {
        case Quarantine:
            moveToQuarantine(filePath);
            break;
        case Delete:
            QFile::remove(filePath);
            break;
        case Heal:
            healFile(filePath); // Вынесем лечение в отдельный метод
            break;
        case Ignore:
            log("[Информация] Угроза проигнорирована: " + filePath);
            break;
        default:
            moveToQuarantine(filePath); // Действие по умолчанию
    }
}

void FileMonitor::healFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite)) {
        log("Ошибка открытия файла для лечения: " + filePath);
        return;
    }

    QByteArray fileData = file.readAll();
    bool modified = false;

    for (const Signature& signature : signatures) {
        if (signature.pattern.isEmpty()) continue;

        int pos = 0;
        while ((pos = fileData.indexOf(signature.pattern, pos)) != -1) {
            fileData.replace(pos, signature.pattern.size(),
                           QByteArray(signature.pattern.size(), '\0'));
            pos += signature.pattern.size();
            modified = true;
        }
    }

    if (modified) {
        file.resize(0);
        file.write(fileData);
        log("[Информация] Файл вылечен: " + filePath);
    }
    file.close();
}
//Загрузка сигнатур из указанного файла.
void FileMonitor::loadSignatures(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        log("Ошибка открытия файла сигнатур: " + filename);
        return;
    }

    QTextStream in(&file);
    QString line;

    // Пропускаем заголовок
    if (!in.readLineInto(&line)) return;

    while (in.readLineInto(&line)) {
        line = line.trimmed();
        if (line.isEmpty()) continue;

        QStringList fields = line.split(',');
        if (fields.size() < 5) continue;

        Signature sig;
        sig.id = fields[0].trimmed();
        sig.type = fields[1].trimmed();
        sig.description = fields[2].trimmed();

        // Преобразуем шаблон в бинарный вид
        QString patternStr = fields[3].trimmed();
        if (patternStr.startsWith("hex:")) {
            // HEX-представление: "hex:4D5A90"
            sig.pattern = QByteArray::fromHex(
                patternStr.mid(4).toLatin1());
        } else if (patternStr.startsWith("str:")) {
            // Строковое представление: "str:MZ"
            sig.pattern = patternStr.mid(4).toLatin1();
        } else {
            // По умолчанию считаем HEX
            sig.pattern = QByteArray::fromHex(patternStr.toLatin1());
        }

        sig.isRegex = fields[4].trimmed().compare("true", Qt::CaseInsensitive) == 0;
        sig.offset = fields.size() > 5 ? fields[5].toLongLong() : 0;
        sig.maxScanSize = fields.size() > 6 ? fields[6].toLongLong() : 0;

        signatures.append(sig);
        qDebug() << "Загружена сигнатура:" << sig.description
                 << "Pattern size:" << sig.pattern.size()
                 << "Offset:" << sig.offset;
    }

    file.close();
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
