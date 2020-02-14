#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <database.h>
#include <QString>
#include "dialog.h"
#include <cstdlib>
#include <imageutil.h>

//Setup  MainWindow
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //initialize face_detector//
    BUILDER_PATH = "/home/intel01/intel_vas/vas/lib/intel64";

    fd_builder.backend_type = static_cast<vas::BackendType>(0);
    fr_builder.backend_type = static_cast<vas::BackendType>(0);
    fd_builder.max_num_faces = -1;
    fd_builder.min_face_ratio = 0.05;
    fd_builder.threshold = 0.5;
    fd_builder.max_frame_latency = 30;
    fd_builder.redetection_period = 10;

    fd = fd_builder.Build("/home/intel01/intel_vas/vas/lib/intel64");
    fr = fr_builder.Build("/home/intel01/intel_vas/vas/lib/intel64");

    db db_;
    db_.connect_db();
    v_image_path = db_.get_image_path();
    v_phone_num = db_.get_phone_num();

    for(int i = 0; i < v_phone_num.size(); i++)
        std::cout << v_phone_num[i] << std::endl;
    for(int i = 0; i < v_phone_num.size(); i++)
        issent.push_back(0);
    for(int i = 0; i < v_image_path.size(); i++)
        register_std(v_image_path[i].c_str(), i);

    cv::VideoCapture capture(0);
    cap = capture;
    if(!cap.isOpened()){
        printf("not allow cam::\n\n\n");
        return;
    }else
        printf("cam opened\n\n");
    //

    ui->setupUi(this);
    timer30ms = new QTimer(this);
    connect(timer30ms, SIGNAL(timeout()), this, SLOT(mainProcess()));
    timer30ms->start(1);
}


MainWindow::~MainWindow()
{
    delete ui;
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
    //printf("mainprocess()\n");
    if(width != ui->label->width()){
        //cap.set(cv::CAP_PROP_FRAME_WIDTH, ui->label->width());
        width = ui->label->width();
    }
    if(height != ui->label->height()){
        //cap.set(cv::CAP_PROP_FRAME_HEIGHT, ui->label->height());
        height = ui->label->height();
    }
    cv::Mat m;
    cap >> m;
    cv::resize(m, frame, cv::Size(ui->label->width(), ui->label->height()), 0, 0);
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

                cv::rectangle(frame, face.rect, cv::Scalar(0, 255, 0), 2);
                cv::putText(frame, text, cv::Point2i(face.rect.x, face.rect.y - 3), 0, 0.5, cv::Scalar(0, 255, 0, 255));
                //send message
                if(issent[fr_result.person_id] == 0){
                    std::cout << "detected user ID : " << fr_result.person_id << std::endl;
                    //sendmessage("rhfmrh1230", "qu9760cs", v_phone_num[fr_result.person_id].c_str(), "01041955413", "check!");
                    issent[fr_result.person_id] = 1;
                }
            }
            else{
                std::snprintf(text, sizeof(text), "Unknown(%.3f)", fr_result.similarity_score);

                cv::rectangle(frame, face.rect, cv::Scalar(255, 0, 0), 2);
                cv::putText(frame, text, cv::Point2i(face.rect.x, face.rect.y - 3), 0, 0.5, cv::Scalar(255, 0, 0, 255));
            }
        }
    }
    if (!frame.empty())
    {
        ui->label->setPixmap(imageutil::cvMatToQPixmap(frame));
    }
    else{
        printf("Fail Image\n");
    }
}

int MainWindow::sendmessage(char * ID, char * PASSWD, const char * RECEIVER, char * SENDER, char * MSG){
    coolsms::sms s;
    s.appversion("Example/1.0");
    s.charset("utf8");
    s.setuser(ID, PASSWD);
    // 받는번호, 보내는번호, 메시지내용, 참조용이름(생략가능), 예약일시(YYYYMMDDHHMISS 형식)(생략가능), Local mesasge-id(생략가능)
    s.addsms(RECEIVER, SENDER, MSG);
    // 서버에 연결합니다.
    if (!s.connect()) {
        std::cerr << "cannot connect to server" << std::endl;
        return 1;
    }
    std::cout << s.send() << " success." << std::endl;
    s.disconnect();
    if (s.errordetected()) {
        std::cout << "Error: " << s.lasterrorstr() << std::endl;
    }
    s.printr();
    s.empty();
    return 0;
}



//capture button clicked
void MainWindow::on_pushButton_clicked()
{
    printf("\n start Button\n");
    QString name, phonenum;
    //name = ui->textedit_name->toPlainText();
    //phonenum = ui->textedit_phonenum->toPlainText();
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
    //connect(&d, SIGNAL(profile()), this, SLOT(add_std()));
    d.initialize(capImage);
    d.setModal(true);
    d.exec();


    //printf("what???\n");
    timer30ms->start(1);
}

void MainWindow::add_std(){
    timer30ms->stop();
    db db_;
    db_.connect_db();
    v_image_path = db_.get_image_path();
    v_phone_num = db_.get_phone_num();


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
}
