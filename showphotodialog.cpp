#include "showphotodialog.h"
#include "ui_showphotodialog.h"

ShowPhotoDialog::ShowPhotoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowPhotoDialog)
{
    ui->setupUi(this);
}

ShowPhotoDialog::~ShowPhotoDialog()
{
    delete ui;
}

void ShowPhotoDialog::setPhoto(QPixmap t_image)
{
    ui->label->setPixmap(t_image);
}
