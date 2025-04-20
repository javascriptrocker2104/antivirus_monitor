#include "actiondialog.h"
#include "filemonitor.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QRadioButton>
#include <QPushButton>

ActionDialog::ActionDialog(QWidget *parent) :
    QDialog(parent),
    deleteRadioButton(new QRadioButton(tr("Удалить"), this)),
    quarantineRadioButton(new QRadioButton(tr("Переместить в карантин"), this)),
    ignoreRadioButton(new QRadioButton(tr("Игнорировать"), this)),
    healRadioButton(new QRadioButton(tr("Вылечить"), this))
{
    selected = ""; // Инициализация в теле конструктора

    setWindowTitle(tr("Действие для зараженного файла"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(400, 200);

    // Настройка радиокнопок
    healRadioButton->setChecked(true);
    selected = "Вылечить";

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel(tr("Выберите действие для зараженного файла:"), this));

    QVBoxLayout *radioLayout = new QVBoxLayout();
    radioLayout->addWidget(healRadioButton);
    radioLayout->addWidget(quarantineRadioButton);
    radioLayout->addWidget(deleteRadioButton);
    radioLayout->addWidget(ignoreRadioButton);
    radioLayout->setSpacing(10);

    mainLayout->addLayout(radioLayout);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal,
        this
    );

    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &ActionDialog::onConfirm);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto updateSelected = [this]() {
        if (healRadioButton->isChecked()) selected = "Вылечить";
        else if (quarantineRadioButton->isChecked()) selected = "Переместить в карантин";
        else if (deleteRadioButton->isChecked()) selected = "Удалить";
        else if (ignoreRadioButton->isChecked()) selected = "Игнорировать";
    };

    connect(healRadioButton, &QRadioButton::toggled, updateSelected);
    connect(quarantineRadioButton, &QRadioButton::toggled, updateSelected);
    connect(deleteRadioButton, &QRadioButton::toggled, updateSelected);
    connect(ignoreRadioButton, &QRadioButton::toggled, updateSelected);
}

QString ActionDialog::selectedAction() const {
    return selected;
}

FileMonitor::Action ActionDialog::selectedActionType() const {
    if (selected == "Удалить") return FileMonitor::Delete;
    if (selected == "Переместить в карантин") return FileMonitor::Quarantine;
    if (selected == "Игнорировать") return FileMonitor::Ignore;
    return FileMonitor::Heal; // По умолчанию
}

void ActionDialog::onConfirm() {
    accept();
}
