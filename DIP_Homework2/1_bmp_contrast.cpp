/**************************************************************************/
// FILE NAME: 1_bmp_contrast.cpp
// VERSRION: 1.0
// DATE: Oct 18, 2022
// AUTHOR: Kuan-Wei Chen / NYCU IEE Oasis Lab / 310510221
// CODE TYPE: CPP
// DESCRIPTION: 2022 Fall Digital Image Procssing / Homework2
// MODIFICATION HISTORY: 
// Date                 Description
//
/**************************************************************************/

#include <stdio.h>
#include <iostream>

#include <string.h>
#include <math.h>
#include <time.h>
#include <direct.h>

#include "stdlib.h"
#include "bitmap.h"
#include "processing.h"

#define BM 0x4d42

using namespace std;

WORD bfType;
BMPFILEHEADER FileHeader;
BMPINFOHEADER InfoHeader;

FILE *fin, *fout1, *fout2;

int width, height, channel;
int size;

double start_time, end_time; 

double gamma = 0.5;

// Show bitmap file header
void showBMPHead(BMPFILEHEADER BMPHead){
	cout << "===== File Header =====" << endl;
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


// Histogram Equalization
bool HE(FILE *fout, RGB** srcImg){
	// Start timer
	start_time = clock();
	
	channel = (InfoHeader.biBitCount == 24) ? 3 : 4;
	
	double sum = height * width;
	
	int hist_b[256] = {0};	
	int hist_g[256] = {0};
	int hist_r[256] = {0};
	int hist_a[256] = {0};
	
	double prob_b[256] = {0};
	double prob_g[256] = {0};
	double prob_r[256] = {0};
	double prob_a[256] = {0};
	
	double cdf_b[256] = {0};
	double cdf_g[256] = {0};
	double cdf_r[256] = {0};
	double cdf_a[256] = {0};
	
	int equal_b[256] = {0};
	int equal_g[256] = {0};
	int equal_r[256] = {0};
	int equal_a[256] = {0};
	
	
	// Create image data
	RGB** dstImg = new RGB*[height];
	for(int i = 0; i < height; i++){
		dstImg[i] = new RGB[width];
	}
	
	
	// Count the number of different levels of each channel
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			hist_b[srcImg[i][j].b] += 1;
			hist_g[srcImg[i][j].g] += 1;
			hist_r[srcImg[i][j].r] += 1;
			if(channel == 4){
				hist_a[srcImg[i][j].a] += 1;
			}
		}
	}
	
	/*
	cout << "Histogram: " << endl;
	for(int k = 0; k < 256; k++){
		cout << "b " << "level " << k << ": " << hist_b[k] << endl;
		cout << "g " << "level " << k << ": " << hist_g[k] << endl;
		cout << "r " << "level " << k << ": " << hist_r[k] << endl;
		if(channel == 4){
			cout << "a " << "level " << k << ": " << hist_a[k] << endl;
		}
		cout << "=====================" << endl;
	}
	cout << endl;
	*/
	
	// Calculate frequency
	for(int k = 0; k < 256; k++){
		prob_b[k] = (hist_b[k] / sum);
		prob_g[k] = (hist_g[k] / sum);
		prob_r[k] = (hist_r[k] / sum);
		if(channel == 4){
			prob_a[k] = (hist_a[k] / sum);
		}
	}
	
	/*
	cout << "Probability: " << endl;
	for(int k = 0; k < 256; k++){
		cout << "b " << "prob " << k << ": " << prob_b[k] << endl;
		cout << "g " << "prob " << k << ": " << prob_g[k] << endl;
		cout << "r " << "prob " << k << ": " << prob_r[k] << endl;
		if(channel == 4){
			cout << "a " << "prob " << k << ": " << prob_a[k] << endl;
		}
		cout << "=====================" << endl;
	}
	cout << endl;
	*/
	
	// Cumulative Distribution Function (CDF)
	cdf_b[0] = prob_b[0];
	cdf_g[0] = prob_g[0];
	cdf_r[0] = prob_r[0];
	if(channel == 4){
		cdf_a[0] = prob_a[0];
	}
	
	for(int k = 1; k < 256; k++){
		cdf_b[k] = cdf_b[k-1] + prob_b[k];
		cdf_g[k] = cdf_g[k-1] + prob_g[k];
		cdf_r[k] = cdf_r[k-1] + prob_r[k];
		if(channel == 4){
			cdf_a[k] = cdf_a[k-1] + prob_a[k];
		}
	}
	
	/*
	cout << "CDF: " << endl;
	for(int k = 0; k < 256; k++){
		cout << "b " << "cdf " << k << ": " << cdf_b[k] << endl;
		cout << "g " << "cdf " << k << ": " << cdf_g[k] << endl;
		cout << "r " << "cdf " << k << ": " << cdf_r[k] << endl;
		if(channel == 4){
			cout << "a " << "cdf " << k << ": " << cdf_a[k] << endl;
		}
		cout << "=====================" << endl;
	}
	cout << endl;
	*/
	
	// Histogram Equalize
	for(int k = 0; k < 256; k++){
		equal_b[k] = (unsigned char)(255 * cdf_b[k] + 0.5);
		equal_g[k] = (unsigned char)(255 * cdf_g[k] + 0.5);
		equal_r[k] = (unsigned char)(255 * cdf_r[k] + 0.5);
		if(channel == 4){
			equal_a[k] = (unsigned char)(255 * cdf_a[k] + 0.5);
		}
	}
	
	/*
	cout << "HE: " << endl;
	for(int k = 0; k < 256; k++){
		cout << "b " << "HE " << k << ": " << equal_b[k] << endl;
		cout << "g " << "HE " << k << ": " << equal_g[k] << endl;
		cout << "r " << "HE " << k << ": " << equal_r[k] << endl;
		if(channel == 4){
			cout << "a " << "HE " << k << ": " << equal_a[k] << endl;
		}
		cout << "=====================" << endl;
	}
	cout << endl;
	*/
	
	// Mapping
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			dstImg[i][j].b = equal_b[srcImg[i][j].b];
			dstImg[i][j].g = equal_g[srcImg[i][j].g];
			dstImg[i][j].r = equal_r[srcImg[i][j].r];
			if(channel == 4){
				dstImg[i][j].a = equal_a[srcImg[i][j].a];
			}
		}
	}
	
	
	// Write bitmap file header
	WORD bfType_w = BM;
	fwrite(&bfType_w, 1, sizeof(WORD), fout);
	fwrite(&FileHeader, 1, sizeof(BMPFILEHEADER), fout);

	// Write bitmap info header
	fwrite(&InfoHeader, 1, sizeof(BMPINFOHEADER), fout);
	
	// Sets the file position indicator for the file stream stream to the value pointed to by offset
	fseek(fout, FileHeader.bfOffBits, 0);
	
	for(int i = 0; i < height; i++){
		for(int j=0; j < width; j++){
			fwrite(&dstImg[i][j], channel, 1, fout);
		}
	}
	
	// Destruction
	for (int i = 0; i < height; i++){
		delete[] dstImg[i]; // Delete columns
	}
	delete[] dstImg; // Delete Rows
	

	end_time = clock();
	cout << "Processing time: "<< (end_time - start_time) / CLOCKS_PER_SEC << " s" << endl;
	return true;
}


// Gamma Transformation
bool GammaTran(FILE *fout, RGB** srcImg, double gamma){
	// Start timer
	start_time = clock();
	
	channel = (InfoHeader.biBitCount == 24) ? 3 : 4;
	
	
	double inv_gamma = 1. / gamma;
	
	
	// Create image data
	RGB** dstImg = new RGB*[height];
	for(int i = 0; i < height; i++){
		dstImg[i] = new RGB[width];
	}
	
	
	
	// Mapping
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			
			dstImg[i][j].b = (unsigned char)(pow( double(srcImg[i][j].b) / 255., inv_gamma) * 255.);
			dstImg[i][j].g = (unsigned char)(pow( double(srcImg[i][j].g) / 255., inv_gamma) * 255.);
			dstImg[i][j].r = (unsigned char)(pow( double(srcImg[i][j].r) / 255., inv_gamma) * 255.);
			

			if(channel == 4){
				
				dstImg[i][j].a = (unsigned char)(pow( double(srcImg[i][j].a) / 255., inv_gamma) * 255.);
				
			}
		}
	}

	
	// Write bitmap file header
	WORD bfType_w = BM;
	fwrite(&bfType_w, 1, sizeof(WORD), fout);
	fwrite(&FileHeader, 1, sizeof(BMPFILEHEADER), fout);

	// Write bitmap info header
	fwrite(&InfoHeader, 1, sizeof(BMPINFOHEADER), fout);
	
	// Sets the file position indicator for the file stream stream to the value pointed to by offset
	fseek(fout, FileHeader.bfOffBits, 0);
	
	for(int i = 0; i < height; i++){
		for(int j=0; j < width; j++){
			fwrite(&dstImg[i][j], channel, 1, fout);
		}
	}
	
	// Destruction
	for (int i = 0; i < height; i++){
		delete[] dstImg[i]; // Delete columns
	}
	delete[] dstImg; // Delete Rows
	

	end_time = clock();
	cout << "Processing time: "<< (end_time - start_time) / CLOCKS_PER_SEC << " s" << endl;
	return true;
}
	

// main function
int main(){
	// Maker directory
	mkdir("./1_contrast");
	
	// Open bitmap file
	fin  = fopen("./input1.bmp","rb");
	fout1 = fopen("./1_contrast/output1_1.bmp","wb");
	fout2 = fopen("./1_contrast/output1_2.bmp","wb");
	
	
	if(fin != NULL){
		cout << "Loading the file..." << endl;
		fread(&bfType, 1, sizeof(WORD), fin);
		
		// Check if the file is bmp file
		if(bfType != BM){
			cout << "The file is not a bmp file!" << endl;
			return 0;
		}
		
		// Read bitmap file header
		fread(&FileHeader, sizeof(BMPFILEHEADER), 1, fin);
		showBMPHead(FileHeader);
		
		// Read bitmap info header
		fread(&InfoHeader, sizeof(BMPINFOHEADER), 1, fin);
		showBMPInfoHead(InfoHeader);
		
		// Sets the file position indicator for the file stream stream to the value pointed to by offset
		fseek(fin, FileHeader.bfOffBits, 0);
		
		// Basic information
		height  = InfoHeader.biHeight;
		width   = InfoHeader.biWidth;
		size    = height * width;
		channel = (InfoHeader.biBitCount == 24) ? 3 : 4;
		
		// Create image data
		RGB** img = new RGB*[height];
		for(int i = 0; i < height; i++){
			img[i] = new RGB[width];
		}
		
		for(int i = 0; i < height; i++){
			for(int j = 0; j < width; j++){
				if(channel == 3){
					fread(&img[i][j], channel, 1, fin);
				}
				else if(channel == 4){
					fread(&img[i][j], channel, 1, fin);
				}
				// cout << (int)(img[i][j].b) << ", " << (int)(img[i][j].g) << ", " << (int)(img[i][j].r) <<endl;
			}
		}
		
		// Image Processing
		cout << "== Image Processing ==" << endl;
		cout << "Contrast Enhancement Start..." << endl << endl;
		
		// Gamma Transformation
		cout << "[output1]" << endl << endl;
		if(GammaTran(fout1, img, gamma)){
			cout << endl;
			cout << "Gamma Transformation Success!" << endl;
		}
		else{
			cout << endl;
			cout << "Gamma Transformation Failed!" << endl;
			return 0;
		}	
		
		cout << "======================" << endl;
		
		// Histogram Equalization
		cout << "[output2]" << endl << endl;
		if(HE(fout2, img)){
			cout << endl;
			cout << "Histogram Equalization Success!" << endl;
		}
		else{
			cout << endl;
			cout << "Histogram Equalization Success!" << endl;
			return 0;
		}
		
		cout << endl;
		
		// Destruction
		for (int i = 0; i < height; i++){
			delete[] img[i]; // Delete columns
		}
		delete[] img; // Delete Rows
		
		// Closed the input file
		cout << "Closed the input file..." << endl;
		fclose(fin);
		
		// Closed the output file
		cout << "Closed the output file..." << endl;
		fclose(fout1);
		fclose(fout2);
	}
	else{
		cout << "File open error!" << endl;
		return 0;
	}
	
	return 0;
}












