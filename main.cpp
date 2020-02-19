#include <QApplication>
#include <iostream>
#include "opencv2/opencv.hpp"
#include "mainwindow.h"
#include "admin_dialog.h"
#include <time.h>
#include "account_dialog.h"

using namespace std;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    MainWindow w;
    w.show();



    return a.exec();
}
