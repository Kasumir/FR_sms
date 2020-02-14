#include "dialog.h"
#include "ui_dialog.h"
#include <imageutil.h>
#include <error_dialog.h>
#include <cstring>
#include <database.h>


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}
/*

 */
void Dialog::initialize(cv::Mat img){
    frame = img;
    cv::Mat tmp;
    cv::resize(img, tmp,  cv::Size(ui->label->width(), ui->label->height()));
    ui->label->setPixmap(imageutil::cvMatToQPixmap(tmp));
}

void Dialog::on_pushButton_clicked()
{
    if(ui->te_name->toPlainText().length() == 0){
        error_dialog e;
        e.setText("input name");
        e.setModal(true);
        e.exec();
        return;
    }else if(ui->te_phone->toPlainText().length() == 0){
        error_dialog e;
        e.setText("input phone number");
        e.setModal(true);
        e.exec();
        return;
    }

    std::string str = "/home/intel01/build-FRsms-Desktop_Qt_5_11_2_GCC_64bit-Debug/std_image/" +  ui->te_name->toPlainText().toStdString() + ".jpg";
    cv::imwrite(str, frame);

    db db_;
    db_.connect_db();
    db_.create_profile(ui->te_name->toPlainText().toStdString(), ui->te_phone->toPlainText().toStdString(), "./std_image/" + ui->te_name->toPlainText().toStdString() + ".jpg");

    emit profile();

    this->close();

}
