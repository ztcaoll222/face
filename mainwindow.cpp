#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Start = 0;

    name = "";

    Algo = PCA_Algo;

    key = 0;

    labelList.append(ui->label_1);
    labelList.append(ui->label_2);
    labelList.append(ui->label_3);
    labelList.append(ui->label_4);
    labelList.append(ui->label_5);
    labelList.append(ui->label_6);
    labelList.append(ui->label_7);
    labelList.append(ui->label_8);
    labelList.append(ui->label_9);
    labelList.append(ui->label_10);

    connect(this, SIGNAL(postCameraThreadMsg(int)), &camerathread, SLOT(getMsg(int)));
    connect(&camerathread, SIGNAL(postMsg(int)), this, SLOT(getCameraThreadMsg(int)));

    connect(&camerathread, SIGNAL(postCamera(QImage)), this, SLOT(getCamera(QImage)));
    connect(&camerathread, SIGNAL(postPhoto(QImage, int)), this, SLOT(getPhoto(QImage, int)));

    connect(&savePhotoThread, SIGNAL(postMsg(int)), this, SLOT(getSavePhotoThreadMsg(int)));

    connect(&savePhotoThread, SIGNAL(postProcess(int)), this, SLOT(getProcess(int)));

    connect(this, SIGNAL(postName(QString)), &savePhotoThread, SLOT(getName(QString)));

    connect(this, SIGNAL(postPhoto(QImage)), &savePhotoThread, SLOT(getPhoto(QImage)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 按键事件
void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (Qt::Key_Escape == e->key()) {
        startCameraThread(STOP);
        QApplication::exit();
    }
    else if (Qt::Key_S == e->key()) {
        if (Start) {
            startCameraThread(STOP);
        }
        else {
            startCameraThread(START);
        }
    }
    else if (Qt::Key_P == e->key()) {
        emit postCameraThreadMsg(P);
    }
    else if (Qt::Key_Backspace == e->key()) {
        removePhoto(-1);
    }
    else if (Qt::Key_Z == e->key()) {
        ++key;
        if (5 == key) {
            QMessageBox::warning(this, "Error!", "ztcaoll222!", QMessageBox::Yes);
            key = 0;
        }
    }
}

// 关闭主窗口事件
void MainWindow::closeEvent(QCloseEvent *)
{
    startCameraThread(STOP);
}

/**
 * 开始摄像头线程
 *
 * START：开始
 * STOP： 停止
 * */
void MainWindow::startCameraThread(int cmd)
{
    ui->progressBar->setMaximum(4);
    ui->progressBar->setValue(0);

    if (START == cmd) {
        emit postCameraThreadMsg(START);
        camerathread.start();

        Start = 1;
        ui->pushButton_start->setText("Stop");
        ui->pushButton_p->setEnabled(true);
    }
    else if (STOP == cmd) {
        emit postCameraThreadMsg(STOP);
        camerathread.wait();

        Start = 0;
        ui->pushButton_start->setText("Start");
        ui->pushButton_p->setEnabled(false);
    }
}

// 处理照片数组增删事件
void MainWindow::handelLabelImage()
{
    int i = 0;
    foreach (MyImage t_image, photoList) {
        labelList[i]->setPixmap(QPixmap::fromImage(t_image.ui_image));
        i++;
    }

    for (; i<labelList.length(); ++i) {
        labelList[i]->setPixmap(QPixmap(":/pic/pics/close.jpg"));
    }

    ui->progressBar->setMaximum(photoList.length());
    handelNameChange();
}

// 处理人名改变事件
void MainWindow::handelNameChange()
{
    if (photoList.length() && !ui->lineEdit_name->text().isEmpty()) {
        ui->pushButton_save->setEnabled(true);
    }
    else {
        ui->pushButton_save->setEnabled(false);
    }
}

/**
 * 处理摄像头线程发来的消息
 *
 * CAS_ERR： 无法加载分离器
 * CAM_ERR： 无法打开摄像头
 * PCA_ERR： 无法加载MyFacePCAModel.xml
 * */
void MainWindow::getCameraThreadMsg(int msg)
{
    if (CAS_ERR == msg) {
        QMessageBox::warning(this, "Error!", "Can not load haarcascade_frontalface_default.xml!", QMessageBox::Yes);
    }
    else if (CAM_ERR == msg) {
        QMessageBox::warning(this, "Error!", "Can not open camera!", QMessageBox::Yes);
        startCameraThread(STOP);
    }
    else if (PCA_ERR == msg) {
        QMessageBox::warning(this, "Error!", "Can not load MyFacePCAModel.xml!", QMessageBox::Yes);
    }
    else if (Fisher_ERR == msg) {
        QMessageBox::warning(this, "Error!", "Can not load MyFaceFisherModel.xml!", QMessageBox::Yes);
    }
    else if (LBPH_ERR == msg) {
        QMessageBox::warning(this, "Error!", "Can not load MyFaceLBPHModel.xml!", QMessageBox::Yes);
    }
    else if (JSON_ERR == msg) {
        QMessageBox::warning(this, "Error!", "Can not load label.json!", QMessageBox::Yes);
    }
    else if (ADD_PROCESS == msg) {
        ui->progressBar->setValue(ui->progressBar->value()+1);
    }
    else if (RESET_PROCESS == msg) {
        ui->progressBar->setMaximum(0);
        ui->progressBar->setValue(0);
    }
}

/**
 * 处理save线程发来的消息
 *
 * IO_ERR：IO错误
 * FINISH： 保存所有图片成功
 * */
void MainWindow::getSavePhotoThreadMsg(int msg)
{
    if (IO_ERR == msg) {
        QMessageBox::warning(this, "Error!", "IO ERROR! Please try again", QMessageBox::Yes);
    }
    else if (FINISH == msg) {
        QMessageBox::warning(this, "Success!", "save photo finish!", QMessageBox::Yes);
        removePhoto(-2);

        ui->progressBar->setValue(0);
        ui->progressBar->setMaximum(0);

        ui->lineEdit_name->clear();
    }
}

// 获得摄像头图像
void MainWindow::getCamera(QImage t_image)
{
    ui->label_camera->setPixmap(QPixmap::fromImage(t_image));
}

// 获得照片,  如果检测出人脸， 则画O， 否则画X
void MainWindow::getPhoto(QImage t_image, int Face)
{
    UMat frame;
    MyImage myImage;

    myImage.image = t_image;

    myImage.ui_image = t_image.copy();
    frame = QImage2UMat(myImage.ui_image);
    if (Face) {
        Point pt;
        pt.x = 75;
        pt.y = 95;
        circle(frame, pt, 7, Scalar(255, 0, 0), 2, 8, 0);
    }
    else {
        Point pt1;
        pt1.x = frame.rows-frame.rows/90;
        pt1.y = frame.cols-frame.cols/2;

        Point pt2;
        pt2.x = pt1.x+90;
        pt2.y = pt1.y+90;
        line(frame, pt1, pt2, Scalar(255, 0, 0), 20, 8, 0);

        pt1.x = pt1.x+90;
        pt1.y = pt1.y;

        pt2.x = pt2.x-90;
        pt2.y = pt2.y;
        line(frame, pt1, pt2, Scalar(255, 0, 0), 20, 8, 0);
    }
    myImage.ui_image = UMat2QImage(frame);

    myImage.Face = Face;

    if (10 > photoList.length()) {
        photoList.append(myImage);

        handelLabelImage();
    }
}

QImage MainWindow::UMat2QImage(UMat t_frame)
{
    QImage t_image;

    if(CV_8UC1 == t_frame.type()) {
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++) {
            colorTable.push_back(qRgb(i,i,i));
        }

        t_image = QImage((const uchar*)t_frame.getMat(ACCESS_FAST).data, t_frame.cols, t_frame.rows, t_frame.step, QImage::Format_Indexed8);
        t_image.setColorTable(colorTable);
    }
    else if(CV_8UC3 == t_frame.type()) {
        t_image = QImage((const uchar*)t_frame.getMat(ACCESS_FAST).data, t_frame.cols, t_frame.rows, t_frame.step, QImage::Format_RGB888);
        t_image = t_image.rgbSwapped();
    }

    return t_image;
}

UMat MainWindow::QImage2UMat(QImage t_image)
{
    UMat t_frame;

    if (QImage::Format_Indexed8 == t_image.format()) {
        t_frame = Mat(t_image.height(), t_image.width(), CV_8UC1, (void*)t_image.constBits(), t_image.bytesPerLine()).getUMat(ACCESS_FAST);
    }
    else if (QImage::Format_RGB888 == t_image.format()) {
        t_frame = Mat(t_image.height(), t_image.width(), CV_8UC3, (void*)t_image.constBits(), t_image.bytesPerLine()).getUMat(ACCESS_FAST);
        cvtColor(t_frame, t_frame, CV_BGR2RGB);
    }

    return t_frame;
}

// 获得进度
void MainWindow::getProcess(int i)
{
    ui->progressBar->setValue(ui->progressBar->value()+i);
}

// 按下"start"按钮时
void MainWindow::on_pushButton_start_clicked()
{
    if (Start) {
        startCameraThread(STOP);
    }
    else {
        startCameraThread(START);
    }
}

// 按下"p"按钮时
void MainWindow::on_pushButton_p_clicked()
{
    emit postCameraThreadMsg(P);
}

// 按下save按键时， 发送人名和已拍照片到save线程
void MainWindow::on_pushButton_save_clicked()
{
    emit postName(ui->lineEdit_name->text());

    foreach (MyImage t_image, photoList) {
        emit postPhoto(t_image.image);
    }

    savePhotoThread.start();
}

void MainWindow::on_pushButton_learn_clicked()
{
    learnDialog = new LearnDialog;
    learnDialog->setAttribute(Qt::WA_DeleteOnClose);
    learnDialog->setModal(true);

    learnDialog->show();
    learnDialog->start();
}

// 人名改变时
void MainWindow::on_lineEdit_name_textChanged(const QString &)
{
    handelNameChange();
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    if ("PCA" == arg1) {
        emit postCameraThreadMsg(PCA_Algo);
    }
    else if ("Fisher" == arg1) {
        emit postCameraThreadMsg(Fisher_Algo);
    }
    else if ("LBPH" == arg1) {
        emit postCameraThreadMsg(LBPH_Algo);
    }
}

// 删除照片， -1删除末尾， -2删除全部， 0-9删除某张照片
void MainWindow::removePhoto(int i)
{
    if (-1 == i) {
        if (0 < photoList.length()) {
            photoList.removeLast();
            handelLabelImage();
        }
    }
    else if (-2 == i) {
        photoList.clear();
        handelLabelImage();
    }
    else if (i>=0 && 9>=i) {
        photoList.removeAt(i);
        handelLabelImage();
    }
}

void MainWindow::on_pushButton_1x_clicked()
{
    removePhoto(0);
}

void MainWindow::on_pushButton_2x_clicked()
{
    removePhoto(1);
}

void MainWindow::on_pushButton_3x_clicked()
{
    removePhoto(2);
}

void MainWindow::on_pushButton_4x_clicked()
{
    removePhoto(3);
}

void MainWindow::on_pushButton_5x_clicked()
{
    removePhoto(4);
}

void MainWindow::on_pushButton_6x_clicked()
{
    removePhoto(5);
}

void MainWindow::on_pushButton_7x_clicked()
{
    removePhoto(6);
}

void MainWindow::on_pushButton_8x_clicked()
{
    removePhoto(7);
}

void MainWindow::on_pushButton_9x_clicked()
{
    removePhoto(8);
}

void MainWindow::on_pushButton_10x_clicked()
{
    removePhoto(9);
}

// 新窗口显示照片
void MainWindow::showPhoto(int i)
{
    if (i < photoList.length()) {
        showPhotoDialog = new ShowPhotoDialog;
        showPhotoDialog->setAttribute(Qt::WA_DeleteOnClose);

        showPhotoDialog->setPhoto(*labelList[i]->pixmap());
        showPhotoDialog->show();
    }
}

void MainWindow::on_pushButton_1o_clicked()
{
    showPhoto(0);
}

void MainWindow::on_pushButton_2o_clicked()
{
    showPhoto(1);
}

void MainWindow::on_pushButton_3o_clicked()
{
    showPhoto(2);
}

void MainWindow::on_pushButton_4o_clicked()
{
    showPhoto(3);
}

void MainWindow::on_pushButton_5o_clicked()
{
    showPhoto(4);
}

void MainWindow::on_pushButton_6o_clicked()
{
    showPhoto(5);
}

void MainWindow::on_pushButton_7o_clicked()
{
    showPhoto(6);
}

void MainWindow::on_pushButton_8o_clicked()
{
    showPhoto(7);
}

void MainWindow::on_pushButton_9o_clicked()
{
    showPhoto(8);
}

void MainWindow::on_pushButton_10o_clicked()
{
    showPhoto(9);
}
