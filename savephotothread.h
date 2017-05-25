#ifndef SAVEPHOTOTHREAD_H
#define SAVEPHOTOTHREAD_H

#include "base.h"

class SavePhotoThread : public QThread
{
    Q_OBJECT

public:
    SavePhotoThread();

protected:
    void run();

private:
    QString basePath;

    QString name;

    QList<QImage> photoList;

private slots:
    void getName(QString);

    void getPhoto(QImage);

signals:
    void postMsg(int);

    void postProcess(int);
};

#endif // SAVEPHOTOTHREAD_H
