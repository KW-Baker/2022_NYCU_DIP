/**************************************************************************/
// FILE NAME: psnr.cpp
// VERSRION: 1.0
// DATE: Dec 1, 2022
// AUTHOR: Kuan-Wei Chen / NYCU IEE Oasis Lab / 310510221
// CODE TYPE: CPP
// DESCRIPTION: 2022 Fall Digital Image Procssing / Homework4
// MODIFICATION HISTORY: 
// Date                 Description
//
// ReadMe
// g++ -std=c++11 psnr.cpp -o psnr.o
// ./psnr.o image1_ori.bmp output1.bmp
/**************************************************************************/

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>

#include "bitmap.h"
using namespace std;


WORD bfType;


BMPFILEHEADER FileHeader;
BMPINFOHEADER InfoHeader;

FILE *fin1, *fin2;

vector<vector<RGB>> img1, img2;
string IN1_PATH, IN2_PATH;

int width, height, channel;
int size;
int padding = 0;


void showBMPHead(BMPFILEHEADER BMPHead){
	cout << "===== File Header =====" << endl;
	cout << "Type: "       << BMPHead.bfType << endl; 
	cout << "File size: "  << BMPHead.bfSize << endl;
	cout << "Reserved 1: " << BMPHead.bfReserved1 << endl;
	cout << "Reserved 2: " << BMPHead.bfReserved2 << endl;
	cout << "OffBits: "    << BMPHead.bfOffBits   << endl << endl;	
}

// Show bitmap info header
void showBMPInfoHead(BMPINFOHEADER BMPInfoHead){
	cout << "===== Info Header =====" << endl;
	cout << "Size of Info Header: "   << BMPInfoHead.biSize <<endl;
	cout << "Bitmap Width: "          << BMPInfoHead.biWidth <<endl;
	cout << "Bitmap Height: "         << BMPInfoHead.biHeight <<endl;
	cout << "Planes: "                << BMPInfoHead.biPlanes << endl;
	cout << "BitCount: "              << BMPInfoHead.biBitCount << endl;
	cout << "Compression: "           << BMPInfoHead.biCompression << endl;
	cout << "Image Size: "            << BMPInfoHead.biSizeImage << endl;
	cout << "XpixelsPerM: "           << BMPInfoHead.biXPelsPerMeter << endl;
	cout << "YpixelsPerM: "           << BMPInfoHead.biYPelsPerMeter << endl;
	cout << "ColorUsed: "             << BMPInfoHead.biClrUsed << endl;
	cout << "ColorImportant: "        << BMPInfoHead.biClrImportant << endl << endl;
}

// Padding(read bmp)
void readPadding(FILE *fp, int padding){
	for(int i = 0; i < padding; i++)
		fgetc(fp);
}


vector<vector<RGB>> readBMP(FILE *fin, vector<vector<RGB>> img){
	// Read bitmap file header
	fread(&FileHeader, sizeof(BMPFILEHEADER), 1, fin);
	//showBMPHead(FileHeader);
	
	// Read bitmap info header
	fread(&InfoHeader, sizeof(BMPINFOHEADER), 1, fin);
	//showBMPInfoHead(InfoHeader);
	
	// Sets the file position indicator for the file stream stream to the value pointed to by offset
	fseek(fin, FileHeader.bfOffBits, 0);
	
	// Basic information
	height  = InfoHeader.biHeight;
	width   = InfoHeader.biWidth;
	size    = height * width;
	channel = (InfoHeader.biBitCount == 24) ? 3 : 4;
	
	padding = (4 - (width * channel) % 4) % 4;

	// Initialize 2D vector
	img.resize(height);
	for(auto i = 0; i < height; i++)
		img[i].resize(height);

	for(auto i = 0; i < height; i++)
		for(auto j = 0; j < width; j++)
			img[i][j] = {0,0,0,0};
		
	for(auto i = 0; i < height; i++){
		for(auto j = 0; j < width; j++)
			fread(&img[i][j], channel, 1, fin);
		readPadding(fin, padding);
	}
	return img;
}


vector<double> MSE(vector<vector<RGB>> img1, vector<vector<RGB>> img2){
	vector<double> mse;
	double mse_r, mse_g, mse_b = 0.0;
	double N = height * width;

	for(auto i = 0; i < height; i++){
		for(auto j = 0; j < width; j++){
			mse_r += pow(img1[i][j].r - img2[i][j].r, 2);
			mse_g += pow(img1[i][j].g - img2[i][j].g, 2);
			mse_b += pow(img1[i][j].b - img2[i][j].b, 2);
		}
	}
		
	mse_r = mse_r / N;
	mse_g = mse_g / N;
	mse_b = mse_b / N;


	mse.push_back(mse_r);
	mse.push_back(mse_g);
	mse.push_back(mse_b);

	return mse;
}
double PSNR(vector<double> mse){
	double psnr = 0.0;

	for(auto i = 0; i < 3; i++)
		psnr += 10 * log10((255 * 255) / mse[i]);

	return psnr;
}


int main(int argc, char **argv){
	vector<double> mse;
	double psnr;

	IN1_PATH = argv[1];
	IN2_PATH = argv[2];

	cout << "Image 1 path: " << IN1_PATH << endl;
	cout << "Image 2 path: " << IN2_PATH << endl; 

	fin1 = fopen(IN1_PATH.c_str(), "rb");
	fin2 = fopen(IN2_PATH.c_str(), "rb");
	img1 = readBMP(fin1, img1);
	img2 = readBMP(fin2, img2);

	
	cout << "========= MSE =========" << endl;
	mse = MSE(img1, img2);
	cout << "MSE_r = " << mse[0] << endl;
	cout << "MSE_g = " << mse[1] << endl;
	cout << "MSE_b = " << mse[2] << endl;

	cout << "========= PSNR ========" << endl;
	psnr = PSNR(mse);
	cout << "PSNR = " << psnr << " dB"<<endl;


	return 0;
}