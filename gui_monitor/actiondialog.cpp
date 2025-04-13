#include "actiondialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

ActionDialog::ActionDialog(QWidget *parent) :
    QDialog(parent),
    deleteRadioButton(new QRadioButton("Удалить", this)),
    quarantineRadioButton(new QRadioButton("Переместить в карантин", this)),
    ignoreRadioButton(new QRadioButton("Игнорировать", this)),
    healRadioButton(new QRadioButton("Вылечить", this)),
    selected("") // Инициализация выбранного действия
{
    setWindowTitle("Настройки"); // Заголовок окна

    QVBoxLayout *layout = new QVBoxLayout(this); // Вертикальная раскладка

    // Добавление радиокнопок и метки в layout
    layout->addWidget(new QLabel("Что необходимо сделать с зараженным файлом?", this));
    layout->addWidget(deleteRadioButton);
    layout->addWidget(quarantineRadioButton);
    layout->addWidget(ignoreRadioButton);
    layout->addWidget(healRadioButton);

    // Создание кнопок "ОК" и "Отмена"
    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    QPushButton *confirmButton = buttonBox->addButton(QDialogButtonBox::Ok);
    QPushButton *cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);

    layout->addWidget(buttonBox); // Добавление кнопок в layout

    // Подключение сигналов к слотам
    connect(confirmButton, &QPushButton::clicked, this, &ActionDialog::onConfirm);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject); // Закрытие диалога при нажатии "Отмена"

    setLayout(layout); // Установка layout
}

QString ActionDialog::selectedAction() const {
    return selected; // Возвращение выбранного действия
}

void ActionDialog::onConfirm() {
    // Определение выбранной радиокнопки
    if (deleteRadioButton->isChecked()) {
        selected = "Удалить";
    } else if (quarantineRadioButton->isChecked()) {
        selected = "Переместить в карантин";
    } else if (ignoreRadioButton->isChecked()) {
        selected = "Игнорировать";
    } else if (healRadioButton->isChecked()) {
        selected = "Вылечить";
    }

    accept(); // Закрытие диалога с результатом "Принято"
}
