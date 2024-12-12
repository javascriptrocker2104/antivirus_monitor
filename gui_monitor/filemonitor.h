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

// Определение структуры Signature
struct Signature {
    QString id;
    QString type;
    QString description;
    QString pattern;
    bool isRegex;
};

// Объявление перечисления Action
enum Action {
    Heal,
    Delete,
    Ignore,
    Quarantine
};

class FileMonitor : public QObject {
    Q_OBJECT

public:
    explicit FileMonitor(const QString& path, QObject *parent = nullptr);
    void startMonitoring();
    void stopMonitoring();
    void setDefaultAction(Action action);

signals:
    void fileChanged(const QString &path);
    void directoryChanged(const QString &path);
    void fileCreated(const QString& path);
    void fileDeleted(const QString& path);

private slots:
    void onFileChanged(const QString& path);
    void onDirectoryChanged(const QString& path);

private:
    void scanDirectory(const QString& dirPath);
    bool isInfected(const QString& filePath);
    void handleInfection(const QString& filePath);
    void promptUser  (const QString& filePath);
    void moveToQuarantine(const QString& filePath);
    bool archiveWithPassword(const QString& filePath);
    void loadSignatures(const QString& filename);
    void log(const QString& message);

    QString directoryPath;
    QFileSystemWatcher *watcher;
    QVector<Signature> signatures;
    QStringList previousFilesList;
    QStringList infectedFiles;
    QStringList getInfectedFiles() const;
    QFile* currentFile;
    QTimer* timer;
    Action defaultAction;
};

#endif // FILEMONITOR_H
