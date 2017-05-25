#ifndef BASE_H
#define BASE_H

#include <QDebug>
#include <QThread>
#include <QMainWindow>
#include <QLabel>
#include <QWidget>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDialog>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QVector>

#include "cv.h"
#include "opencv/cv.hpp"
#include "opencv2/face.hpp"

using namespace std;
using namespace cv;
using namespace cv::face;

enum {
    IO_ERR,
    JSON_ERR,
    CAS_ERR,
    PCA_ERR,
    PCA_Algo,
    Fisher_ERR,
    Fisher_Algo,
    LBPH_ERR,
    LBPH_Algo,
    CAM_ERR,
    START,
    STOP,
    P,
    FINISH,
    ADD_PROCESS,
    RESET_PROCESS,
    FEW_IMAGES
};

struct MyImage {
    QImage image;
    QImage ui_image;
    int Face;
};

#endif // BASE_H
