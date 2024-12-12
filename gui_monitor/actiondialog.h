#ifndef ACTIONDIALOG_H
#define ACTIONDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>

class ActionDialog : public QDialog {
    Q_OBJECT

public:
    explicit ActionDialog(QWidget *parent = nullptr);
    QString selectedAction() const;
private slots:
    void onConfirm();

private:
    QRadioButton *deleteRadioButton;
    QRadioButton *quarantineRadioButton;
    QRadioButton *ignoreRadioButton;
    QRadioButton *healRadioButton;
    QString selected; // Добавьте это поле
};

#endif // ACTIONDIALOG_H
