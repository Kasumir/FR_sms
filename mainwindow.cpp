#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "error_dialog.h"
#include <QThread>
#include <database.h>
#include <QString>
#include "dialog.h"
#include "account_dialog.h"
#include <cstdlib>
#include <imageutil.h>
#include <admin_dialog.h>
#include <time.h>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <fstream>

using namespace cv;
//Setup  MainWindow
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //initialize face_detector//
    BUILDER_PATH = "./vas/lib/intel64";

    fd_builder.backend_type = static_cast<vas::BackendType>(0);
    fr_builder.backend_type = static_cast<vas::BackendType>(0);
    fd_builder.max_num_faces = -1;
    fd_builder.min_face_ratio = 0.05;
    fd_builder.threshold = 0.5;
    fd_builder.max_frame_latency = 30;
    fd_builder.redetection_period = 10;

    reset_stdlist();

    cv::VideoCapture capture(0);
    cap = capture;
    if(!cap.isOpened()){
        printf("not allow cam::\n\n\n");
        return;
    }else
        printf("cam opened\n\n");
    //
    logo = cv::imread("./src/logo.jpg");
    logo_gray = cv::imread("./src/logo.jpg", IMREAD_GRAYSCALE);
    cv::resize(logo, logo, cv::Size(300, 100), 0, 0);
    cv::resize(logo_gray, logo_gray, cv::Size(300, 100), 0, 0);

    ui->setupUi(this);
    timer30ms = new QTimer(this);
    connect(timer30ms, SIGNAL(timeout()), this, SLOT(mainProcess()));
    timer30ms->start(1);
}



MainWindow::~MainWindow()
{
    delete ui;
}

string MainWindow::getDate(){
    struct tm* datetime;
    time_t t;
    t = time(NULL);
    datetime = localtime(&t);
    char str[100];
    sprintf(str, "%d-%02d-%02d", datetime->tm_year + 1900, datetime->tm_mon + 1, datetime->tm_mday);
    std::string strr = str;

    return strr;
}

void MainWindow::reset_stdlist(){
    fd = fd_builder.Build(BUILDER_PATH);
    fr = fr_builder.Build(BUILDER_PATH);

    db db_;
    db_.connect_db();
    v_image_path.clear();
    v_phone_num.clear();
    v_date.clear();
    v_YN.clear();
    auto p = db_.getData(-1, -1);



    for(int i = 0; i < p.size(); i++){
        v_image_path.push_back(p[i][3]);
        v_phone_num.push_back(p[i][2]);
        v_date.push_back(p[i][4]);
        v_YN.push_back(p[i][5]);
    }

    for(int i = 0; i < v_phone_num.size(); i++)
        std::cout << v_phone_num[i] << std::endl;
    for(int i = 0; i < v_image_path.size(); i++)
        register_std(v_image_path[i].c_str(), i);
}

void MainWindow::register_std(const char * image_path, int ID){
    cv::Mat src = imread(image_path, cv::IMREAD_COLOR);
    auto faces = fd->Detect(src, false);

    if(faces.size() == 0){
        std::cout << "No face is detected in register_image." << std::endl;
        return;
    }

    vas::fr::DetectedFace input_face(faces[0].left_eye, faces[0].right_eye, 0);
    auto register_feature = fr->ExtractFeature(src, input_face);

    auto register_result = fr->Register(register_feature.get(), ID);
    if(register_result.first != vas::fr::RegistrationStatus::SUCCESS){
        std::cout << "Registration fails: " << int32_t(register_result.first) << std::endl;
        return;
    }
}

void MainWindow::mainProcess()
{
    if(width != ui->label->width()){
        width = ui->label->width();
    }
    if(height != ui->label->height()){
        height = ui->label->height();
    }
    cv::Mat m;
    cap >> m;
    cv::resize(m, frame, cv::Size(ui->label->width(), ui->label->height()), 0, 0);
    cv::resize(m, display, cv::Size(ui->label->width(), ui->label->height()), 0, 0);



    cv::Mat imageROI(display, cv::Rect(frame.cols - logo.cols, frame.rows- logo.rows, logo.cols, logo.rows));
    cv::Mat mask(155 - logo_gray);
    logo.copyTo(imageROI, mask);

    auto faces = fd->Detect(frame, true);

    if(faces.size() > 0){
        std::vector<vas::fr::DetectedFace> detected_faces;
        for(const auto& face : faces){
            detected_faces.emplace_back(face.left_eye, face.right_eye, face.tracking_id);
        }
        auto recognized_result = fr->Recognize(frame, detected_faces, true);
        /////////////////////
        // Display Process //
        /////////////////////
        char text[256] = { 0, };

        for(int32_t i = 0; i < faces.size(); i++){
            face = faces[i];
            auto fr_result = recognized_result[i];

            //Draw FD's rectangle & matched or not
            if (fr_result.status == vas::fr::RecognitionStatus::SUCCESS){
                std::snprintf(text, sizeof(text), "Matched(%.3f) %d", fr_result.similarity_score, fr_result.person_id);

                cv::rectangle(display, face.rect, cv::Scalar(0, 255, 0), 2);
                cv::putText(display, text, cv::Point2i(face.rect.x, face.rect.y - 3), 0, 0.5, cv::Scalar(0, 255, 0, 255));


                //send message
                if(v_date[fr_result.person_id] != getDate() && std::atoi(v_YN[fr_result.person_id].c_str()) > 0){
                    //date
                    std::cout << "detected user ID : " << fr_result.person_id << std::endl;
                    auto data = getAccountData();

                    if(data[0] != "default" && sendmessage(data[0].c_str(), data[1].c_str(), v_phone_num[fr_result.person_id].c_str(), data[2].c_str(), data[3].c_str())){
                        db db_;
                        db_.connect_db();
                        db_.updateData(fr_result.person_id, 4, getDate());
                        //cout << "sendmessage!!!!!!!" << endl;
                        v_date[fr_result.person_id] = getDate();
                    }
                }
            }
            else{
                std::snprintf(text, sizeof(text), "Unknown(%.3f)", fr_result.similarity_score);

                cv::rectangle(display, face.rect, cv::Scalar(255, 0, 0), 2);
                cv::putText(display, text, cv::Point2i(face.rect.x, face.rect.y - 3), 0, 0.5, cv::Scalar(255, 0, 0, 255));
            }
        }
    }
    if (!display.empty())
    {
        ui->label->setPixmap(imageutil::cvMatToQPixmap(display));
    }
    else{
        printf("Fail Image\n");
    }
}
std::vector<std::string> MainWindow::getAccountData(){
    char inputString[100];
    vector<string> v(4);

    ifstream inFile("account.txt");
    for(int i = 0; i < 4 && !inFile.eof(); i++){
        inFile.getline(inputString, MAX_SIZE);
        v[i] = inputString;
    }
    inFile.close();

    return v;
}

int MainWindow::sendmessage(const char * ID, const char * PASSWD, const char * RECEIVER, const char * SENDER, const char * MSG){
    coolsms::sms s;
    s.appversion("Example/1.0");
    s.charset("utf8");
    s.setuser(ID, PASSWD);
    // 받는번호, 보내는번호, 메시지내용, 참조용이름(생략가능), 예약일시(YYYYMMDDHHMISS 형식)(생략가능), Local mesasge-id(생략가능)
    s.addsms(RECEIVER, SENDER, MSG);
    // 서버에 연결합니다.
    if (!s.connect()) {
        std::cerr << "cannot connect to server" << std::endl;
        return 0;
    }
    std::cout << s.send() << " success." << std::endl;
    if(s.send() == 0){
        auto v = getAccountData();
        ofstream outFile("account.txt");
        outFile.clear();
        for(int i = 0; i < 3; i++){
            outFile << "default" << endl;
        }
        outFile << v[3].c_str() << endl;
        outFile.close();
        s.disconnect();
        s.printr();
        s.empty();

        error_dialog er;
        er.setText("Failed to send message. Check your account infomation.");
        er.setModal(true);
        er.exec();
        return 0;
    }
    s.disconnect();
    if (s.errordetected()) {
        std::cout << "Error: " << s.lasterrorstr() << std::endl;

        return 0;
    }
    s.printr();
    s.empty();
    return 1;
}



//capture button clicked
void MainWindow::on_pushButton_clicked()
{
    QString name, phonenum;
    timer30ms->stop();  
    if(!(0 <= face.rect.x && 0 <= face.rect.width && face.rect.x + face.rect.width <= frame.cols && 0 <= face.rect.y && 0 <= face.rect.height && face.rect.y + face.rect.height <= frame.rows)){
        timer30ms->start(1);
        return;
    }
    if(face.rect.empty()){
        timer30ms->start(1);
        return;
    }
    cv::Mat capImage = frame(face.rect);
    Dialog d;
    connect(&d, SIGNAL(profile()), this, SLOT(add_std()));
    d.initialize(capImage);
    d.setModal(true);
    d.exec();

    timer30ms->start(1);
}

void MainWindow::add_std(){
    timer30ms->stop();
    db db_;
    db_.connect_db();
    v_image_path.clear();
    v_phone_num.clear();
    v_date.clear();
    v_YN.clear();

    auto p = db_.getData(-1, -1);

    for(int i = 0; i < p.size(); i++){
        v_image_path.push_back(p[i][3]);
        v_phone_num.push_back(p[i][2]);
        v_date.push_back(p[i][4]);
        v_YN.push_back(p[i][5]);
    }

    cv::Mat src = imread(v_image_path[v_image_path.size() - 1], cv::IMREAD_COLOR);
    auto faces = fd->Detect(src, false);

    if(faces.size() == 0){
        std::cout << "No face is detected in register_image." << std::endl;
        timer30ms->start(1);
        return;
    }

    vas::fr::DetectedFace input_face(faces[0].left_eye, faces[0].right_eye, 0);
    auto register_feature = fr->ExtractFeature(src, input_face);
    auto register_result = fr->Register(register_feature.get(), std::atoi(db_.size().c_str()) - 1);
    if(register_result.first != vas::fr::RegistrationStatus::SUCCESS){
        std::cout << "Registration fails: " << int32_t(register_result.first) << std::endl;
        timer30ms->start(1);
        return;
    }

    timer30ms->start(1);
    return;

}

void MainWindow::on_admin_btn_clicked()
{
    admin_dialog ad;
    connect(&ad, SIGNAL(deleted()), this, SLOT(reset_stdlist()));
    ad.setModal(true);
    ad.exec();
}

void MainWindow::on_account_btn_clicked()
{
    timer30ms->stop();
    account_dialog ac;
    ac.setModal(true);
    ac.exec();
    timer30ms->start(1);
}
