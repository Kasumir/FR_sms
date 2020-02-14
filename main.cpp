#include <QApplication>
#include <iostream>
#include "opencv2/opencv.hpp"
#include "mainwindow.h"

using namespace std;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Widget w;
    //w.initialize(cv::imread("/home/intel01/Desktop/asdf.jpg"));
    //w.show();

    MainWindow w;
    w.show();


    return a.exec();
}
