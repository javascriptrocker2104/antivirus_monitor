#ifndef ACTIONDIALOG_H
#define ACTIONDIALOG_H

#include <QDialog>
#include "filemonitor.h"

class QRadioButton;

class ActionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ActionDialog(QWidget *parent = nullptr);

    QString selectedAction() const;
    FileMonitor::Action selectedActionType() const;

private slots:
    void onConfirm();

private:
    QRadioButton *deleteRadioButton;
    QRadioButton *quarantineRadioButton;
    QRadioButton *ignoreRadioButton;
    QRadioButton *healRadioButton;
    QString selected;
};

#endif // ACTIONDIALOG_H
