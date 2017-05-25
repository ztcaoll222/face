#include "savephotothread.h"

SavePhotoThread::SavePhotoThread()
{
    basePath = QCoreApplication::applicationDirPath()+"/";
    name = "";
}

void SavePhotoThread::run()
{
    QString childDirName = basePath+"faces/"+QString::number(QDateTime::currentMSecsSinceEpoch(), 10)+"_"+name+"/"; // 时间戳_name
    QDir childDir;

    if (!childDir.mkdir(childDirName)) {
        emit postMsg(IO_ERR);
    }
    else {
        int i = 0;
        foreach (QImage t_image, photoList) {
            if (!t_image.save(childDirName+QString::number(i, 10)+".jpg")) {
                emit postMsg(IO_ERR);
            }
            ++i;
        }

        QFile nameFile(childDirName+"name");
        nameFile.open(QIODevice::WriteOnly | QIODevice::Text);
        nameFile.write(name.toLatin1());
        nameFile.close();
    }

    emit postProcess(photoList.length());
    emit postMsg(FINISH);

    name = "";
    photoList.clear();
}

void SavePhotoThread::getName(QString t_name)
{
    name = t_name;
}

void SavePhotoThread::getPhoto(QImage t_image)
{
    photoList.append(t_image);
}
