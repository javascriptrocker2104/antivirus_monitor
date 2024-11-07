#ifndef FILEMONITOR_H
#define FILEMONITOR_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QFileSystemWatcher>
#include <QTimer> // Добавьте этот заголовок для таймера

// Определение структуры Signature
struct Signature {
    QString id;
    QString type;
    QString description;
    QString pattern;
    bool isRegex;
};

class FileMonitor : public QObject {
    Q_OBJECT
private:
    QVector<Signature> signatures;
    QString directoryPath;
    QStringList previousFilesList;
    QFileSystemWatcher *watcher;
    QTimer *scanTimer; // Таймер для периодического сканирования
    QStringList infectedFiles;


public:
    explicit FileMonitor(const QString& path, QObject *parent = nullptr);
    void startMonitoring();
    void scanDirectory(const QString& dirPath);
    void onFileChanged(const QString& path);
    void onFileCreated(const QString& path);
    void onDirectoryChanged(const QString& path);
    bool isInfected(const QString& filePath);
    void handleInfection(const QString& filePath);
    void loadSignatures(const QString& filename);
    void log(const QString& message);
    void stopMonitoring();
    QStringList getInfectedFiles() const;

signals:
    void fileChanged(const QString& path);
    void fileCreated(const QString& path);
    void DirectoryChanged(const QString& path);
};

#endif // FILEMONITOR_H
