#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "base.h"
#include "camerathread.h"
#include "showphotodialog.h"
#include "savephotothread.h"
#include "learndialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *e);

    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    int key;

    int Start;

    QString name;

    int Algo;

    CameraThread camerathread;

    ShowPhotoDialog *showPhotoDialog;

    SavePhotoThread savePhotoThread;

    LearnDialog *learnDialog;

    QList<MyImage> photoList;

    QList<QLabel *> labelList;

    void startCameraThread(int);

    void handelLabelImage();

    void handelNameChange();

    QImage UMat2QImage(UMat);

    UMat QImage2UMat(QImage);

    void removePhoto(int);

    void showPhoto(int);

private slots:
    void getCameraThreadMsg(int);

    void getSavePhotoThreadMsg(int);

    void getCamera(QImage);

    void getPhoto(QImage, int);

    void getProcess(int);

    void on_pushButton_start_clicked();

    void on_pushButton_p_clicked();

    void on_lineEdit_name_textChanged(const QString &arg1);

    void on_pushButton_save_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_1x_clicked();

    void on_pushButton_2x_clicked();

    void on_pushButton_3x_clicked();

    void on_pushButton_4x_clicked();

    void on_pushButton_5x_clicked();

    void on_pushButton_6x_clicked();

    void on_pushButton_7x_clicked();

    void on_pushButton_8x_clicked();

    void on_pushButton_9x_clicked();

    void on_pushButton_10x_clicked();

    void on_pushButton_1o_clicked();

    void on_pushButton_2o_clicked();

    void on_pushButton_3o_clicked();

    void on_pushButton_4o_clicked();

    void on_pushButton_5o_clicked();

    void on_pushButton_6o_clicked();

    void on_pushButton_7o_clicked();

    void on_pushButton_8o_clicked();

    void on_pushButton_9o_clicked();

    void on_pushButton_10o_clicked();

    void on_pushButton_learn_clicked();

signals:
    void postCameraThreadMsg(int);

    void postName(QString);

    void postPhoto(QImage);

};

#endif // MAINWINDOW_H
