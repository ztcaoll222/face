#include "camerathread.h"

CameraThread::CameraThread()
{
    Start = 0;
    faceLoad = 0;
    pcaLoad = 0;
    fisherLoad = 0;
    lbphLoad = 0;
    Algo = PCA_Algo;
    key = 0;
    Face = 0;

    closeImage.load(":/pic/pics/close.jpg");
    camera = closeImage;
    photo = closeImage;

    basePath = QCoreApplication::applicationDirPath()+"/";
}

void CameraThread::run()
{
    faceCascade = new CascadeClassifier;
    if (faceCascade->load(QString(basePath+"haar/haarcascade_frontalface_default.xml").toStdString())) {
        faceLoad = 1;
    }
    else {
        emit postMsg(CAS_ERR);
        faceLoad = 0;
    }
    emit postMsg(ADD_PROCESS);

    modelPCA = createEigenFaceRecognizer();
    try {
        modelPCA->load(QString(basePath+"recs/MyFacePCAModel.xml").toStdString());
        pcaLoad = 1;
    }
    catch(cv::Exception) {
        emit postMsg(PCA_ERR);
        pcaLoad = 0;
    }
    emit postMsg(ADD_PROCESS);

    modelFisher = createFisherFaceRecognizer();
    try {
        modelFisher->load(QString(basePath+"recs/MyFaceFisherModel.xml").toStdString());
        fisherLoad = 1;
    }
    catch(cv::Exception) {
        emit postMsg(Fisher_ERR);
        pcaLoad = 0;
    }
    emit postMsg(ADD_PROCESS);

    modelLBPH = createLBPHFaceRecognizer();
    try {
        modelLBPH->load(QString(basePath+"recs/MyFaceLBPHModel.xml").toStdString());
        lbphLoad = 1;
    }
    catch(cv::Exception) {
        emit postMsg(LBPH_ERR);
        lbphLoad = 0;
    }
    emit postMsg(ADD_PROCESS);

    emit postMsg(RESET_PROCESS);

    cap = VideoCapture(0);
    if (!cap.isOpened()) {
        emit postMsg(CAM_ERR);
    }
    else {
        while (Start) {
            waitKey(100);
            cap >> frame;

            if (faceLoad) {
                faces = handelCamera(frame, faceFrame);
                Face = faces.size();
            }

            if (key) {
                photo = UMat2QImage(faceFrame);
                emit postPhoto(photo, Face);
            }
            key = 0;

            if (Face) {
                frame = racFace(frame, faceFrame);
            }

            camera = UMat2QImage(frame);
            emit postCamera(camera);
        }
    }

    camera = closeImage;
    emit postCamera(camera);

    cap.release();
    modelPCA->clear();
    modelFisher->clear();
    modelLBPH->clear();
    delete faceCascade;
}

// 人脸识别
UMat CameraThread::racFace(UMat t_frame, UMat t_faceFrame)
{
    int pcaLabel = 0;
    int fisherLabel = 0;
    int lbphLabel = 0;

    int label = 0;

    if (pcaLoad) {
        pcaLabel = modelPCA->predict(t_faceFrame);
    }

    if (fisherLoad) {
        fisherLabel = modelFisher->predict(t_faceFrame);
    }

    if (lbphLoad) {
        lbphLabel = modelLBPH->predict(t_faceFrame);
    }

    if (PCA_Algo == Algo) {
        label = pcaLabel;
    }
    else if (Fisher_Algo == Algo) {
        label = fisherLabel;
    }
    else if (LBPH_Algo == Algo) {
        label = lbphLabel;
    }

    QString name = getNameByLabel(label);

    putText(t_frame, name.toStdString(), Point(faces[0].x, faces[0].y), FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0));

    return t_frame;
}

// 查询
QString CameraThread::getNameByLabel(int label)
{
    QByteArray val;
    QFile file;
    file.setFileName(basePath+"faces/label.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    QString trans = QString("%1").arg(label);

    QJsonObject b = QJsonObject(QJsonDocument::fromJson(val).object());
    return b.take(trans).toString();
}

// 人脸检测， 返回人脸数目
vector<Rect> CameraThread::handelCamera(UMat &t_frame, UMat &t_faceFrame)
{
    vector<Rect> t_faces;

    cvtColor(t_frame, t_faceFrame, COLOR_BGR2GRAY);
    equalizeHist(t_faceFrame, t_faceFrame);

    faceCascade->detectMultiScale(t_faceFrame, t_faces, 1.1, 4, CV_HAAR_DO_ROUGH_SEARCH, Size(100, 100),Size(500, 500));

    if (t_faces.size()) {
        Point pt1;
        pt1.x = t_faces[0].x;
        pt1.y = t_faces[0].y;

        Point pt2;
        pt2.x = pt1.x+t_faces[0].width;
        pt2.y = pt1.y+t_faces[0].height;

        rectangle(t_frame, pt1, pt2, Scalar(255, 0, 0), 2, 8, 0);

        t_faceFrame = t_faceFrame(t_faces[0]);
        resize(t_faceFrame, t_faceFrame, Size(92, 112));
    }

    return t_faces;
}

// UMat转QImage， 如果不是RGB1通道或RGB3通道， 返回前一帧
QImage CameraThread::UMat2QImage(UMat t_frame)
{
    QImage t_image;

    if(CV_8UC1 == t_frame.type()) {
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++) {
            colorTable.push_back(qRgb(i,i,i));
        }

        t_image = QImage((const uchar*)t_frame.getMat(ACCESS_FAST).data, t_frame.cols, t_frame.rows, t_frame.step, QImage::Format_Indexed8);
        t_image.setColorTable(colorTable);
        return t_image;
    }
    else if(CV_8UC3 == t_frame.type()) {
        t_image = QImage((const uchar*)t_frame.getMat(ACCESS_FAST).data, t_frame.cols, t_frame.rows, t_frame.step, QImage::Format_RGB888);
        t_image = t_image.rgbSwapped();
        return t_image;
    }

    return camera;
}

void CameraThread::getMsg(int msg)
{
    if (START == msg) {
        Start = 1;
    }
    else if (STOP == msg) {
        Start = 0;
    }
    else if (P == msg) {
        key = 1;
    }
    else if (PCA_Algo == msg) {
        Algo = PCA_Algo;
    }
    else if (Fisher_Algo == msg) {
        Algo = Fisher_Algo;
    }
    else if (LBPH_Algo == msg) {
        Algo = LBPH_Algo;
    }
}
