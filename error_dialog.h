#ifndef ERROR_DIALOG_H
#define ERROR_DIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class error_dialog;
}

class error_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit error_dialog(QWidget *parent = nullptr);
    void setText(QString str);
    ~error_dialog();

private:
    Ui::error_dialog *ui;
};

#endif // ERROR_DIALOG_H
