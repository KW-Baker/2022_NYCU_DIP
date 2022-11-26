/**************************************************************************/
// FILE NAME: 5_bmp_IE_gamma.cpp
// VERSRION: 1.0
// DATE: Nov 26, 2022
// AUTHOR: Kuan-Wei Chen / NYCU IEE Oasis Lab / 310510221
// CODE TYPE: CPP
// DESCRIPTION: 2022 Fall Digital Image Procssing / Homework3
// MODIFICATION HISTORY: 
// Date                 Description
//
/**************************************************************************/
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <direct.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <string>

#include "stdlib.h"
#include "bitmap.h"


#define BM 0x4d42
using namespace std;

WORD bfType;
BMPFILEHEADER FileHeader;
BMPINFOHEADER InfoHeader;

FILE *fin, *fout;

string in_fileName, out_fileName;
string in_filePath, out_filePath;

int width, height, channel;
int size;

double start_time, end_time; 

double gamma = 0.8;

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

// Padding(read bmp)
void readPadding(FILE *fp, int padding){
	for(int i = 0; i < padding; i++)
		fgetc(fp);
}

// Padding(write bmp)
void writePadding(FILE *fp, int padding){
	for(int i = 0; i < padding; i++)
		fputc(0, fp);
}

// Write bmp
void writeBmp(FILE *fout, RGB** img, int padding){
	// Write bitmap file header
	WORD bfType_w = BM;
	fwrite(&bfType_w, 1, sizeof(WORD), fout);
	fwrite(&FileHeader, 1, sizeof(BMPFILEHEADER), fout);

	// Write bitmap info header
	fwrite(&InfoHeader, 1, sizeof(BMPINFOHEADER), fout);
	
	// Sets the file position indicator for the file stream stream to the value pointed to by offset
	fseek(fout, FileHeader.bfOffBits, 0);
	
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			fwrite(&img[i][j], channel, 1, fout);
		}
		writePadding(fout, padding);
	}
}

// Gamma Transformation
RGB** GammaTran(RGB** srcImg, double gamma){
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
			dstImg[i][j].a = (unsigned char)(0);
				
			if(channel == 4)
				dstImg[i][j].a = (unsigned char)(pow( double(srcImg[i][j].a) / 255., inv_gamma) * 255.);
		}
	}

	end_time = clock();
	cout << "Processing time: "<< (end_time - start_time) / CLOCKS_PER_SEC << " s" << endl;
	return dstImg;
}




// main function
int main(int argc, char **argv){
	// cout << "Please enter the input bmp file name: " << endl;
	// cin >> in_fileName;
	// cout << "Please enter the output bmp file name: " << endl;
	// cin >> out_fileName;
	
	// in_filePath.append("./").append(in_fileName);
	// out_filePath.append("./").append(out_fileName);
	
	// Open bitmap file
	fin  = fopen("output4_1.bmp","rb");
	fout = fopen("output4_2.bmp","wb");
	
	
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
		
		
		// int padding = (width * 3) % 4;
		int padding = (4 - (width * channel) % 4) % 4;



		// Create image data
		RGB** img = new RGB*[height];
		for(int i = 0; i < height; i++){
			img[i] = new RGB[width];
		}
		
		for(int i = 0; i < height; i++){
			for(int j = 0; j < width; j++){
				fread(&img[i][j], channel, 1, fin);
				// cout << (int)(img[i][j].b) << ", " << (int)(img[i][j].g) << ", " << (int)(img[i][j].r) <<endl;
			}
			readPadding(fin, padding);
		}



		RGB** tmp_img = new RGB*[height];
		for(int i = 0; i < height; i++){
			tmp_img[i] = new RGB[width];
		}
		
		// Image Processing
		cout << "== Image Processing ==" << endl;
		cout << "Contrast Enhancement Start..." << endl << endl;
		
		// Gamma Transformation
		cout << "[output]" << endl << endl;

		tmp_img = GammaTran(img, gamma);
		
		writeBmp(fout, tmp_img, padding);


		cout << "======================" << endl;
		
		
		
		// Closed the input file
		cout << "Closed the input file..." << endl;
		fclose(fin);
		
		// Closed the output file
		cout << "Closed the output file..." << endl;
		fclose(fout);
	}
	else{
		cout << "File open error!" << endl;
		return 0;
	}
	
	return 0;
}
