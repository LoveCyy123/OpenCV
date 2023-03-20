#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_ChooseImage_clicked()//Choose one jpg
{
    QString FileName = QFileDialog::getOpenFileName(this, tr(""), "../../../../open_image","files(*)");

    m_srcimg = cv::imread(FileName.toStdString());

    if(!m_srcimg.data){

        std::cout<<"Choosen Image is empty"<<std::endl;

        return;
    }

    cv::cvtColor(m_srcimg, m_greyimg, CV_BGR2GRAY);//map convert to greyimg

    cv::Mat TmpImg;

    cv::cvtColor(m_srcimg, TmpImg, CV_BGR2RGB);//BGR convert to RGB

    QImage QTmpImg = QImage((const unsigned char*)(TmpImg.data), TmpImg.cols, TmpImg.rows, TmpImg.step, QImage::Format_RGB888);

    ui->Label1->setPixmap(QPixmap::fromImage(QTmpImg));
    QTmpImg = QTmpImg.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Label1->setScaledContents(true);
    ui->Label1->resize(QTmpImg.size());
    ui->Label1->show();
}


void MainWindow::on_ConvertToGrey_clicked()//BGR convert to GREY
{
    if(!m_greyimg.data){

        std::cout<<"BGR image is empty"<<std::endl;

        return;
    }

    m_greyimg.create(m_srcimg.rows, m_srcimg.cols, CV_8UC1);

    //使用加权平均法来进行灰度处理
    for(int i = 0 ; i < m_srcimg.rows ; i++){
        for(int j = 0 ; j < m_srcimg.cols ; j ++){
            m_greyimg.at<uchar>(i, j) = (int)0.11 * m_srcimg.at<cv::Vec3b>(i, j)[0]
                                            + 0.59 * m_srcimg.at<cv::Vec3b>(i, j)[1]
                                            + 0.3 * m_srcimg.at<cv::Vec3b>(i, j)[2];

        }
    }

    QImage Qtmp = QImage((const uchar*)(m_greyimg.data), m_greyimg.cols, m_greyimg.rows, m_greyimg.cols*m_greyimg.channels(), QImage::Format_Indexed8);
    ui->Label2->setPixmap(QPixmap::fromImage(Qtmp));
    Qtmp = Qtmp.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Label2->setScaledContents(true);
    ui->Label2->resize(Qtmp.size());
    ui->Label2->show();
}

cv::Mat MainWindow::GetGrayLevel(cv::Mat grey)
{
    if(!grey.data){

        std::cout<<"GetGreayLevel input is NULL"<<std::endl;

        return grey;

    }
    cv::Mat graylevel;
    QVector<int> pixelvec(256, 0);

    //将所有图像的灰度值分布保存到QVec中
    for(int i = 0 ; i < grey.rows ; i++){
        for(int j = 0 ; j <grey.cols ; j++){
            pixelvec[grey.at<uchar>(i,j)]++;
        }
    }

    graylevel.create(350, 256, CV_8UC1);

    int max_rows = 0 ;

    //找到所有灰度值中最大的灰度值,即灰度直方图的最高频灰度
    for(int i = 0 ; i <= 255 ; i++){
        if(pixelvec[i] > max_rows){
            max_rows = pixelvec[i];
        }
    }

    //设定灰度直方图灰度值为255
    for(int i = 0 ; i < 256 ; i++){
        for(int j = 0 ; j < 350 ; j++){
            graylevel.at<uchar>(j,i) = 255;
        }
    }

    //以350为纵轴，256为横轴，在灰度直方图的每一个点都设为白色的基础上，将比例图画出来
    for(int i = 0 ; i < 256 ; i++){
        for(int j = 0 ; j < 350 - int(320.*float(pixelvec[i])/float(max_rows)); j++){
            graylevel.at<uchar>(j,i) = 0;
        }
    }

    return graylevel;
}

void MainWindow::on_GreyHistogram_clicked()//BGR convert to Grey Hisstom
{
    if(!m_greyimg.data){

        std::cout<<"Grey image is empty"<<std::endl;

        return;
    }

    QImage Qtmp = QImage((const uchar*)(m_greyimg.data), m_greyimg.cols, m_greyimg.rows, m_greyimg.cols*m_greyimg.channels(), QImage::Format_Indexed8);

    ui->Label2->setPixmap(QPixmap::fromImage(Qtmp));
    Qtmp = Qtmp.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Label2->setScaledContents(true);
    ui->Label2->resize(Qtmp.size());
    ui->Label2->show();

    cv::Mat greylevel = GetGrayLevel(m_greyimg);

    cv::imshow("GreyLevel ",greylevel);
    //cv::waitKey(0);

    //cv::destroyWindow("GreyLevel");
    //cv::waitKey(1);
}



void MainWindow::on_GreyBalance_clicked()
{
    if(!m_greyimg.data || !m_srcimg.data){

        std::cout<<"Grey image || Src image is empty"<<std::endl;

        return;
    }

    cv::Mat balancedimg;
    balancedimg.create(m_srcimg.rows, m_srcimg.cols, CV_8UC1);

    QVector<int> pixel(256, 0);
    QVector<float> pixel_gray(256, 0.);
    float pixel_count = 0;

    //将像素的灰度值储存在vec中
    for(int i = 0 ; i < m_srcimg.rows ; i++){
        for(int j = 0 ; j <m_srcimg.cols ; j++){
            pixel[m_greyimg.at<uchar>(i,j)]++;
        }
    }

    //计算所有的像素点个数
    for(auto& val : pixel){
        pixel_count += val;
    }

    /*
     * 此段代码的意义在于：
     * 将每一列的像素总值算出来
     *  *255 / 总像素数
     * 得到每一列的像素数
    */
    for(int i = 0 ; i < 256 ; i++){
        float pixel_sum = 0.;
        for(int j = 0 ; j <= i ; j++){
            pixel_sum += pixel[j];
        }
        pixel_gray[i] = 255*pixel_sum/pixel_count;
    }

    //再将灰度值图像对应的均衡灰度值传递给banlancedimg
    for(int i = 0 ; i < m_srcimg.rows ; i++){
        for(int j = 0 ; j < m_srcimg.cols ; j++){
            balancedimg.at<uchar>(i,j) = pixel_gray[m_greyimg.at<uchar>(i,j)];
        }
    }

    QImage Qtmp = QImage((const uchar*)(balancedimg.data), balancedimg.cols, balancedimg.rows, balancedimg.cols*balancedimg.channels(), QImage::Format_Indexed8);
    ui->Label4->setPixmap(QPixmap::fromImage(Qtmp));
    Qtmp = Qtmp.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Label4->setScaledContents(true);
    ui->Label4->resize(Qtmp.size());
    ui->Label4->show();

}



void MainWindow::on_GradientSharpening_clicked()
{
    if(!m_greyimg.data || !m_srcimg.data){

        std::cout<<"Src image || Grey image is empty"<<std::endl;

        return;
    }
    cv::Mat grad1,grad2;

    grad1.create(m_srcimg.rows, m_srcimg.cols, CV_8UC1);
    grad2.create(m_srcimg.rows, m_srcimg.cols, CV_8UC1);


    //grad1使用了最典型的梯度算法，也就是 x,y x+1,y x,y+1 再去求x，y的偏导
    for(int i = 0 ; i < m_srcimg.rows - 1 ; i++)
        for(int j = 0 ; j < m_srcimg.cols - 1 ; j++){
            grad1.at<uchar>(i,j) = cv::saturate_cast<uchar>(std::max(fabs(m_greyimg.at<uchar>(i+1, j)-m_greyimg.at<uchar>(i,j)),fabs(m_greyimg.at<uchar>(i, j+1)-m_greyimg.at<uchar>(i,j))));
            //grad2.at<uchar>(i,j) = cv::saturate_cast<uchar>(m_greyimg.at<uchar>(i,j) - m_greyimg.at<uchar>(i,j));
        }

    cv::Sobel(m_greyimg, grad2, m_greyimg.depth(), 1, 1, 3, 1, 0, cv::BORDER_DEFAULT);
    //todo::手写一个sobel算子锐化灰度图像
    QImage Qtemp1,Qtemp2;

    Qtemp1 = QImage((const uchar*)(grad1.data), grad1.cols, grad1.rows, grad1.cols*grad1.channels(), QImage::Format_Indexed8);
    ui->Label3->setPixmap(QPixmap::fromImage(Qtemp1));
    Qtemp1 = Qtemp1.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Label3->setScaledContents(true);
    ui->Label3->resize(Qtemp1.size());
    ui->Label3->show();


    Qtemp2 = QImage((const uchar*)(grad2.data), grad2.cols, grad2.rows, grad2.cols*grad2.channels(), QImage::Format_Indexed8);
    ui->Label4->setPixmap(QPixmap::fromImage(Qtemp2));
    Qtemp2 = Qtemp2.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Label4->setScaledContents(true);
    ui->Label4->resize(Qtemp2.size());
    ui->Label4->show();
}

void MainWindow::on_Roberts_clicked()
{
    if(!m_greyimg.data || !m_srcimg.data){

        std::cout<<"Src image || Grey image is empty"<<std::endl;

        return;
    }
    cv::Mat grad1,grad2;

    grad1.create(m_srcimg.rows, m_srcimg.cols, CV_8UC1);
    grad2.create(m_srcimg.rows, m_srcimg.cols, CV_8UC1);

    //roberts算子为 -1 0  和  0 -1
    //             0  1     1  0
    //那么，根据卷积的计算公式就是 i+1，j+1 -  i,j      i+1,j  -   i,j+1
    //grad1使用了最典型的梯度算法，也就是 x,y x+1,y x,y+1 再去求x，y的偏导
    for(int i = 0 ; i < m_srcimg.rows - 1 ; i++)
        for(int j = 0 ; j < m_srcimg.cols - 1 ; j++){
            grad1.at<uchar>(i,j) = cv::saturate_cast<uchar>((fabs(m_greyimg.at<uchar>(i+1, j+1)-m_greyimg.at<uchar>(i,j)),fabs(m_greyimg.at<uchar>(i+1, j)-m_greyimg.at<uchar>(i,j+1))));
            grad2.at<uchar>(i,j) = cv::saturate_cast<uchar>(m_greyimg.at<uchar>(i,j) - m_greyimg.at<uchar>(i,j));
        }

    //cv::Sobel(m_greyimg, grad2, m_greyimg.depth(), 1, 1, 3, 1, 0, cv::BORDER_DEFAULT);
    //todo::手写一个sobel算子锐化灰度图像
    QImage Qtemp1,Qtemp2;

    Qtemp1 = QImage((const uchar*)(grad1.data), grad1.cols, grad1.rows, grad1.cols*grad1.channels(), QImage::Format_Indexed8);
    ui->Label3->setPixmap(QPixmap::fromImage(Qtemp1));
    Qtemp1 = Qtemp1.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Label3->setScaledContents(true);
    ui->Label3->resize(Qtemp1.size());
    ui->Label3->show();


    Qtemp2 = QImage((const uchar*)(grad2.data), grad2.cols, grad2.rows, grad2.cols*grad2.channels(), QImage::Format_Indexed8);
    ui->Label4->setPixmap(QPixmap::fromImage(Qtemp2));
    Qtemp2 = Qtemp2.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Label4->setScaledContents(true);
    ui->Label4->resize(Qtemp2.size());
    ui->Label4->show();
}

void MainWindow::on_Sobel_clicked()
{
    if(!m_greyimg.data || !m_srcimg.data){

        std::cout<<"Src image || Grey image is empty"<<std::endl;

        return;
    }
    cv::Mat grad1,grad2,dx,dy,dm,dn;

    grad1.create(m_srcimg.rows, m_srcimg.cols, CV_8UC1);
    grad2.create(m_srcimg.rows, m_srcimg.cols, CV_8UC1);
    dx.create(m_srcimg.rows, m_srcimg.cols, CV_8UC1);
    dy.create(m_srcimg.rows, m_srcimg.cols, CV_8UC1);
    dm.create(m_srcimg.rows, m_srcimg.cols, CV_8UC1);
    dn.create(m_srcimg.rows, m_srcimg.cols, CV_8UC1);

    //              x方向        y方向
    //roberts算子为 -1 0 1 和  -1 -2 -1
    //             -2 0 2     0  0  0   -----> I 方向 向上为y的方向
    //
    //             -1 0 1     1  2  1   | j方向
    //那么，根据卷积的计算公式就是 i+1，j+1 -  i,j      i+1,j  -   i,j+1
    //grad1使用了最典型的梯度算法，也就是 x,y x+1,y x,y+1 再去求x，y的偏导
    for(int i = 1 ; i < m_srcimg.rows - 1 ; i++) //此处的从1开始，因为要将后面的点的值删掉
        for(int j = 1 ; j < m_srcimg.cols - 1 ; j++){
            dx.at<uchar>(i,j) = cv::saturate_cast<uchar>(fabs(m_greyimg.at<uchar>(i+1, j+1)-m_greyimg.at<uchar>(i - 1 ,j - 1) + 2 *m_greyimg.at<uchar>(i+ 1,j) +
                                                            m_greyimg.at<uchar>(i + 1 ,j - 1) - 2*m_greyimg.at<uchar>(i - 1 ,j) - m_greyimg.at<uchar>(i - 1 ,j + 1 )));
            dy.at<uchar>(i,j) = cv::saturate_cast<uchar>(fabs( m_greyimg.at<uchar>(i - 1 ,j - 1) + 2 *m_greyimg.at<uchar>(i ,j-1) - m_greyimg.at<uchar>(i+1, j+1)+
                                                              m_greyimg.at<uchar>(i + 1 ,j - 1) - 2*m_greyimg.at<uchar>(i , j+1) - m_greyimg.at<uchar>(i - 1 ,j + 1 )));

            grad1.at<uchar>(i,j) = dx.at<uchar>(i,j) + dy.at<uchar>(i,j);

            dm.at<uchar>(i,j) = cv::saturate_cast<uchar>(fabs(3 * m_greyimg.at<uchar>(i+1, j+1) + 10 * m_greyimg.at<uchar>(i + 1 ,j) + 3 *m_greyimg.at<uchar>(i+ 1,j -1 ) - 3*
                                                            m_greyimg.at<uchar>(i - 1 ,j + 1) - 10*m_greyimg.at<uchar>(i - 1 ,j) -3* m_greyimg.at<uchar>(i - 1 ,j - 1 )));
            dn.at<uchar>(i,j) = cv::saturate_cast<uchar>(fabs(3*m_greyimg.at<uchar>(i - 1 ,j - 1) + 10 *m_greyimg.at<uchar>(i ,j-1) -3* m_greyimg.at<uchar>(i+1, j+1)+3*
                                                              m_greyimg.at<uchar>(i + 1 ,j - 1) - 10*m_greyimg.at<uchar>(i , j+1) -3* m_greyimg.at<uchar>(i - 1 ,j + 1 )));

            grad2.at<uchar>(i,j) = dm.at<uchar>(i,j) + dn.at<uchar>(i,j);
        }


    QImage Qtemp1,Qtemp2;

    Qtemp1 = QImage((const uchar*)(grad1.data), grad1.cols, grad1.rows, grad1.cols*grad1.channels(), QImage::Format_Indexed8);
    ui->Label3->setPixmap(QPixmap::fromImage(Qtemp1));
    Qtemp1 = Qtemp1.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Label3->setScaledContents(true);
    ui->Label3->resize(Qtemp1.size());
    ui->Label3->show();


    Qtemp2 = QImage((const uchar*)(grad2.data), grad2.cols, grad2.rows, grad2.cols*grad2.channels(), QImage::Format_Indexed8);
    ui->Label4->setPixmap(QPixmap::fromImage(Qtemp2));
    Qtemp2 = Qtemp2.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Label4->setScaledContents(true);
    ui->Label4->resize(Qtemp2.size());
    ui->Label4->show();

}

void MainWindow::on_Laplace_clicked()
{
    if(!m_greyimg.data || !m_srcimg.data){

        std::cout<<"Src image || Grey image is empty"<<std::endl;

        return;
    }
    cv::Mat grad1,grad2;

    grad1.create(m_srcimg.rows, m_srcimg.cols, CV_8UC1);
    grad2.create(m_srcimg.rows, m_srcimg.cols, CV_8UC1);

    //laplace算子为  0 1 0 1 -4 1 0 1 0  和  1 1 1 1 -8 1 1 1 1
    //那么，根据卷积的计算公式就是 i+1，j+1 -  i,j      i+1,j  -   i,j+1
    //grad1使用了最典型的梯度算法，也就是 x,y x+1,y x,y+1 再去求x，y的偏导
    for(int i = 1 ; i < m_srcimg.rows - 1 ; i++)
        for(int j = 1 ; j < m_srcimg.cols - 1 ; j++){
            grad1.at<uchar>(i,j) = cv::saturate_cast<uchar>((fabs(m_greyimg.at<uchar>(i+1, j)-4*m_greyimg.at<uchar>(i,j)+m_greyimg.at<uchar>(i, j+1)+m_greyimg.at<uchar>(i-1, j)+m_greyimg.at<uchar>(i, j-1))));
            grad2.at<uchar>(i,j) = cv::saturate_cast<uchar>((fabs(m_greyimg.at<uchar>(i+1, j)-8*m_greyimg.at<uchar>(i,j)+m_greyimg.at<uchar>(i, j+1)+m_greyimg.at<uchar>(i-1, j)+m_greyimg.at<uchar>(i, j-1)
                                                                  +m_greyimg.at<uchar>(i-1, j-1)+m_greyimg.at<uchar>(i+1, j-1)+m_greyimg.at<uchar>(i-1, j+1)+m_greyimg.at<uchar>(i+1, j+1))));
        }
    QImage Qtemp1,Qtemp2;

    Qtemp1 = QImage((const uchar*)(grad1.data), grad1.cols, grad1.rows, grad1.cols*grad1.channels(), QImage::Format_Indexed8);
    ui->Label3->setPixmap(QPixmap::fromImage(Qtemp1));
    Qtemp1 = Qtemp1.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Label3->setScaledContents(true);
    ui->Label3->resize(Qtemp1.size());
    ui->Label3->show();


    Qtemp2 = QImage((const uchar*)(grad2.data), grad2.cols, grad2.rows, grad2.cols*grad2.channels(), QImage::Format_Indexed8);
    ui->Label4->setPixmap(QPixmap::fromImage(Qtemp2));
    Qtemp2 = Qtemp2.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Label4->setScaledContents(true);
    ui->Label4->resize(Qtemp2.size());
    ui->Label4->show();
}

void MainWindow::DoubleThresholdLink(cv::Mat &imageInput,double lowThreshold,double highThreshold)
{
    for(int i=1;i<imageInput.rows-1;i++)
    {
        for(int j=1;j<imageInput.cols-1;j++)
        {
            if(imageInput.at<uchar>(i,j)>lowThreshold&&imageInput.at<uchar>(i,j)<255)
            {
                if(imageInput.at<uchar>(i-1,j-1)==255||imageInput.at<uchar>(i-1,j)==255||imageInput.at<uchar>(i-1,j+1)==255||
                    imageInput.at<uchar>(i,j-1)==255||imageInput.at<uchar>(i,j)==255||imageInput.at<uchar>(i,j+1)==255||
                    imageInput.at<uchar>(i+1,j-1)==255||imageInput.at<uchar>(i+1,j)==255||imageInput.at<uchar>(i+1,j+1)==255)
                {
                    imageInput.at<uchar>(i,j)=255;
                    DoubleThresholdLink(imageInput,lowThreshold,highThreshold); //递归调用
                }
                else
            {
                    imageInput.at<uchar>(i,j)=0;
            }
            }
        }
    }
}

void MainWindow::DoubleThreshold(cv::Mat &imageIput,double lowThreshold,double highThreshold)
{
    for(int i=0;i<imageIput.rows;i++)
    {
        for(int j=0;j<imageIput.cols;j++)
        {
            if(imageIput.at<uchar>(i,j)>highThreshold)
            {
                imageIput.at<uchar>(i,j)=255;
            }
            if(imageIput.at<uchar>(i,j)<lowThreshold)
            {
                imageIput.at<uchar>(i,j)=0;
            }
        }
    }
}


void MainWindow::on_canny_clicked()
{
    //1.转换灰度
    if(!m_greyimg.data || !m_srcimg.data){

        std::cout<<"Src image || Grey image is empty"<<std::endl;

        return;
        
    }
   
    //2.滤波降噪处理,使用高斯滤波

    std::cout<<"gauss start "<<std::endl;
    cv::Mat gauss,grey;
    gauss.create(m_greyimg.rows, m_greyimg.cols, CV_8UC1);
    grey.create(m_greyimg.rows, m_greyimg.cols, CV_8UC1);
    for (int i = 1; i < m_greyimg.rows - 1; i++)
    {
        for (int j = 1; j < m_greyimg.cols - 1; j++){
            gauss.at<uchar>(i,j) = cv::saturate_cast<uchar>(fabs((0.751136*m_greyimg.at<uchar>(i - 1, j - 1) +0.123841* m_greyimg.at<uchar>(i - 1, j) +0.0751136* m_greyimg.at<uchar>(i - 1, j + 1)
                                                         +0.123841*m_greyimg.at<uchar>(i, j - 1) +0.20418*m_greyimg.at<uchar>(i, j) + 0.123841*m_greyimg.at<uchar>(i, j + 1)
                                                         + 0.0751136*m_greyimg.at<uchar>(i + 1, j - 1) + 0.123841*m_greyimg.at<uchar>(i + 1, j) + 0.0751136*m_greyimg.at<uchar>(i + 1, j + 1))));
        }
    }//todo:: gauss滤波的数学原理

    std::cout<<"gauss success "<<std::endl;

    //3.差分计算幅值和方向,sobel处理
    cv::Mat f_x,f_y;
    f_x.create(m_greyimg.rows, m_greyimg.cols, CV_8UC1);
    f_y.create(m_greyimg.rows, m_greyimg.cols, CV_8UC1);
    QVector<double> direction((m_greyimg.rows-1)*(m_greyimg.rows-1),0);
    int k = 0;
    for (int i = 1; i < gauss.rows - 1; i++)
    {
        for (int j = 1; j < gauss.cols - 1; j++)
        {
            f_x.at<uchar>(i, j) = cv::saturate_cast<uchar>(fabs(m_greyimg.at<uchar>(i + 1, j - 1) + 2*m_greyimg.at<uchar>(i + 1, j) + m_greyimg.at<uchar>(i + 1, j + 1)
                                                            - m_greyimg.at<uchar>(i - 1, j - 1) - 2*m_greyimg.at<uchar>(i - 1, j) - m_greyimg.at<uchar>(i - 1, j + 1)));
            f_y.at<uchar>(i, j) = cv::saturate_cast<uchar>(fabs(m_greyimg.at<uchar>(i - 1, j + 1) + 2*m_greyimg.at<uchar>(i, j + 1) + m_greyimg.at<uchar>(i + 1, j + 1)
                                                            - m_greyimg.at<uchar>(i - 1, j - 1) - 2*m_greyimg.at<uchar>(i, j - 1) - m_greyimg.at<uchar>(i + 1, j - 1)));
            grey.at<uchar>(i, j) = sqrt(pow(f_x.at<uchar>(i, j),2)+pow(f_y.at<uchar>(i, j),2));

            if(f_x.at<uchar>(i, j)==0)
            {
                direction[k]=atan(f_y.at<uchar>(i, j)/0.0000000001)*57.3;  //防止除数为0异常
            }
            else {
                direction[k]=atan(f_y.at<uchar>(i, j)/f_x.at<uchar>(i, j))*57.3;
            }
            direction[k]+=90;
            k++;
        }
    }

    std::cout<<"sobel success , k 是 "<<k<<std::endl;

    //4.极大值抑制
    cv::Mat max_control=grey.clone();
    int count = 0;
    for (int i = 2; i < gauss.rows - 2; i++)
    {
        for (int j = 2; j < gauss.cols -2 ; j++){

            if(count > k){
                std::cout<<"count beyond limit, count is "<<count<<std::endl;
                continue;
            }

            //std::cout<<"count  is "<<count<<std::endl;

            int value00=grey.at<uchar>((i-1),j-1);
            int value01=grey.at<uchar>((i-1),j);
            int value02=grey.at<uchar>((i-1),j+1);
            int value10=grey.at<uchar>((i),j-1);
            int value11=grey.at<uchar>((i),j);
            int value12=grey.at<uchar>((i),j+1);
            int value20=grey.at<uchar>((i+1),j-1);
            int value21=grey.at<uchar>((i+1),j);
            int value22=grey.at<uchar>((i+1),j+1);

            if(direction[count]>0&&direction[count]<=45)
            {
                if(value11<=(value12+(value02-value12)*tan(direction[i*max_control.rows+j]))||(value11<=(value10+(value20-value10)*tan(direction[i*max_control.rows+j]))))
                {
                    max_control.at<uchar>(i,j)=0;
                }
            }

            if(direction[count]>45&&direction[count]<=90)
            {
                if(value11<=(value01+(value02-value01)/tan(direction[i*max_control.cols+j]))||value11<=(value21+(value20-value21)/tan(direction[i*max_control.cols+j])))
                {
                    max_control.at<uchar>(i,j)=0;

                }
            }

            if(direction[count]>90&&direction[count]<=135)
            {
                if(value11<=(value01+(value00-value01)/tan(180-direction[i*max_control.cols+j]))||value11<=(value21+(value22-value21)/tan(180-direction[i*max_control.cols+j])))
                {
                    max_control.at<uchar>(i,j)=0;
                }
            }
            if(direction[count]>135&&direction[count]<=180)
            {
                if(value11<=(value10+(value00-value10)*tan(180-direction[i*max_control.cols+j]))||value11<=(value12+(value22-value11)*tan(180-direction[i*max_control.cols+j])))
                {
                    max_control.at<uchar>(i,j)=0;
                }
            }
            count++;
        }
    }

    std::cout<<"max control success "<<std::endl;
    //5.滞后阈值,canny双阈值链接

    DoubleThreshold(max_control,10,40);
    DoubleThresholdLink(max_control,10,40);
    cv::Mat res_img;
    res_img.create(m_greyimg.rows, m_greyimg.cols, CV_8UC1);
    for (int i = 0; i < m_greyimg.rows - 1; i++)
    {
        for (int j = 0; j < m_greyimg.cols - 1; j++){
            res_img.at<uchar>(i, j) = cv::saturate_cast<uchar>(m_greyimg.at<uchar>(i, j) - max_control.at<uchar>(i, j));
        }
    }

    QImage Qtemp2 = QImage((const uchar*)(max_control.data), max_control.cols, max_control.rows, max_control.cols*max_control.channels(), QImage::Format_Indexed8);
    ui->Label3->setPixmap(QPixmap::fromImage(Qtemp2));
    Qtemp2 = Qtemp2.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Label3->setScaledContents(true);
    ui->Label3->resize(Qtemp2.size());
    ui->Label3->show();

    QImage Qtemp = QImage((const uchar*)(res_img.data), res_img.cols, res_img.rows, res_img.cols*res_img.channels(), QImage::Format_Indexed8);
    ui->Label4->setPixmap(QPixmap::fromImage(Qtemp));
    Qtemp = Qtemp.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Label4->setScaledContents(true);
    ui->Label4->resize(Qtemp.size());
    ui->Label4->show();


}
