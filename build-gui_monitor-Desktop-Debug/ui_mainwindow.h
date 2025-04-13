/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *settingsAction;
    QAction *exitAction;
    QWidget *centralwidget;
    QLabel *headerLabel;
    QPushButton *selectFolderButton;
    QPushButton *pushButton;
    QPushButton *stopButton;
    QListWidget *listWidget;
    QLabel *statusLabel;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(900, 650);
        MainWindow->setStyleSheet(QString::fromUtf8("\n"
"   /* Main window style */\n"
"   QMainWindow {\n"
"       background: qlineargradient(x1:0, y1:0, x2:1, y2:1,\n"
"                                 stop:0 #1a2a6c, stop:1 #b21f1f);\n"
"   }\n"
"\n"
"   /* Menu bar style */\n"
"   QMenuBar {\n"
"       background: qlineargradient(x1:0, y1:0, x2:1, y2:1,\n"
"                                 stop:0 #34495e, stop:1 #2c3e50);\n"
"       color: white;\n"
"       font-weight: bold;\n"
"   }\n"
"\n"
"   QMenuBar::item {\n"
"       background: transparent;\n"
"       padding: 5px 10px;\n"
"   }\n"
"\n"
"   QMenuBar::item:selected {\n"
"       background: #2980b9; /* \320\246\320\262\320\265\321\202 \321\204\320\276\320\275\320\260 \320\277\321\200\320\270 \320\275\320\260\320\262\320\265\320\264\320\265\320\275\320\270\320\270 */\n"
"   }\n"
"\n"
"   QMenuBar::item:pressed {\n"
"       background: #3498db; /* \320\246\320\262\320\265\321\202 \321\204\320\276\320\275\320\260 \320\277\321\200\320\270 \320\275\320\260\320\266\320\260\321\202\320\270\320\270 */\n"
"  "
                        " }\n"
"\n"
"   /* Central widget */\n"
"   QWidget#centralwidget {\n"
"       background: rgba(255, 255, 255, 0.9);\n"
"       border-radius: 15px;\n"
"       border: 2px solid #2c3e50;\n"
"   }\n"
"\n"
"\n"
"   /* Buttons style */\n"
"   QPushButton {\n"
"       background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                 stop:0 #3498db, stop:1 #2980b9);\n"
"       border: 1px solid #2980b9;\n"
"       color: white;\n"
"       font-weight: bold;\n"
"       border-radius: 5px;\n"
"       padding: 5px;\n"
"       min-width: 100px;\n"
"   }\n"
"\n"
"   QPushButton:hover {\n"
"       background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                 stop:0 #2980b9, stop:1 #3498db);\n"
"   }\n"
"\n"
"   QPushButton:pressed {\n"
"       background: #2c3e50;\n"
"   }\n"
"\n"
"   /* Special buttons */\n"
"   QPushButton#stopButton {\n"
"       background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                 stop:0 #e74c3c, stop:1 #c0392b);\n"
""
                        "       border: 1px solid #c0392b;\n"
"   }\n"
"\n"
"   QPushButton#stopButton:hover {\n"
"       background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                 stop:0 #c0392b, stop:1 #e74c3c);\n"
"   }\n"
"\n"
"   QPushButton#pushButton {\n"
"       background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                 stop:0 #f39c12, stop:1 #e67e22);\n"
"       border: 1px solid #e67e22;\n"
"   }\n"
"\n"
"   QPushButton#pushButton:hover {\n"
"       background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                 stop:0 #e67e22, stop:1 #f39c12);\n"
"   }\n"
"\n"
"/* List widget */\n"
"QListWidget {\n"
"    background: white;\n"
"    border: 2px solid #bdc3c7;\n"
"    border-radius: 10px;\n"
"    font-family: 'Consolas';\n"
"    font-size: 11px;\n"
"}\n"
"\n"
"QListWidget::item {\n"
"    padding: 5px;\n"
"    border-bottom: 1px solid #ecf0f1;\n"
"}\n"
"\n"
"QListWidget::item:selected {\n"
"    background: #3498db;\n"
"    color: white;\n"
"}\n"
""
                        "\n"
"   /* Header label */\n"
"   QLabel#headerLabel {\n"
"       font-size: 16px;\n"
"       font-weight: bold;\n"
"       color: #2c3e50;\n"
"       background: transparent;\n"
"       border: none;\n"
"   }\n"
"\n"
"   /* Scroll bars */\n"
"   QScrollBar:vertical {\n"
"       border: none;\n"
"       background: #ecf0f1;\n"
"       width: 10px;\n"
"       margin: 0px 0px 0px 0px;\n"
"   }\n"
"\n"
"   QScrollBar::handle:vertical {\n"
"       background: #bdc3c7;\n"
"       min-height: 20px;\n"
"       border-radius: 5px;\n"
"   }\n"
"\n"
"   QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"       height: 0px;\n"
"   }\n"
"   "));
        settingsAction = new QAction(MainWindow);
        settingsAction->setObjectName(QString::fromUtf8("settingsAction"));
        exitAction = new QAction(MainWindow);
        exitAction->setObjectName(QString::fromUtf8("exitAction"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setGeometry(QRect(20, 20, 860, 610));
        headerLabel = new QLabel(centralwidget);
        headerLabel->setObjectName(QString::fromUtf8("headerLabel"));
        headerLabel->setGeometry(QRect(20, 10, 300, 30));
        selectFolderButton = new QPushButton(centralwidget);
        selectFolderButton->setObjectName(QString::fromUtf8("selectFolderButton"));
        selectFolderButton->setGeometry(QRect(20, 60, 300, 30));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(360, 60, 180, 30));
        stopButton = new QPushButton(centralwidget);
        stopButton->setObjectName(QString::fromUtf8("stopButton"));
        stopButton->setGeometry(QRect(580, 60, 200, 30));
        listWidget = new QListWidget(centralwidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setGeometry(QRect(20, 110, 820, 470));
        statusLabel = new QLabel(centralwidget);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
        statusLabel->setGeometry(QRect(20, 590, 821, 20));
        statusLabel->setStyleSheet(QString::fromUtf8("color: #7f8c8d; font-style: italic;"));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 900, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menuFile->addAction(settingsAction);
        menuFile->addAction(exitAction);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Antivirus Monitor", nullptr));
        settingsAction->setText(QApplication::translate("MainWindow", "\342\232\231\357\270\217 \320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270", nullptr));
#ifndef QT_NO_SHORTCUT
        settingsAction->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_NO_SHORTCUT
        exitAction->setText(QApplication::translate("MainWindow", "\360\237\232\252 \320\222\321\213\321\205\320\276\320\264", nullptr));
        headerLabel->setText(QApplication::translate("MainWindow", "\360\237\224\215 \320\220\320\275\321\202\320\270\320\262\320\270\321\200\321\203\321\201\320\275\321\213\320\271 \320\274\320\276\320\275\320\270\321\202\320\276\321\200", nullptr));
        selectFolderButton->setText(QApplication::translate("MainWindow", "\360\237\223\201 \320\222\321\213\320\261\321\200\320\260\321\202\321\214 \320\277\320\260\320\277\320\272\321\203 \320\264\320\273\321\217 \320\274\320\276\320\275\320\270\321\202\320\276\321\200\320\270\320\275\320\263\320\260", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "\360\237\232\250 \320\232\320\260\321\200\320\260\320\275\321\202\320\270\320\275", nullptr));
        stopButton->setText(QApplication::translate("MainWindow", "\342\217\271 \320\236\321\201\321\202\320\260\320\275\320\276\320\262\320\270\321\202\321\214 \320\274\320\276\320\275\320\270\321\202\320\276\321\200\320\270\320\275\320\263", nullptr));
        statusLabel->setText(QApplication::translate("MainWindow", "\320\241\321\202\320\260\321\202\321\203\321\201: \320\223\320\276\321\202\320\276\320\262 \320\272 \321\200\320\260\320\261\320\276\321\202\320\265", nullptr));
        menuFile->setTitle(QApplication::translate("MainWindow", "\320\244\320\260\320\271\320\273", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
