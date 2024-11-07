#ifndef QUARANTINE_H
#define QUARANTINE_H

#include <QDialog>
#include <QFileSystemWatcher>
#include <QString>

namespace Ui {
class Quarantine;
}

class Quarantine : public QDialog // Убедитесь, что это QDialog
{
    Q_OBJECT

public:
    explicit Quarantine(QWidget *parent = nullptr);
    ~Quarantine();

private slots:
    void removedLines(const QString& filePath);

private:
    Ui::Quarantine *ui;
    QFileSystemWatcher *fileWatcher; // Объявление fileWatcher
};

#endif // QUARANTINE_H
