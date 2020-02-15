#include "admin_dialog.h"
#include "ui_admin_dialog.h"
#include <cstdlib>

admin_dialog::admin_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::admin_dialog)
{
    ui->setupUi(this);

    db_.connect_db();


    add_rows();
}

admin_dialog::~admin_dialog()
{
    delete ui;
}

void admin_dialog::add_rows(){
    ui->tableWidget->clear();
    ckbox_list.clear();

    //set table size
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setRowCount(std::atoi(db_.size().c_str()));
    //set column header
    QStringList list;
    ui->tableWidget->setHorizontalHeaderLabels(list << "check" << "pic" << "name" << "phonenum" << "date" << "delete");

    //add rows
    auto p = db_.getData(-1, -1);
    for(int i = 0; i < p.size(); i++){
        //add checkbox
        QHBoxLayout * cblayout = new QHBoxLayout();
        QCheckBox * ckbox = new QCheckBox();
        cblayout->addWidget(ckbox);
        cblayout->setAlignment(Qt::AlignCenter);
        cblayout->setContentsMargins(0,0,0,0);
        QWidget * wid = new QWidget();
        wid->setLayout(cblayout);
        ui->tableWidget->setCellWidget(i,0, wid);
        ckbox_list.push_back(ckbox);
        //add Qlabel
        QLabel * label = new QLabel();
        label->resize(60, 80);
        cv::Mat frame;
        cv::resize(cv::imread(p[i][3].c_str(), cv::IMREAD_COLOR), frame, cv::Size(60,80));
        label->setPixmap(imageutil::cvMatToQPixmap(frame));
        ui->tableWidget->setCellWidget(i, 1, label);
        //text
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(p[i][1].c_str()));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(p[i][2].c_str()));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(p[i][4].c_str()));
    }

    //set table cell size
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();

    for(int i = 0; i < p.size(); i++){
        for(int j = 0; j < p[i].size(); j++){
            cout << p[i][j] << " ";
        }
        cout << endl;
    }
}

void admin_dialog::on_delete_btn_clicked()
{
    bool a = false;
    std::vector<QCheckBox*>::iterator iter;
    for(int i = 0; i < ckbox_list.size(); i++){
        iter = ckbox_list.begin();

        if((*(iter+i))->checkState()){
            db_.delData(i);
            ui->tableWidget->removeRow(i);
            ckbox_list.erase(iter+i);
            i--;
            a = true;
        }
    }
    add_rows();
    if(a)
        emit deleted();
}











































