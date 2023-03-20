#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma execution_character_set("utf-8")//display chinese words

//QT Files
#include <QMainWindow>
#include <QFileDialog>
#include <QVector>
//cv Files
#include"opencv2/opencv.hpp"
#include"opencv2/imgproc/types_c.h"
//Windows
#include <algorithm>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    cv::Mat m_srcimg, m_greyimg, m_noiseimg;
    //获得灰度图的子函数
    cv::Mat GetGrayLevel(cv::Mat grey);

    void DoubleThresholdLink(cv::Mat &imageInput,double lowThreshold,double highThreshold);

    void DoubleThreshold(cv::Mat &imageIput,double lowThreshold,double highThreshold);

private slots:

    //选择图片
    void on_ChooseImage_clicked();
    //彩色图像转成灰色图像
    void on_ConvertToGrey_clicked();
    //获得灰度直方图
    void on_GreyHistogram_clicked();
    //灰度均衡
    void on_GreyBalance_clicked();
    //梯度锐化
    void on_GradientSharpening_clicked();

    void on_Roberts_clicked();

    void on_Sobel_clicked();

    void on_Laplace_clicked();


    void on_canny_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
