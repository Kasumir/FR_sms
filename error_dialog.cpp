#include "error_dialog.h"
#include "ui_error_dialog.h"

error_dialog::error_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::error_dialog)
{
    ui->setupUi(this);

}

error_dialog::~error_dialog()
{
    delete ui;
}

void error_dialog::setText(QString str){
    ui->lineEdit->setText(str);
}

