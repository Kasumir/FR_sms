#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QtGlobal>
#include <vas/fd.h>
#include <vas/fr.h>
#include <vector>
#include <string>
#include <sms/coolsms.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void register_std(const char * image_path, int ID);
    int sendmessage(const char * ID, const char * PASSWD, const char * RECEIVER, const char * SENDER, const char * MSG);
    std::unique_ptr<vas::fd::FaceDetector> fd;
    std::unique_ptr<vas::fr::FaceRecognizer> fr;

private:
    Ui::MainWindow *ui;
    QTimer *timer30ms;
    cv::VideoCapture cap;
    cv::Mat frame;
    cv::Mat display;
    cv::Mat logo;
    cv::Mat logo_gray;

    vas::fd::Face face;
    vas::fd::FaceDetector::Builder fd_builder;
    vas::fr::FaceRecognizer::Builder fr_builder;
    std::vector<std::string> v_phone_num, v_image_path, v_date, v_YN;

    //std::vector<bool> issent;
    int width, height, logo_width, logo_height;

    char * BUILDER_PATH;

    void startDisplay();
    std::string getDate();
    std::vector<std::string> getAccountData();
private slots:
    void mainProcess();
    void on_pushButton_clicked();
    void add_std();
    void on_admin_btn_clicked();
    void reset_stdlist();
    void on_account_btn_clicked();
};

#endif // MAINWINDOW_H
