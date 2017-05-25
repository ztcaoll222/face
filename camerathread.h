#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include "base.h"

class CameraThread : public QThread
{
    Q_OBJECT

public:
    CameraThread();

protected:
    void run();

private:
    int Start;

    int key;

    int faceLoad;

    int pcaLoad;

    int fisherLoad;

    int lbphLoad;

    int Face;

    int Algo;

    QImage closeImage;

    QString basePath;

    CascadeClassifier *faceCascade;

    VideoCapture cap;

    UMat frame;

    UMat faceFrame;

    vector<Rect> faces;

    QImage camera;

    QImage photo;

    Ptr<BasicFaceRecognizer> modelPCA;

    Ptr<BasicFaceRecognizer> modelFisher;

    Ptr<LBPHFaceRecognizer> modelLBPH;

    vector<Rect> handelCamera(UMat &, UMat &);

    UMat racFace(UMat, UMat);

    QImage UMat2QImage(UMat);

    QString getNameByLabel(int);

private slots:
    void getMsg(int);

signals:
    void postMsg(int);

    void postCamera(QImage);

    void postPhoto(QImage, int);
};

#endif // CAMERATHREAD_H
