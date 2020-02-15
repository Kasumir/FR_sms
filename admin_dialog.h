#ifndef ADMIN_DIALOG_H
#define ADMIN_DIALOG_H

#include <QDialog>
#include <QStringList>
#include <QLabel>
#include <opencv2/opencv.hpp>
#include <imageutil.h>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QtCore/Qt>
#include <QHBoxLayout>
#include <QWidget>
#include <list>
#include "database.h"


namespace Ui {
class admin_dialog;
}

class admin_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit admin_dialog(QWidget *parent = nullptr);
    ~admin_dialog();

private slots:
    void on_delete_btn_clicked();



private:
    void add_rows();

    Ui::admin_dialog *ui;
    std::vector<QCheckBox*> ckbox_list;
    db db_;

signals:
    void deleted();
};

#endif // ADMIN_DIALOG_H
