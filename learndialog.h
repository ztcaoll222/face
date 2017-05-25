#ifndef LEARNDIALOG_H
#define LEARNDIALOG_H

#include "base.h"
#include "leanrnthread.h"

namespace Ui {
class LearnDialog;
}

class LearnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LearnDialog(QWidget *parent = 0);
    ~LearnDialog();

    void start();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::LearnDialog *ui;

    LeanrnThread *learnThread;

    void stop();

private slots:
    void getMsg(int);

    void getMax(int);

    void getProcess(int);

signals:
    void postMsg(int);

};

#endif // LEARNDIALOG_H
