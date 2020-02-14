#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <opencv2/opencv.hpp>

namespace Ui {
class Dialog;

}
class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    void initialize(cv::Mat frame);
    ~Dialog();

private:
    Ui::Dialog *ui;
    cv::Mat frame;



signals:
    void profile();
private slots:
    void on_pushButton_clicked();
};

#endif // DIALOG_H
