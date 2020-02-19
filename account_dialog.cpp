#include "account_dialog.h"
#include "ui_account_dialog.h"

account_dialog::account_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::account_dialog)
{
    ui->setupUi(this);

    ifstream inFile("account.txt");
    for(int i = 0; i < 4 && !inFile.eof(); i++){
        inFile.getline(inputString, MAX_SIZE);
        inputs[i] = inputString;
    }

    ui->te_id->setPlainText(inputs[0]);
    ui->te_pw->setPlainText(inputs[1]);
    ui->te_phonenum->setPlainText(inputs[2]);
    ui->te_msg->setPlainText(inputs[3]);

    inFile.close();
}

account_dialog::~account_dialog()
{
    delete ui;
}

void account_dialog::on_pushButton_clicked()
{
    if(ui->te_id->toPlainText().length() > 0 && ui->te_msg->toPlainText().length() > 0 && ui->te_phonenum->toPlainText().length() > 0 && ui->te_pw->toPlainText().length() > 0){
        ofstream outFile("account.txt");
        outFile.clear();

        inputs[0] = ui->te_id->toPlainText();
        inputs[1] = ui->te_pw->toPlainText();
        inputs[2] = ui->te_phonenum->toPlainText();
        inputs[3] = ui->te_msg->toPlainText();

        for(int i = 0; i < TE_NUM; i++){
            outFile << inputs[i].toStdString().c_str() << endl;
        }

        outFile.close();

        this->close();
    }
    else{
        error_dialog er;
        er.setText("Please enter all item");
        er.setModal(true);
        er.exec();
    }
}
