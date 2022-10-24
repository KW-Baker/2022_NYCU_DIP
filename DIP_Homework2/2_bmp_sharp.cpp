/**************************************************************************/
// FILE NAME: 2_bmp_sharp.cpp
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

// iteration
int iter1 = 1;
int iter2 = 2;

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


// sharp
bool sharp(FILE *fout, RGB** srcImg, int iteration){
	// Start timer
	start_time = clock();
	channel = (InfoHeader.biBitCount == 24) ? 3 : 4;
	
	int tmpHeight;
	int tmpWidth;
	int pixel_b, pixel_g, pixel_r, pixel_a;	
	
	// Laplacian filter
	int kernel_size = 3;
	int Lkernel[kernel_size][kernel_size];
	
	Lkernel[0][0] = -1;
	Lkernel[0][1] = -1;
	Lkernel[0][2] = -1;
	Lkernel[1][0] = -1;
	Lkernel[1][1] =  9;
	Lkernel[1][2] = -1;
	Lkernel[2][0] = -1;
	Lkernel[2][1] = -1;
	Lkernel[2][2] = -1;
	
	cout << "Laplacian filter: " << endl;
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			printf("%2d ", Lkernel[i][j]);
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
	
	// Laplacian
	for(int iter = 0; iter < iteration; iter++){
		
		// Zero Padding
		/*for(int i = 0; i < tmpHeight; i++){
			for(int j = 0; j < tmpWidth; j++){
				tmpImg[i][j].b = 0;
				tmpImg[i][j].g = 0;
				tmpImg[i][j].r = 0;
				
				if(channel == 4){
					tmpImg[i][j].a = 0;
				}
				// cout << (int)(tmpImg[i][j].b) << ", " << (int)(tmpImg[i][j].g) << ", " << (int)(tmpImg[i][j].r) << endl;
			}
		}*/
		
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
						pixel_b += tmpImg[i+k_i][j+k_j].b * Lkernel[k_i][k_j];
						pixel_g += tmpImg[i+k_i][j+k_j].g * Lkernel[k_i][k_j];
						pixel_r += tmpImg[i+k_i][j+k_j].r * Lkernel[k_i][k_j];
						if(channel == 4){
							pixel_a += tmpImg[i+k_i][j+k_j].a * Lkernel[k_i][k_j];
						}
					}	
				}
				
				// pixel_b
				if(pixel_b < 0){
					dstImg[i][j].b = 0;
				}
				else if(pixel_b > 255){
					dstImg[i][j].b = 255;
				}
				else{
					dstImg[i][j].b = (unsigned char)pixel_b;
				}
				
				// pixel_g
				if(pixel_g < 0){
					dstImg[i][j].g = 0;
				}
				else if(pixel_g > 255){
					dstImg[i][j].g = 255;
				}
				else{
					dstImg[i][j].g = (unsigned char)pixel_g;
				}
				
				// pixel_r
				if(pixel_r < 0){
					dstImg[i][j].r = 0;
				}
				else if(pixel_r > 255){
					dstImg[i][j].r = 255;
				}
				else{
					dstImg[i][j].r = (unsigned char)pixel_r;
				}
				
				// pixel_a
				if(channel == 4){
					if(pixel_a < 0){
						dstImg[i][j].a = 0;
					}
					else if(pixel_a > 255){
						dstImg[i][j].a = 255;
					}
					else{
						dstImg[i][j].a = (unsigned char)pixel_a;
					}
				}	
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
	
	// Destruction
	for (int i = 0; i < tmpHeight; i++){
		delete[] tmpImg[i]; // Delete columns
	}
	delete[] tmpImg; // Delete Rows


	end_time = clock();
	cout << "Processing time: "<< (end_time - start_time) / CLOCKS_PER_SEC << " s" << endl;
	return true;
}


// main function
int main(){
	// Maker directory
	mkdir("./2_sharpness");
	
	// Open bitmap file
	fin  = fopen("./input2.bmp","rb");
	fout1 = fopen("./2_sharpness/output2_1.bmp","wb");
	fout2 = fopen("./2_sharpness/output2_2.bmp","wb");
	
	
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
		cout << "Sharpness Start..." << endl << endl;
		
		// 1st sharp
		cout << "[output1] iteration: " << iter1 << endl << endl;
		if(sharp(fout1, img, iter1)){
			cout << endl;
			cout << "Sharp - 1 Success!" << endl;
		}
		else{
			cout << endl;
			cout << "Sharp - 1 Failed!" << endl;
			return 0;
		}	
		
		cout << "======================" << endl;
		
		// 2nd sharp
		cout << "[output2] iteration: " << iter2 << endl << endl;
		if(sharp(fout2, img, iter2)){
			cout << endl;
			cout << "Sharp - 2 Success!" << endl;
		}
		else{
			cout << endl;
			cout << "Sharp - 2 Failed!" << endl;
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












