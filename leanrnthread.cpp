#include "leanrnthread.h"

LeanrnThread::LeanrnThread()
{
    basePath = QCoreApplication::applicationDirPath()+"/";
}

void LeanrnThread::run()
{
    makeJsonAt();

    learn();

    emit postMsg(FINISH);
}

// 生成at.txt和json
void LeanrnThread::makeJsonAt()
{
    QDir baseDir = QDir(basePath+"faces/");

    QStringList baseDairList = baseDir.entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
    emit postMax(baseDairList.length());

    QString name = "";
    QFile *nameFile;

    QDir clildDir;
    QStringList clildDirList;
    QString fileName;

    QFile atFile(basePath+"faces/at.txt");
    if (!atFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit postMsg(IO_ERR);
        return;
    }

    QFile labelFile(basePath+"faces/label.json");
    if (!labelFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit postMsg(IO_ERR);
        return;
    }
    \
    int label = 1;
    QJsonObject dict;
    QJsonDocument document;

    foreach (QString t_baseDirList, baseDairList) {
        // 读取子目录下的name文件， 不存在或如果读取的到的文本是""则写入子目录的名称
        nameFile = new QFile(basePath+"faces/"+t_baseDirList+"/name");
        if (!nameFile->open(QIODevice::ReadWrite | QIODevice::Text)) {
            emit postMsg(IO_ERR);
        }

        name = nameFile->readAll();
        if (name.isEmpty()) {
            name = t_baseDirList;
            nameFile->write(name.toLatin1());
        }

        emit postProcess(1);

        // 读取子目录下的文件(除name文件), 写入at.txt, 虽然在这里没什么卵用
        clildDir = QDir(basePath+"faces/"+t_baseDirList+"/");
        clildDirList = clildDir.entryList(QDir::NoDotAndDotDot | QDir::Files);
        foreach (QString t_file, clildDirList) {
            if ("name" == t_file) {
                continue;
            }

            QString t_imagePath = QFileInfo(basePath+"faces/"+t_baseDirList+"/"+t_file).absoluteFilePath();
            fileName = t_imagePath+";"+QString::number(label, 10)+"\n";
            atFile.write(fileName.toLatin1());

            // 把图片读进images
            Mat t_image = imread(t_imagePath.toStdString(), IMREAD_GRAYSCALE);
            images.push_back(t_image);
            labels.push_back(label);
        }

        emit postProcess(1);

        // 写json, "label":"name"
        dict.insert(QString::number(label, 10), name);

        label++;
        nameFile->close();
        delete nameFile;
        emit postProcess(1);
    }
    document.setObject(dict);
    labelFile.write(document.toJson(QJsonDocument::Compact));

    labelFile.close();
    atFile.close();
}

// 训练
void LeanrnThread::learn()
{
    if (!labels.isEmpty()) {
        if (1 <= labels.last()) {
            Ptr<BasicFaceRecognizer> modelPCA = createEigenFaceRecognizer();
            modelPCA->train(images.toStdVector(), labels.toStdVector());
            modelPCA->save(QString(basePath+"recs/MyFacePCAModel.xml").toStdString());

            emit postProcess(20);
            emit postProcess(20);
            emit postProcess(20);

            emit postMsg(FEW_IMAGES);
        }
        else {
            Ptr<BasicFaceRecognizer> modelPCA = createEigenFaceRecognizer();
            modelPCA->train(images.toStdVector(), labels.toStdVector());
            modelPCA->save(QString(basePath+"recs/MyFacePCAModel.xml").toStdString());

            emit postProcess(20);

            Ptr<BasicFaceRecognizer> modelFisher = createFisherFaceRecognizer();
            modelFisher->train(images.toStdVector(), labels.toStdVector());
            modelFisher->save(QString(basePath+"recs/MyFaceFisherModel.xml").toStdString());

            emit postProcess(20);

            Ptr<LBPHFaceRecognizer> modelLBPH = createLBPHFaceRecognizer();
            modelLBPH->train(images.toStdVector(), labels.toStdVector());
            modelLBPH->save(QString(basePath+"recs/MyFaceLBPHModel.xml").toStdString());

            emit postProcess(20);
        }
    }
}
