#ifndef ACTIONDIALOG_H
#define ACTIONDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>

// Класс для диалогового окна выбора действия с зараженным файлом
class ActionDialog : public QDialog {
    Q_OBJECT

public:
    explicit ActionDialog(QWidget *parent = nullptr); // Конструктор
    QString selectedAction() const; // Метод для получения выбранного действия

private slots:
    void onConfirm(); // Слот для обработки подтверждения выбора

private:
    QRadioButton *deleteRadioButton; // Радиокнопка для удаления
    QRadioButton *quarantineRadioButton; // Радиокнопка для карантина
    QRadioButton *ignoreRadioButton; // Радиокнопка для игнорирования
    QRadioButton *healRadioButton; // Радиокнопка для лечения
    QString selected; // Хранит выбранное действие
};

#endif // ACTIONDIALOG_H
