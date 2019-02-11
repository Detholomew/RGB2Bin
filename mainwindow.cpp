#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>


#include<opencv2/core/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cvImg_in = cv::imread("img_default.jpg"); //defaul image
    resetBand();
    cv::cvtColor(cvImg_in,cvImg_in,COLOR_BGR2RGB);
    cvImg_host = cvImg_in;
    h = cvImg_host.rows;
    w = cvImg_host.cols;
    frameSizeout=sizeof(unsigned char)*w*h;
    QImg_rgb = QImage((const unsigned char*)(cvImg_host.data),w,h, cvImg_host.step,  QImage::Format_RGB888);
    ui->label->clear();
    ui->label->setPixmap(QPixmap::fromImage(QImg_rgb));
    ui->label->resize(ui->label->pixmap()->size());

    callKernel();

    QImg_bin = QImage((const unsigned char*)(cvImg_out.data),w,h,cvImg_out.step,  QImage::Format_Indexed8);
    ui->label_2->clear();
    ui->label_2->setPixmap(QPixmap::fromImage(QImg_bin));
    ui->label_2->resize(ui->label_2->pixmap()->size());


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::callKernel()
{
    cvImg_out = Mat::zeros(h, w, CV_8U);
    unsigned  char* outdata_h = (unsigned  char*)malloc(frameSizeout);
    binGreen(cvImg_host.data, outdata_h, w, h, hL, hH, sL, sH, vL, vH);
    memcpy(cvImg_out.data, outdata_h, frameSizeout);
    free(outdata_h);

}



void MainWindow::resetBand()
{
    //hL = 70,  hH = 160,  sL = 43, sH = 255,  vL = 46,  vH = 255;
     hL = 60,  hH = 180,  sL = 43, sH = 255,  vL = 46,  vH = 255;
    setRange();
    chanel=3;
    ui->radioButton_3->click();
    ui->verticalSlider->setValue(0);
    ui->verticalSlider_2->setValue(0);

}



void MainWindow::on_pushButton_clicked()  // choose image
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Image"),"",tr("Image File(*.bmp *.jpg *.jpeg *.png)"));
    QTextCodec *code = QTextCodec::codecForName("gb18030");
    std::string name = code->fromUnicode(filename).data();
    cvImg_in = cv::imread(name);
    if(!cvImg_in.data)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("image data null"));
        msgBox.exec();
        return;
    }
    if ( cvImg_in.channels() != 3)
    {
        cvImg_in.data=0;
        QMessageBox msgBox;
        msgBox.setText(tr("image data wrong"));
        msgBox.exec();
        return;
    }
    //cv BGR plan to RGB plan
    cv::cvtColor(cvImg_in,cvImg_in,COLOR_BGR2RGB);
    int imgH =cvImg_in.rows;
    int imgW = cvImg_in.cols;
    int imageSize=imgH*imgW;

//considering mainwindow size and image preview
//if image size too big, before save we reduce image data size
    if (imageSize>360*280)
    {
        double scale = sqrt(imageSize/(360 * 280));
        Size dsize = Size((int)imgW/scale, (int)imgH/scale); //default size
        cvImg_host = Mat(dsize, CV_32S);
        cv::resize( cvImg_in, cvImg_host,dsize);

    }
    else
        cvImg_host = cvImg_in;
    resetBand();
    h = cvImg_host.rows;
    w = cvImg_host.cols;
    frameSizeout=sizeof(unsigned char)*w*h;
    QImg_rgb = QImage((const unsigned char*)(cvImg_host.data),w,h, cvImg_host.step,  QImage::Format_RGB888);
    ui->label->clear();
    ui->label->setPixmap(QPixmap::fromImage(QImg_rgb));
    ui->label->resize(ui->label->pixmap()->size());

    callKernel();

    QImg_bin = QImage((const unsigned char*)(cvImg_out.data),w,h,cvImg_out.step,  QImage::Format_Indexed8);
    ui->label_2->clear();
    ui->label_2->setPixmap(QPixmap::fromImage(QImg_bin));
    ui->label_2->resize(ui->label_2->pixmap()->size());
}

void MainWindow::on_pushButton_2_clicked()
{QString fileName = QFileDialog::getSaveFileName(this,tr("Save Image"), "", tr("Image File(*.bmp *.jpg *.jpeg *.png)"));
    QTextCodec *code = QTextCodec::codecForName("gb18030");
    std::string name = code->fromUnicode(fileName).data();
        if (fileName.isNull())
        {
            QMessageBox msgBox;
            msgBox.setText(tr("no filename entred"));
            msgBox.exec();

            return;
        }
        if(!cvImg_in.data)
        {
            QMessageBox msgBox;
            msgBox.setText(tr("please load an image"));
            msgBox.exec();
            return;
        }
        //
        int imgH =cvImg_in.rows;
        int imgW = cvImg_in.cols;
        int imgSize = sizeof(unsigned  char)*imgH*imgW;
        Mat cvImg_save = Mat::zeros(imgH, imgW, CV_8U);
        unsigned  char* outdata_h = (unsigned  char*)malloc(imgSize);
        //from here, timing time used for creating binarized image
        double start = clock();
        binGreen(cvImg_in.data, outdata_h, imgW, imgH, hL, hH, sL, sH, vL, vH);
        memcpy(cvImg_save.data, outdata_h, imgSize);
        free(outdata_h);
        double fin = clock();
        double useTime=(fin - start);
        //save image
        cvImg_save.convertTo(cvImg_save, CV_8UC3, 255.0, 0);
        imshow("image out", cvImg_save);
        imwrite(name, cvImg_save);
        //display time used in new window
        std::string mesT = "Time(ms) used :  " + std::to_string(useTime);
        QMessageBox msgBox;
        msgBox.setText(tr(mesT.c_str()));
        msgBox.exec();
}

void MainWindow::on_pushButton_3_clicked()
{
    resetBand();
    if(!cvImg_in.data)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("please load an image"));
        msgBox.exec();
        return;
    }
    callKernel();
    QImg_bin = QImage((const unsigned char*)(cvImg_out.data),w,h,cvImg_out.step,  QImage::Format_Indexed8);
    ui->label_2->clear();
    ui->label_2->setPixmap(QPixmap::fromImage(QImg_bin));
    ui->label_2->resize(ui->label_2->pixmap()->size());

}


void MainWindow::setValue(int mark)
{
    if(mark==1)
    {  //min
        float value=ui->verticalSlider->value();
        value=value/100;
        switch (chanel) {
        case 3: //v
            vL=value*(255-46)+46;
            break;
        case 2: //s
            sL=value*(255-43)+43;
            break;
        case 1: //h
            hL=value*(180-60)+60;
            break;
        }
    }
    else
    {  //max
        float value=100-ui->verticalSlider_2->value();
        value=value/100;
        switch (chanel) {
        case 3: //v
            vH=46+value*(255-46);
            break;
        case 2: //s
            sH=43+value*(255-43);
            break;
        case 1: //h
            hH=60+value*(180-60);
            break;
        }
    }
}


void MainWindow::setRange(){
    switch (chanel) {
    case 3://v
    {int vband=255-46;
        int vmax=100*(255-vH)/vband;
        int vmin=100*(vL-46)/vband;
        ui->verticalSlider_2->setValue(vmax);
        ui->verticalSlider->setValue(vmin);
        ui->label_4->setText(QStringLiteral("%1").arg(100-vmax));
        ui->label_3->setText(QStringLiteral("%1").arg(vmin));
        break;}
    case 2://s
      {  int sband=255-43;
        int smax=100*(255-sH)/sband;
        int smin=100*(sL-43)/sband;
        ui->label_4->setText(QStringLiteral("%1").arg(100-smax));
        ui->label_3->setText(QStringLiteral("%1").arg(smin));
        ui->verticalSlider_2->setValue(smax);
        ui->verticalSlider->setValue(smin);
        break;}
    case 1://h
    {
        int hband=180-60;
        int hmax=100*(180-hH)/hband;
        int hmin=100*(hL-60)/hband;
        ui->verticalSlider_2->setValue(hmax);
        ui->verticalSlider->setValue(hmin);
        ui->label_4->setText(QStringLiteral("%1").arg(100-hmax));
        ui->label_3->setText(QStringLiteral("%1").arg(hmin));
        break;
    }
    }
}

void MainWindow::on_radioButton_clicked()
{
    chanel=1;
    setRange();
}

void MainWindow::on_radioButton_2_clicked()
{
    chanel=2;
    setRange();
}

void MainWindow::on_radioButton_3_clicked()
{
    chanel=3;
    setRange();
}

void MainWindow::on_verticalSlider_sliderReleased()
{
 //value min
    if(!cvImg_host.data)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("pleas load an image"));
        msgBox.exec();
        return;
    }
    setValue(1);
    callKernel();
    QImg_bin = QImage((const unsigned char*)(cvImg_out.data),w,h,cvImg_out.step,  QImage::Format_Indexed8);
    ui->label_2->clear();
    ui->label_2->setPixmap(QPixmap::fromImage(QImg_bin));
    ui->label_2->resize(ui->label_2->pixmap()->size());
}




void MainWindow::on_verticalSlider_valueChanged(int value)
{

 //value min

    ui->label_3->setText(QStringLiteral("%1").arg(value));
    if(value>(100-ui->verticalSlider_2->value()))
    {
        value=100-ui->verticalSlider_2->value();
        ui->verticalSlider->setValue(value);
        ui->label_3->setText("***");
    }


}


void MainWindow::on_verticalSlider_2_valueChanged(int value)
{ //max
    if(!cvImg_in.data)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("please load an image"));
        msgBox.exec();
        return;
    }
    value=100-value;
    ui->label_4->setText(QStringLiteral("%1").arg(value));
    //value min
    if(value < ui->verticalSlider->value())
    {
        value=100-ui->verticalSlider->value();
        ui->verticalSlider->setValue(value);
        ui->label_4->setText(tr("***"));
    }


}

void MainWindow::on_verticalSlider_2_sliderReleased()
{
    if(!cvImg_host.data)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("pleas load an image"));
        msgBox.exec();
        return;
    }
    //max
    setValue(2);
    callKernel();
    QImg_bin = QImage((const unsigned char*)(cvImg_out.data),w,h,cvImg_out.step,  QImage::Format_Indexed8);
    ui->label_2->clear();
    ui->label_2->setPixmap(QPixmap::fromImage(QImg_bin));
    ui->label_2->resize(ui->label_2->pixmap()->size());
}
