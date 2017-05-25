#ifndef LEANRNTHREAD_H
#define LEANRNTHREAD_H

#include "base.h"

class LeanrnThread : public QThread
{
    Q_OBJECT

public:
    LeanrnThread();

protected:
    void run();

private:
    QString basePath;

    QVector<Mat> images;

    QVector<int> labels;

    int last;

    void makeJsonAt();

    void learn();

private slots:

signals:
    void postMsg(int);

    void postMax(int);

    void postProcess(int);
};

#endif // LEANRNTHREAD_H
