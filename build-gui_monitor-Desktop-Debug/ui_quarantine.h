/********************************************************************************
** Form generated from reading UI file 'quarantine.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QUARANTINE_H
#define UI_QUARANTINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QDialog>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Quarantine
{
public:
    QListWidget *listWidget;

    void setupUi(QDialog *Quarantine)
    {
        if (Quarantine->objectName().isEmpty())
            Quarantine->setObjectName(QString::fromUtf8("Quarantine"));
        Quarantine->resize(800, 600);

        // Устанавливаем вертикальный layout
        QVBoxLayout *layout = new QVBoxLayout(Quarantine);
        listWidget = new QListWidget(Quarantine);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        layout->addWidget(listWidget); // Добавляем listWidget в layout

        Quarantine->setLayout(layout); // Устанавливаем layout для QDialog

        retranslateUi(Quarantine);

        QMetaObject::connectSlotsByName(Quarantine);
    } // setupUi

    void retranslateUi(QDialog *Quarantine)
    {
        Quarantine->setWindowTitle(QApplication::translate("Quarantine", "Карантин", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Quarantine: public Ui_Quarantine {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QUARANTINE_H
