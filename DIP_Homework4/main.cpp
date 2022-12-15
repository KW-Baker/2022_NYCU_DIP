/**************************************************************************/
// FILE NAME: main.cpp
// VERSRION: 1.0
// DATE: Dec 10, 2022
// AUTHOR: Kuan-Wei Chen / NYCU IEE Oasis Lab / 310510221
// CODE TYPE: CPP
// DESCRIPTION: 2022 Fall Digital Image Procssing / Homework4
// MODIFICATION HISTORY: 
// Date                 Description
//
/**************************************************************************/
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

void calcPSF(Mat& outputImg, Size filterSize, int len1, int len2, int len3, int len4, double theta, int R1, int R2);
void fftshift(const Mat& inputImg, Mat& outputImg);
void filter2DFreq(const Mat& inputImg, Mat& outputImg, const Mat& H);
void calcWnrFilter(const Mat& input_h_PSF, Mat& output_G, double nsr);


int main(int argc, char *argv[]){
    // Hint LEN1 > LEN2 > LEN3 > LEN4
    int LEN1_1 = 90;
    int LEN1_2 = 90;
    int LEN1_3 = 10;
    int LEN1_4 = 1;
    double THETA1 = 140;
    int R1_1 = 0;
    int R1_2 = 3;
    int snr1 = 4;


    int LEN2_1 = 90;
    int LEN2_2 = 90;
    int LEN2_3 = 10;
    int LEN2_4 = 1;
    double THETA2 = 140;
    int R2_1 = 0;
    int R2_2 = 4;
    int snr2 = 100;

    //int snr = 4;
    
    

    string str_input1 = "C:\\Project\\DIP\\DIPHW4\\input1.bmp";
    string str_input2 = "C:\\Project\\DIP\\DIPHW4\\input2.bmp";

    Mat img1, img2;

    img1 = imread(str_input1);
    if(img1.empty()) { //check whether the image is loaded or not
        cout << "ERROR : Image1 cannot be loaded..!!" << endl;
        return -1;
    }

    img2 = imread(str_input2);
    if(img2.empty()) { //check whether the image is loaded or not
        cout << "ERROR : Image2 cannot be loaded..!!" << endl;
        return -1;
    }


    vector<Mat> img1_channels;
    vector<Mat> imgOut1_channels;
    Mat imgOut1;

    split(img1, img1_channels);
    split(img1, imgOut1_channels);


    vector<Mat> img2_channels;
    vector<Mat> imgOut2_channels;
    Mat imgOut2;

    split(img2, img2_channels);
    split(img2, imgOut2_channels);
    

    // it needs to process even image only
    Rect img1_roi_b = Rect(0, 0, img1_channels[0].cols & -2, img1_channels[0].rows & -2);
    Rect img1_roi_g = Rect(0, 0, img1_channels[1].cols & -2, img1_channels[1].rows & -2);
    Rect img1_roi_r = Rect(0, 0, img1_channels[2].cols & -2, img1_channels[2].rows & -2);

    Rect img2_roi_b = Rect(0, 0, img2_channels[0].cols & -2, img2_channels[0].rows & -2);
    Rect img2_roi_g = Rect(0, 0, img2_channels[1].cols & -2, img2_channels[1].rows & -2);
    Rect img2_roi_r = Rect(0, 0, img2_channels[2].cols & -2, img2_channels[2].rows & -2);


    //Hw calculation (start)
    Mat Hw1, h1;
    calcPSF(h1, img1_roi_b.size(), LEN1_1, LEN1_2, LEN1_3, LEN1_4, THETA1, R1_1, R1_2);
    calcPSF(h1, img1_roi_g.size(), LEN1_1, LEN1_2, LEN1_3, LEN1_4, THETA1, R1_1, R1_2);
    calcPSF(h1, img1_roi_r.size(), LEN1_1, LEN1_2, LEN1_3, LEN1_4, THETA1, R1_1, R1_2);

    calcWnrFilter(h1, Hw1, 1.0 / double(snr1));

    Mat Hw2, h2;
    calcPSF(h2, img2_roi_b.size(), LEN2_1, LEN2_2, LEN2_3, LEN2_4, THETA2, R2_1, R2_2);
    calcPSF(h2, img2_roi_g.size(), LEN2_1, LEN2_2, LEN2_3, LEN2_4, THETA2, R2_1, R2_2);
    calcPSF(h2, img2_roi_r.size(), LEN2_1, LEN2_2, LEN2_3, LEN2_4, THETA2, R2_1, R2_2);

    calcWnrFilter(h2, Hw2, 1.0 / double(snr2));

    
    
    // Convert
    img1_channels[0].convertTo(img1_channels[0], CV_32F);
    img1_channels[1].convertTo(img1_channels[1], CV_32F);
    img1_channels[2].convertTo(img1_channels[2], CV_32F);

    img2_channels[0].convertTo(img2_channels[0], CV_32F);
    img2_channels[1].convertTo(img2_channels[1], CV_32F);
    img2_channels[2].convertTo(img2_channels[2], CV_32F);


    // filtering
    filter2DFreq(img1_channels[0](img1_roi_b), imgOut1_channels[0], Hw1);
    filter2DFreq(img1_channels[1](img1_roi_g), imgOut1_channels[1], Hw1);
    filter2DFreq(img1_channels[2](img1_roi_r), imgOut1_channels[2], Hw1);

    filter2DFreq(img2_channels[0](img2_roi_b), imgOut2_channels[0], Hw2);
    filter2DFreq(img2_channels[1](img2_roi_g), imgOut2_channels[1], Hw2);
    filter2DFreq(img2_channels[2](img2_roi_r), imgOut2_channels[2], Hw2);


    imgOut1_channels[0].convertTo(imgOut1_channels[0], CV_8U);
    imgOut1_channels[1].convertTo(imgOut1_channels[1], CV_8U);
    imgOut1_channels[2].convertTo(imgOut1_channels[2], CV_8U);

    imgOut2_channels[0].convertTo(imgOut2_channels[0], CV_8U);
    imgOut2_channels[1].convertTo(imgOut2_channels[1], CV_8U);
    imgOut2_channels[2].convertTo(imgOut2_channels[2], CV_8U);

    normalize(imgOut1_channels[0], imgOut1_channels[0], 0, 255, NORM_MINMAX);
    normalize(imgOut1_channels[1], imgOut1_channels[1], 0, 255, NORM_MINMAX);
    normalize(imgOut1_channels[2], imgOut1_channels[2], 0, 255, NORM_MINMAX);

    normalize(imgOut2_channels[0], imgOut2_channels[0], 0, 255, NORM_MINMAX);
    normalize(imgOut2_channels[1], imgOut2_channels[1], 0, 255, NORM_MINMAX);
    normalize(imgOut2_channels[2], imgOut2_channels[2], 0, 255, NORM_MINMAX);

    merge(imgOut1_channels, imgOut1);
    merge(imgOut2_channels, imgOut2);
    
    
    medianBlur(imgOut1, imgOut1, 3);
    // medianBlur(imgOut2, imgOut2, 3);
    

    // Mat sharpening_kernel = (Mat_<double>(3, 3) <<  -1, -1, -1,
    //                                                 -1,  9, -1,
    //                                                 -1, -1, -1);
    Mat sharpening_kernel = (Mat_<double>(3, 3) <<   0, -1,  0,
                                                    -1,  5, -1,
                                                     0, -1,  0);

    Mat gaussain_kernel = (Mat_<double>(3, 3) <<    0.0625, 0.1250,  0.0625,
                                                    0.1250, 0.2500,  0.1250,
                                                    0.0625, 0.1250,  0.0625);

    filter2D(imgOut1, imgOut1, -1, gaussain_kernel);
    filter2D(imgOut2, imgOut2, -1, gaussain_kernel);
   

    // filter2D(imgOut1, imgOut1, -1, sharpening_kernel);
    filter2D(imgOut2, imgOut2, -1, sharpening_kernel);
    filter2D(imgOut2, imgOut2, -1, sharpening_kernel);
    

    imwrite("C:\\Project\\DIP\\DIPHW4\\output1.bmp", imgOut1);
    imwrite("C:\\Project\\DIP\\DIPHW4\\output2.bmp", imgOut2);


    // waitKey(0);
    return 0;
}


void calcPSF(Mat& outputImg, Size filterSize, int len1, int len2, int len3, int len4, double theta, int R1, int R2){
    Mat h(filterSize, CV_32F, Scalar(0));
    Point point(filterSize.width / 2, filterSize.height / 2);
    ellipse(h, point, Size(0, cvRound(float(len1) / 2.0)), 90.0 - theta, 0, 360, Scalar(63), FILLED);
    ellipse(h, point, Size(0, cvRound(float(len2) / 2.0)), 90.0 - theta, 0, 360, Scalar(127), FILLED);
    ellipse(h, point, Size(0, cvRound(float(len3) / 2.0)), 90.0 - theta, 0, 360, Scalar(191), FILLED);
    ellipse(h, point, Size(0, cvRound(float(len4) / 2.0)), 90.0 - theta, 0, 360, Scalar(255), FILLED);
    circle(h, point, R1, Scalar(0), -1, 8);
    circle(h, point, R2, Scalar(255), -1, 8);

    Scalar summa = sum(h);
    outputImg = h / summa[0];

    // h.convertTo(h, CV_8U);
    // imwrite("C:\\Project\\DIP\\DIPHW4\\psf.bmp", h);
}

void fftshift(const Mat& inputImg, Mat& outputImg){
    outputImg = inputImg.clone();
    int cx = outputImg.cols / 2;
    int cy = outputImg.rows / 2;
    Mat q0(outputImg, Rect(0, 0, cx, cy));
    Mat q1(outputImg, Rect(cx, 0, cx, cy));
    Mat q2(outputImg, Rect(0, cy, cx, cy));
    Mat q3(outputImg, Rect(cx, cy, cx, cy));
    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

void filter2DFreq(const Mat& inputImg, Mat& outputImg, const Mat& H){
    Mat planes[2] = { Mat_<float>(inputImg.clone()), Mat::zeros(inputImg.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);
    dft(complexI, complexI, DFT_SCALE);
    Mat planesH[2] = { Mat_<float>(H.clone()), Mat::zeros(H.size(), CV_32F) };
    Mat complexH;
    merge(planesH, 2, complexH);
    Mat complexIH;
    mulSpectrums(complexI, complexH, complexIH, 0);
    idft(complexIH, complexIH);
    split(complexIH, planes);
    outputImg = planes[0];
}

void calcWnrFilter(const Mat& input_h_PSF, Mat& output_G, double nsr){
    Mat h_PSF_shifted;
    fftshift(input_h_PSF, h_PSF_shifted);
    Mat planes[2] = { Mat_<float>(h_PSF_shifted.clone()), Mat::zeros(h_PSF_shifted.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);
    dft(complexI, complexI);
    split(complexI, planes);
    Mat denom;
    pow(abs(planes[0]), 2, denom);
    denom += nsr;
    divide(planes[0], denom, output_G);
}
