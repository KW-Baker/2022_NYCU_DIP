/**************************************************************************/
// FILE NAME: 4_bmp_IE_denoise.cpp
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


RGB** denoise(RGB** srcImg, int iteration){
	// Start timer
	start_time = clock();
	channel = (InfoHeader.biBitCount == 24) ? 3 : 4;
	
	int tmpHeight;
	int tmpWidth;
	double pixel_b, pixel_g, pixel_r, pixel_a;	
	
	// Guassian kernel
	int kernel_size = 3;
	float Gkernel[kernel_size][kernel_size];
	
	Gkernel[0][0] = 1/16.;
	Gkernel[0][1] = 1/8.;
	Gkernel[0][2] = 1/16.;
	Gkernel[1][0] = 1/8.;
	Gkernel[1][1] = 1/4.;
	Gkernel[1][2] = 1/8.;
	Gkernel[2][0] = 1/16.;
	Gkernel[2][1] = 1/8.;
	Gkernel[2][2] = 1/16.;
	
	cout << "Guassian kernel: " << endl;
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			printf("%4f  ", Gkernel[i][j]);
		}
		cout << endl << endl;
	}
	
	// tmpImg height & width
	tmpHeight = height + 2;
	tmpWidth  = width  + 2;
	
	// Create image data
	RGB** dstImg = new RGB*[height];
	for(int i = 0; i < height; i++){
		dstImg[i] = new RGB[width];
	}
	
	RGB** tmpImg = new RGB*[tmpHeight];
	for(int i = 0; i < tmpHeight; i++){
		tmpImg[i] = new RGB[tmpWidth];
	}
	
	// Copy srcImg to dstImg
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			dstImg[i][j].b = srcImg[i][j].b;
			dstImg[i][j].g = srcImg[i][j].g;
			dstImg[i][j].r = srcImg[i][j].r;
			
			if(channel == 4){
				dstImg[i][j].a = srcImg[i][j].a;
			}
		}
	}
	
	// Guassian blur
	for(int iter = 0; iter < iteration; iter++){
		// Replcation Padding
		tmpImg[0][0].b = dstImg[0][0].b;
		tmpImg[0][0].g = dstImg[0][0].g;
		tmpImg[0][0].r = dstImg[0][0].r;
		
		tmpImg[0][tmpWidth-1].b = dstImg[0][width-1].b;
		tmpImg[0][tmpWidth-1].g = dstImg[0][width-1].g;
		tmpImg[0][tmpWidth-1].r = dstImg[0][width-1].r;
		
		tmpImg[tmpHeight-1][0].b = dstImg[height-1][0].b;
		tmpImg[tmpHeight-1][0].g = dstImg[height-1][0].g;
		tmpImg[tmpHeight-1][0].r = dstImg[height-1][0].r;
		
		tmpImg[tmpHeight-1][tmpWidth-1].b = dstImg[height-1][width-1].b;
		tmpImg[tmpHeight-1][tmpWidth-1].g = dstImg[height-1][width-1].g;
		tmpImg[tmpHeight-1][tmpWidth-1].r = dstImg[height-1][width-1].r;
		
		if(channel == 4){
			tmpImg[0][0].a = dstImg[0][0].a;
			tmpImg[0][tmpWidth-1].a = dstImg[0][width-1].a;
			tmpImg[tmpHeight-1][0].a = dstImg[height-1][0].a;
			tmpImg[tmpHeight-1][tmpWidth-1].a = dstImg[height-1][width-1].a;
		}
		
		for(int i = 0; i < height; i++){
			tmpImg[i][0].b = dstImg[i][0].b;
			tmpImg[i][0].g = dstImg[i][0].g;
			tmpImg[i][0].r = dstImg[i][0].r;
			
			tmpImg[i][tmpWidth-1].b = dstImg[i][width-1].b;
			tmpImg[i][tmpWidth-1].g = dstImg[i][width-1].g;
			tmpImg[i][tmpWidth-1].r = dstImg[i][width-1].r;
			
			if(channel == 4){
				tmpImg[i][0].a = dstImg[i][0].a;
				tmpImg[i][tmpWidth-1].a = dstImg[i][width-1].a;
			}
		}
		for(int j = 0; j < width; j++){
			tmpImg[0][j].b = dstImg[0][j].b;
			tmpImg[0][j].g = dstImg[0][j].g;
			tmpImg[0][j].r = dstImg[0][j].r;
			
			tmpImg[tmpHeight-1][j].b = dstImg[height-1][j].b;
			tmpImg[tmpHeight-1][j].g = dstImg[height-1][j].g;
			tmpImg[tmpHeight-1][j].r = dstImg[height-1][j].r;

			if(channel == 4){
				tmpImg[0][j].a = dstImg[0][j].a;
				tmpImg[tmpHeight-1][j].a = dstImg[height-1][j].a;
			}
		}
		
		// Copy
		for(int i = 0; i < height; i++){
			for(int j = 0; j < width; j++){
				tmpImg[i+1][j+1].b = dstImg[i][j].b;
				tmpImg[i+1][j+1].g = dstImg[i][j].g;
				tmpImg[i+1][j+1].r = dstImg[i][j].r;
				
				if(channel == 4){
					tmpImg[i+1][j+1].a = dstImg[i][j].a;
				}
			}
		}
		
		// Covolution
		for(int i = 0; i < height; i++){
			for(int j = 0; j < width; j++){
				pixel_b = 0;
				pixel_g = 0;
				pixel_r = 0;
				pixel_a = 0;
				
				for(int k_i = 0; k_i < kernel_size; k_i++){
					for(int k_j = 0; k_j < kernel_size; k_j++){
						pixel_b += tmpImg[i+k_i][j+k_j].b * Gkernel[k_i][k_j];
						pixel_g += tmpImg[i+k_i][j+k_j].g * Gkernel[k_i][k_j];
						pixel_r += tmpImg[i+k_i][j+k_j].r * Gkernel[k_i][k_j];
						if(channel == 4){
							pixel_a += tmpImg[i+k_i][j+k_j].a * Gkernel[k_i][k_j];
						}
					}	
				}
				dstImg[i][j].b = (unsigned char)pixel_b;
				dstImg[i][j].g = (unsigned char)pixel_g;
				dstImg[i][j].r = (unsigned char)pixel_r;
				
				if(channel == 4){
					dstImg[i][j].a = pixel_a;
				}	
			}
		}
	}

	end_time = clock();
	cout << "Processing time: "<< (end_time - start_time) / CLOCKS_PER_SEC << " s" << endl;
	return tmpImg;
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
	fin  = fopen("output3_1.bmp","rb");
	fout = fopen("output3_2.bmp","wb");
	
	
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

		tmp_img = denoise(img, 10);
		
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
