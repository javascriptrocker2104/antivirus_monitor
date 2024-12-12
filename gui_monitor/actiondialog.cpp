#include "actiondialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

ActionDialog::ActionDialog(QWidget *parent) :
    QDialog(parent),
    deleteRadioButton(new QRadioButton("Удалить файл", this)),
    quarantineRadioButton(new QRadioButton("Карантин", this)),
    ignoreRadioButton(new QRadioButton("Игнорировать", this)),
    healRadioButton(new QRadioButton("Вылечить", this)),
    selected("") // Инициализируем выбранное действие пустой строкой
{
    // Установим заголовок окна
    setWindowTitle("Выберите действие");

    // Создаем вертикальный layout
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Добавляем радиокнопки в layout
    layout->addWidget(new QLabel("Выберите действие:", this));
    layout->addWidget(deleteRadioButton);
    layout->addWidget(quarantineRadioButton);
    layout->addWidget(ignoreRadioButton);
    layout->addWidget(healRadioButton);

    // Создаем кнопки "ОК" и "Отмена"
    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    QPushButton *confirmButton = buttonBox->addButton(QDialogButtonBox::Ok);
    QPushButton *cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);

    layout->addWidget(buttonBox);

    // Подключаем сигналы к слотам
    connect(confirmButton, &QPushButton::clicked, this, &ActionDialog::onConfirm);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject); // Закрываем диалог при нажатии "Отмена"

    setLayout(layout);
}

QString ActionDialog::selectedAction() const {
    return selected; // Возвращаем выбранное действие
}

void ActionDialog::onConfirm() {
    // Определяем, какая радиокнопка выбрана
    if (deleteRadioButton->isChecked()) {
        selected = "Удалить файл";
    } else if (quarantineRadioButton->isChecked()) {
        selected = "Карантин";
    } else if (ignoreRadioButton->isChecked()) {
        selected = "Игнорировать";
    } else if (healRadioButton->isChecked()) {
        selected = "Вылечить";
    }

    accept(); // Закрываем диалог с результатом "Принято"
}
