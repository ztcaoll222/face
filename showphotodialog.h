#ifndef SHOWPHOTODIALOG_H
#define SHOWPHOTODIALOG_H

#include "base.h"

namespace Ui {
class ShowPhotoDialog;
}

class ShowPhotoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShowPhotoDialog(QWidget *parent = 0);
    ~ShowPhotoDialog();

    void setPhoto(QPixmap);

private:
    Ui::ShowPhotoDialog *ui;
};

#endif // SHOWPHOTODIALOG_H
