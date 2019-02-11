#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

//interface of CUDA, using CUDA to create  binary image data depends on green level of RGB image
//HSV color map is used
extern "C" void binGreen(unsigned char*indata_h, unsigned char*outdata_h,int w, int h,
    float hL=70, float hH=160, float sL=43, float sH=255, float vL=46, float vH=255);

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void resetBand(); //reset the threshold of green level
    void callKernel(); // call the CUDA interface fonction, for preview image data only
    void setRange();  //set range of verticalSlider
    void setValue(int mark); // set threshold values

    void on_pushButton_clicked(); //image load button clicked, changing image data

    void on_pushButton_2_clicked(); // save button clicked

    void on_pushButton_3_clicked(); // reset value button clicked, call resetband()

    void on_radioButton_clicked(); // choose threshold value (H chanel of HSV)

    void on_radioButton_2_clicked(); // choose threshold value (S chanel of HSV)

    void on_radioButton_3_clicked();// choose threshold value (V chanel of HSV)

    void on_verticalSlider_sliderReleased(); // min slider released call setValue(), change preview image too

    void on_verticalSlider_valueChanged(int value);// min slider value changed for display on mianwindow

    void on_verticalSlider_2_valueChanged(int value); // max slider value changed for display on mianwindow

    void on_verticalSlider_2_sliderReleased(); // max slider released call setValue(), change preview image too

private:
    Ui::MainWindow *ui;
    cv::Mat cvImg_in,cvImg_host,cvImg_out; // OpenCv image data, cvImg_host used for preview image
    QImage QImg_rgb,QImg_bin; // QT image data for display
    //threshold values ( for all green hL = 60,  hH = 180,  sL = 43, sH = 255,  vL = 46,  vH = 255)
    int hL,hH,sL,sH,vL,vH;
    int frameSizeout,imageSize;//size data of image
    int h,w; // size data of image
    int chanel; // HSV chanel pointer
};

#endif // MAINWINDOW_H
