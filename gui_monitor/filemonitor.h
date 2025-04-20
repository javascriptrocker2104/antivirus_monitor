#ifndef FILEMONITOR_H
#define FILEMONITOR_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QString>
#include <QVector>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <QDateTime>
#include <QRegularExpression>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>
#include <QQueue>

// Структура для хранения сигнатуры вируса
struct Signature {
    QString id;
    QString type;
    QString description;
    QByteArray pattern;  // Изменено с QString на QByteArray
    bool isRegex;
    qint64 offset;       // Добавлено: позиция в файле для поиска
    qint64 maxScanSize;  // Добавлено: максимальный размер сканирования
};



class FileMonitor : public QObject {
    Q_OBJECT


public:
    // Перечисление возможных действий при обнаружении вируса
    enum Action {
        Heal,
        Delete,
        Ignore,
        Quarantine
    };
    Q_ENUM(Action)


    explicit FileMonitor(const QString& path, QObject *parent = nullptr);//Конструктор
    void startMonitoring();   // Начать мониторинг
    void stopMonitoring();    // Остановить мониторинг
    void setDefaultAction(Action action); // Установить действие по умолчанию

signals:
    void fileChanged(const QString &path); // Сигнал о изменении файла
    void fileDeleted(const QString& path);  // Сигнал о удалении файла

private slots:
    void onFileChanged(const QString& path); // Обработчик изменения файла

private:
    void scanDirectory();                  // Сканировать директорию
    bool isInfected(const QString& filePath); // Проверить файл на зараженность
    void handleInfection(const QString& filePath); // Обработать зараженный файл
    void moveToQuarantine(const QString& filePath); // Переместить файл в карантин
    bool archiveWithPassword(const QString& filePath); // Архивировать файл с паролем
    void loadSignatures(const QString& filename); // Загрузить сигнатуры из файла
    void log(const QString& message);      // Логирование сообщений
    void savePreviousFilesList();          // Сохранить список ранее обработанных файлов
    void loadPreviousFilesList();          // Загрузить список ранее обработанных файлов
    void processFile(const QString& filePath);                // Обработать следующий файл из очереди
    void onFileRemoved(const QString& path); // Обработчик удаления файла
    void healFile(const QString& filePath);

    QString directoryPath;                  // Путь к директории
    QFileSystemWatcher *watcher;           // Наблюдатель за файловой системой
    QVector<Signature> signatures;          // Сигнатуры вирусов
    QStringList previousFilesList;         // Список ранее обработанных файлов
    QStringList infectedFiles;             // Список зараженных файлов
    QFile* currentFile;                     // Текущий обрабатываемый файл
    QTimer* timer;                          // Таймер для периодических задач
    Action defaultAction;                   // Действие по умолчанию при заражении
    QQueue<QString> processingQueue;        // Очередь файлов для обработки
};

#endif // FILEMONITOR_H
