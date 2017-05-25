#include "learndialog.h"
#include "ui_learndialog.h"

LearnDialog::LearnDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LearnDialog)
{
    ui->setupUi(this);
}

LearnDialog::~LearnDialog()
{
    delete ui;
}

void LearnDialog::start()
{
    ui->label_status->setText("Please wait...");

    learnThread = new LeanrnThread;
    connect(learnThread, SIGNAL(postMsg(int)), this, SLOT(getMsg(int)));
    connect(learnThread, SIGNAL(postMax(int)), this, SLOT(getMax(int)));
    connect(learnThread, SIGNAL(postProcess(int)), this, SLOT(getProcess(int)));

    learnThread->start();
}

void LearnDialog::stop()
{
    disconnect(learnThread, SIGNAL(postMsg(int)), this, SLOT(getMsg(int)));
    disconnect(learnThread, SIGNAL(postMax(int)), this, SLOT(getMax(int)));
    disconnect(learnThread, SIGNAL(postProcess(int)), this, SLOT(getProcess(int)));

    learnThread->terminate();

    delete learnThread;
}

void LearnDialog::closeEvent(QCloseEvent *)
{
    stop();
}

void LearnDialog::getMsg(int msg)
{
    if (IO_ERR == msg) {
        // 遇到了无法解决的崩溃bug， 暂时就这么将就用着吧（虽然大概以后也不会去改了）
        ui->label_status->setText("Error!\nIO ERROR! Please try again");
    }
    else if (FINISH == msg) {
        ui->label_status->setText("Finish!");

        QMessageBox::warning(this, "Success!", "Learn finish!", QMessageBox::Yes);
        learnThread->wait();

        this->close();
    }
    else if (FEW_IMAGES == msg) {
        // 同上
        ui->label_status->setText("Error!\nYou needs at least 2 images to learn.\nPlease add more images to your data set!");
    }
}

void LearnDialog::getMax(int max)
{
    ui->progressBar->setMaximum(max*3+20+20+20);
}

void LearnDialog::getProcess(int i)
{
    ui->progressBar->setValue(ui->progressBar->value()+i);
}
