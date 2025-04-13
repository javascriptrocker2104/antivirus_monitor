#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // Создание объекта, который управляет основными ресурсами приложения
    QApplication a(argc, argv);
    // Создание объекта главного окна приложения
    MainWindow w;
    // Отображение главного окна на экране
    w.show();
    // Запуск основного цикла обработки событий приложения
    return a.exec();
}
