#include "dialog.h"
#include "ui_dialog.h"
#include <imageutil.h>

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
void Dialog::initialize(cv::Mat img){
    frame = img;
    cv::Mat tmp;
    cv::resize(img, tmp,  cv::Size(ui->label->width(), ui->label->height()));
    ui->label->setPixmap(imageutil::cvMatToQPixmap(tmp));
}

void Dialog::on_pushButton_clicked()
{

}
