#ifndef ACCOUNT_DIALOG_H
#define ACCOUNT_DIALOG_H

#include <QDialog>
#include <fstream>
#include <string>
#include <iostream>
#include <QString>
#include <error_dialog.h>

#define MAX_SIZE 80
#define TE_NUM 4

using namespace std;

namespace Ui {
class account_dialog;
}

class account_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit account_dialog(QWidget *parent = nullptr);
    ~account_dialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::account_dialog *ui;
    char inputString[MAX_SIZE];
    QString inputs[TE_NUM];
};

#endif // ACCOUNT_DIALOG_H
