/**************************************************************************/
// FILE NAME: bmp_qr.cpp
// VERSRION: 1.0
// DATE: Oct 3, 2022
// AUTHOR: Kuan-Wei Chen / NYCU IEE Oasis Lab / 310510221
// CODE TYPE: CPP
// DESCRIPTION: 2022 Fall Digital Image Procssing / Homework1
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
#define SCALING 1.5 
#define LENGTH_NAME_BMP 30
#define LENGTH_MODE     2

using namespace std;

BMPFILEHEADER strHead;
RGBQUAD       strPla[256];
BMPINFOHEADER strInfo;

int src_width, src_height = 0;
int dst_width, dst_height = 0;
double scaling = 1.5;

double start_time, end_time; 

unsigned quanBit;

char strFile[LENGTH_NAME_BMP]; // input file name
string mode; // operation mode

int image_num = 0;

// Show bitmap file header
void showBMPHead(BMPFILEHEADER BMPHead)
{
	cout << "===== File Header =====" << endl;
	cout << "File size: "  << BMPHead.bfSize << endl;
	cout << "Reserved 1: " << BMPHead.bfReserved1 << endl;
	cout << "Reserved 2: " << BMPHead.bfReserved2 << endl;
	cout << "OffBits: "    << BMPHead.bfOffBits   << endl << endl;	
}

// Show bitmap info header
void showBMPInfoHead(BMPINFOHEADER BMPInfoHead)
{
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


// Horizontal flip
bool flipHorizontal(FILE *fpi)
{
	// Make directory
	_mkdir("./1_result_flip");

	IMAGEDATA_3 *dstImg_3 = NULL; // 3 channels
	IMAGEDATA_4 *dstImg_4 = NULL; // 4 channels
	FILE *fpw;
	int width, height;
	
	// Start timer
	start_time = clock();
	
	width  = strInfo.biWidth;
	height = strInfo.biHeight; 
	
	dstImg_3 = (IMAGEDATA_3*)malloc(width * height * sizeof(IMAGEDATA_3)); // channel = 3
	dstImg_4 = (IMAGEDATA_4*)malloc(width * height * sizeof(IMAGEDATA_4)); // channel = 4 
	
	
	
	if(strInfo.biBitCount == 24) // channel = 3
	{
		// Initialize
		for(int i = 0; i < height; ++i)
		{
			for(int j = 0; j < width; ++j)
			{
				(*(dstImg_3 + i * width +j)).blue  = 0;
				(*(dstImg_3 + i * width +j)).green = 0;
				(*(dstImg_3 + i * width +j)).red   = 0;	
			}
		}
		
		// Read Image data information
		fread(dstImg_3, sizeof(struct IMAGEDATA_3) * width, height, fpi);
	}
	else if(strInfo.biBitCount == 32) // channel = 4
	{
		// Initialize
		for(int i = 0; i < height; ++i)
		{
			for(int j = 0; j < width; ++j)
			{
				(*(dstImg_4 + i * width +j)).blue     = 0;
				(*(dstImg_4 + i * width +j)).green    = 0;
				(*(dstImg_4 + i * width +j)).red      = 0;
				(*(dstImg_4 + i * width +j)).reserved = 0;
			}
		}
		// Read Image data information
		fread(dstImg_4, sizeof(struct IMAGEDATA_4) * width, height, fpi);
	}
	else
	{
		cout << "Can not read image !" << endl;
		return false;
	}
	
	
	cout << "Write the output file..." << endl;
	
	// Open output bmp file
	if(image_num == 1){
		if((fpw=fopen("./1_result_flip/output1_flip.bmp", "wb"))==NULL)
		{
			cout << "Create the bmp file error!" << endl;
			return false;	
		}
	}
	else if(image_num == 2)
	{
		if((fpw=fopen("./1_result_flip/output2_flip.bmp", "wb"))==NULL)
		{
			cout << "Create the bmp file error!" << endl;
			return false;	
		}
	}
	else
	{
		cout << "Can not find input1.bmp or input2.bmp" << endl;
		return false;
	}
		
	
	
	// Write bitmap file header
	WORD bfType_w = BM;
	fwrite(&bfType_w, 1, sizeof(WORD), fpw);
	fwrite(&strHead, 1, sizeof(BMPFILEHEADER), fpw);
	
	
	// Write bitmap info header
	strInfo.biWidth  = width;
	strInfo.biHeight = height;
	fwrite(&strInfo, 1, sizeof(BMPINFOHEADER), fpw);
	
	
	// Write bitmap color table
	if(strInfo.biBitCount == 24) // channels = 3
	{
		for(unsigned int i = 0; i < strInfo.biClrUsed; i++)
		{
			fwrite(&strPla[i].rgbBlue,     1, sizeof(BYTE), fpw);
			fwrite(&strPla[i].rgbGreen,    1, sizeof(BYTE), fpw);
			fwrite(&strPla[i].rgbRed,      1, sizeof(BYTE), fpw);	
		}		
	}
	else if(strInfo.biBitCount == 32) // channels = 4
	{
		for(unsigned int i = 0; i < strInfo.biClrUsed; i++)
		{
			fwrite(&strPla[i].rgbBlue,     1, sizeof(BYTE), fpw);
			fwrite(&strPla[i].rgbGreen,    1, sizeof(BYTE), fpw);
			fwrite(&strPla[i].rgbRed,      1, sizeof(BYTE), fpw);
			fwrite(&strPla[i].rgbReserved, 1, sizeof(BYTE), fpw);	
		}	
	}
	else
	{
		cout << "Can not write image data!" << endl;
		return false;
	}
	
	
	// Flip
	// Write bitmap data
	if(strInfo.biBitCount == 24) // channels = 3
	{
		for(int i = 0; i < height; ++i)
		{
			for(int j = 0; j < width; ++j)
			{
				fwrite( &(*(dstImg_3 + i * width + (width - 1 - j))).red,   1, sizeof(BYTE), fpw);
				fwrite( &(*(dstImg_3 + i * width + (width - 1 - j))).green, 1, sizeof(BYTE), fpw);
				fwrite( &(*(dstImg_3 + i * width + (width - 1 - j))).blue,  1, sizeof(BYTE), fpw);			
			}
		}
	}
	else if(strInfo.biBitCount == 32) // channels = 4
	{
		for(int i = 0; i < height; ++i)
		{
			for(int j = 0; j < width; ++j)
			{
				fwrite( &(*(dstImg_4 + i * width + (width - 1 - j))).red,      1, sizeof(BYTE), fpw);
				fwrite( &(*(dstImg_4 + i * width + (width - 1 - j))).green,    1, sizeof(BYTE), fpw);
				fwrite( &(*(dstImg_4 + i * width + (width - 1 - j))).blue,     1, sizeof(BYTE), fpw);
				fwrite( &(*(dstImg_4 + i * width + (width - 1 - j))).reserved, 1, sizeof(BYTE), fpw);
			}
		}
	}
	else
	{
		cout << "Error!" << endl;
		return false;
	}
	
	// Close output file
	fclose(fpw);
	cout << "Close the output file..." << endl;
	
	delete[] dstImg_3;
	delete[] dstImg_4;
	
	end_time = clock();
	cout << "Processing time: "<< (end_time - start_time) / CLOCKS_PER_SEC << " s" << endl;
	return true;
}


// Qantizer
unsigned char quantizer(unsigned char quanBit, unsigned char src)
{
	int k;
	
	k = 256 / pow(2, quanBit);
	return (unsigned char)((src / k) * k);
}


// Quantization Resolution
bool quanResolution(FILE *fpi, unsigned char quanBit)
{
	FILE *fpw;
	
	IMAGEDATA_3 *dstImg_3 = NULL; // 3 channels
	IMAGEDATA_4 *dstImg_4 = NULL; // 4 channels
	
	int width, height;
	
	unsigned char pixel_r;
	unsigned char pixel_g;
	unsigned char pixel_b;
	unsigned char pixel_a; //alpha
	
	// Maker directory
	mkdir("./2_result_quantization");
	
	// Start timer
	start_time = clock();
	
	
	width  = strInfo.biWidth;
	height = strInfo.biHeight; 
	
	
		
	

	dstImg_3 = (IMAGEDATA_3*)malloc(width * height * sizeof(IMAGEDATA_3)); // channel = 3
	dstImg_4 = (IMAGEDATA_4*)malloc(width * height * sizeof(IMAGEDATA_4)); // channel = 4 
	
	
	
	if(strInfo.biBitCount == 24) // channel = 3
	{
		// Initialization
		for(int i = 0; i < height; ++i)
		{
			for(int j = 0; j < width; ++j)
			{
				(*(dstImg_3 + i * width +j)).blue  = 0;
				(*(dstImg_3 + i * width +j)).green = 0;
				(*(dstImg_3 + i * width +j)).red   = 0;
				
			}
		}
		// Read Image data information
		fread(dstImg_3, sizeof(struct IMAGEDATA_3) * width, height, fpi);
	}
	else if(strInfo.biBitCount == 32) // channel = 4
	{
		// Initialization
		for(int i = 0; i < height; ++i)
		{
			for(int j = 0; j < width; ++j)
			{
				(*(dstImg_4 + i * width +j)).blue     = 0;
				(*(dstImg_4 + i * width +j)).green    = 0;
				(*(dstImg_4 + i * width +j)).red      = 0;
				(*(dstImg_4 + i * width +j)).reserved = 0;
			}
		}
		// Read Image data information
		fread(dstImg_4, sizeof(struct IMAGEDATA_4) * width, height, fpi);
	} 
	else
	{
		cout << "Can not write image data!" << endl;
		return false;
	}


	
	cout << "Write the output file..." << endl;
	
	// Open output bmp image
	if(quanBit == 6)
	{
		if(image_num == 1)
		{ 
			if((fpw=fopen("./2_result_quantization/output1_1.bmp", "wb"))==NULL)
			{
				cout << "Create the bmp file error!" << endl;
				return false;	
			}
		} 
		else
		{ 
			if((fpw=fopen("./2_result_quantization/output2_1.bmp", "wb"))==NULL)
			{
				cout << "Create the bmp file error!" << endl;
				return false;	
			}
		} 
	}
	else if(quanBit == 4)
	{
		if(image_num == 1)
		{ 
			if((fpw=fopen("./2_result_quantization/output1_2.bmp", "wb"))==NULL)
			{
				cout << "Create the bmp file error!" << endl;
				return false;	
			}
		} 
		else
		{ 
			if((fpw=fopen("./2_result_quantization/output2_2.bmp", "wb"))==NULL)
			{
				cout << "Create the bmp file error!" << endl;
				return false;	
			}
		} 
			
	}
	else if(quanBit == 2)
	{
		if(image_num == 1)
		{ 
			if((fpw=fopen("./2_result_quantization/output1_3.bmp", "wb"))==NULL)
			{
				cout << "Create the bmp file error!" << endl;
				return false;	
			}
		} 
		else
		{ 
			if((fpw=fopen("./2_result_quantization/output2_3.bmp", "wb"))==NULL)
			{
				cout << "Create the bmp file error!" << endl;
				return false;	
			}
		} 
	}
	else
	{
		cout << "Create the bmp file error!" << endl;
		return false;
	}
	
	// Write bitmap file header
	WORD bfType_w = BM;
	fwrite(&bfType_w, 1, sizeof(WORD), fpw);
	fwrite(&strHead, 1, sizeof(BMPFILEHEADER), fpw);
	
	// Write bitmap info header
	fwrite(&strInfo, 1, sizeof(BMPINFOHEADER), fpw);
	
	// Write color table
	if(strInfo.biBitCount == 24) // channels = 3
	{
		for(unsigned int i = 0; i < strInfo.biClrUsed; i++)
		{
			fwrite(&strPla[i].rgbBlue,     1, sizeof(BYTE), fpw);
			fwrite(&strPla[i].rgbGreen,    1, sizeof(BYTE), fpw);
			fwrite(&strPla[i].rgbRed,      1, sizeof(BYTE), fpw);	
		}		
	}
	else if(strInfo.biBitCount == 32) // channels = 4
	{
		for(unsigned int i = 0; i < strInfo.biClrUsed; i++)
		{
			fwrite(&strPla[i].rgbBlue,     1, sizeof(BYTE), fpw);
			fwrite(&strPla[i].rgbGreen,    1, sizeof(BYTE), fpw);
			fwrite(&strPla[i].rgbRed,      1, sizeof(BYTE), fpw);
			fwrite(&strPla[i].rgbReserved, 1, sizeof(BYTE), fpw);	
		}
	}
	else
	{
		cout << "Can not write color table" << endl;
		return false;
	}
	
	
	// Quantization
	// Weite bitmap data
	if(strInfo.biBitCount == 24) // channels = 3
	{
		for(int i = 0; i < height; ++i)
		{
			for(int j = 0; j < width; ++j)
			{	
				
				pixel_r = quantizer(quanBit, ((*(dstImg_3 + i * width + j)).red));
				pixel_g = quantizer(quanBit, ((*(dstImg_3 + i * width + j)).green));
				pixel_b = quantizer(quanBit, ((*(dstImg_3 + i * width + j)).blue));
				fwrite( &pixel_r,   1, sizeof(BYTE), fpw);
				fwrite( &pixel_g,   1, sizeof(BYTE), fpw);
				fwrite( &pixel_b,   1, sizeof(BYTE), fpw);
			}
		}
	}
	else if(strInfo.biBitCount == 32) // channels = 4
	{
		for(int i = 0; i < height; ++i)
		{
			for(int j = 0; j < width; ++j)
			{
				pixel_r = quantizer(quanBit, ((*(dstImg_4 + i * width + j)).red));
				pixel_g = quantizer(quanBit, ((*(dstImg_4 + i * width + j)).green));
				pixel_b = quantizer(quanBit, ((*(dstImg_4 + i * width + j)).blue));
				pixel_a = quantizer(quanBit, ((*(dstImg_4 + i * width + j)).reserved));
				fwrite( &pixel_r,   1, sizeof(BYTE), fpw);
				fwrite( &pixel_g,   1, sizeof(BYTE), fpw);
				fwrite( &pixel_b,   1, sizeof(BYTE), fpw);
				fwrite( &pixel_a,   1, sizeof(BYTE), fpw);
			}
		}
	}
	else
	{
		cout << "Can not write image data!" << endl;
		return false;
	}
	
	fclose(fpw);
	cout << "Close the output file..." << endl;
	
	delete[] dstImg_3;
	delete[] dstImg_4;
	
	end_time = clock();
	cout << "Processing time: "<< (end_time - start_time) / CLOCKS_PER_SEC << " s" << endl;
	return true;
}

	

// Bilinear interpolation
bool bilinear_resize(FILE *fpi)
{
	IMAGEDATA_3 *srcImg_3 = NULL;
	IMAGEDATA_3 *dstImg_3 = NULL;
	IMAGEDATA_4 *srcImg_4 = NULL;
	IMAGEDATA_4 *dstImg_4 = NULL;
	
	BMPFILEHEADER DST_HEAD;
	BMPINFOHEADER DST_INFO;
	
	FILE *fpw;
	
	int lineByte;
	
	float x_ratio;
	float y_ratio;
	float x_weight;
	float y_weight;
	
	int x_h, y_h; // high 
	int x_l, y_l; // low
	
	unsigned char red_a,   red_b,   red_c,   red_d   = 0;
	unsigned char green_a, green_b, green_c, green_d = 0;
	unsigned char blue_a,  blue_b,  blue_c,  blue_d  = 0;
	unsigned char alpha_a, alpha_b, alpha_c, alpha_d = 0;

	unsigned char pixel_r;
	unsigned char pixel_g;
	unsigned char pixel_b;
	unsigned char pixel_a;
	
	
	src_width  = strInfo.biWidth;
	src_height = strInfo.biHeight; 
	
	dst_width  = int(double(src_width) * scaling);
	dst_height = int(double(src_height) * scaling);
	
	// Show input and ouput width & height 
	cout << "input  width:  " << src_width  << endl;
	cout << "input  height: " << src_height << endl << endl;
	cout << "output width:  " << dst_width  << endl;
	cout << "output height: " << dst_height << endl;
	
	lineByte = (dst_width + 3) / 4 * 4;
	// cout << "lineByte: " << lineByte << endl;
	
	
	
	
	
	if(mode == "-u" || mode == "-U")
	{
		// Maker directory
		_mkdir("./3_result_upScaling");
		
		// Start timer
		start_time = clock();
		
		// Open output bitmap file
		if((fpw = fopen("./3_result_upScaling/up_scaling.bmp", "wb")) == NULL)
		{
			cout << "Create the bmp file error!" << endl;
			return false;
		}
	}
	else if(mode == "-d" || mode == "-D")
	{
		// Maker directory
		_mkdir("./4_result_downScaling");
		
		// Start timer
		start_time = clock();
		
		// Open output bitmap file
		if((fpw = fopen("./4_result_downScaling/dwon_scaling.bmp", "wb")) == NULL)
		{
			cout << "Create the bmp file error!" << endl;
			return false;
		}
	}
	
	// Open output bmp file
	
	
	
	// Set output bitmap file header
	WORD bfType_w = BM;
	DST_HEAD.bfOffBits = sizeof(BMPFILEHEADER) + sizeof(BMPINFOHEADER) + sizeof(bfType_w); // =54  // + 256 * sizeof(RGBQUAD) 
	DST_HEAD.bfReserved1 = 0;
	DST_HEAD.bfReserved2 = 0;
	DST_HEAD.bfSize = DST_HEAD.bfOffBits + dst_height * lineByte * 4;
	showBMPHead(DST_HEAD);
	
	
	// Set outpu bitmap info header
	DST_INFO.biSize = sizeof(BMPINFOHEADER);
	DST_INFO.biWidth  = dst_width;
	DST_INFO.biHeight = dst_height;
	DST_INFO.biPlanes = strInfo.biPlanes;
	DST_INFO.biBitCount = strInfo.biBitCount;
	DST_INFO.biCompression = strInfo.biCompression;
	DST_INFO.biXPelsPerMeter = strInfo.biXPelsPerMeter;
	DST_INFO.biYPelsPerMeter = strInfo.biYPelsPerMeter;
	DST_INFO.biSizeImage = 0;
	DST_INFO.biClrUsed = 0;
	DST_INFO.biClrImportant = 0;
	showBMPInfoHead(DST_INFO);
	
	
	// Write output bmp file header & info header
	fwrite(&bfType_w, 1, sizeof(WORD), fpw);
	fwrite(&DST_HEAD, sizeof(BMPFILEHEADER), 1, fpw);
	fwrite(&DST_INFO, sizeof(BMPINFOHEADER), 1, fpw);
	
	// Calculate ratio
	if(dst_width >= 1)
	{
		x_ratio = float(src_width  - 1) / float(dst_width  - 1);
	}
	else
	{
		x_ratio = 0;
	}
	
	if(dst_height >= 1)
	{
		y_ratio = float(src_height - 1) / float(dst_height - 1);
	}
	else
	{
		y_ratio = 0;
	}
	
	
	// Initialize
	if(strInfo.biBitCount == 24) // channel = 3
	{
		srcImg_3 = (IMAGEDATA_3*)malloc(src_width * src_height * sizeof(IMAGEDATA_3));
		dstImg_3 = (IMAGEDATA_3*)malloc(dst_width * dst_height * sizeof(IMAGEDATA_3));
		
		for(int i = 0; i < src_height; ++i)
		{
			for(int j = 0; j < src_width; ++j)
			{
				(*(srcImg_3 + i * src_width + j)).blue  = 0;
				(*(srcImg_3 + i * src_width + j)).green = 0;
				(*(srcImg_3 + i * src_width + j)).red   = 0;	
			}
		}
		
		for(int i = 0; i < dst_height; ++i)
		{
			for(int j = 0; j < dst_width; ++j)
			{
				(*(dstImg_3 + i * dst_width + j)).blue  = 0;
				(*(dstImg_3 + i * dst_width + j)).green = 0;
				(*(dstImg_3 + i * dst_width + j)).red   = 0;	
			}
		}
		
		// Read input data
		fread(srcImg_3, sizeof(struct IMAGEDATA_3) * src_width, src_height, fpi);	
	}
	else if(strInfo.biBitCount == 32) // channel = 4
	{
		
		srcImg_4 = (IMAGEDATA_4*)malloc(src_width * src_height * sizeof(IMAGEDATA_4));
		dstImg_4 = (IMAGEDATA_4*)malloc(dst_width * dst_height * sizeof(IMAGEDATA_4));

		for(int i = 0; i < src_height; ++i)
		{
			for(int j = 0; j < src_width; ++j)
			{
				(*(srcImg_4 + i * src_width +j)).blue     = 0;
				(*(srcImg_4 + i * src_width +j)).green    = 0;
				(*(srcImg_4 + i * src_width +j)).red      = 0;
				(*(srcImg_4 + i * src_width +j)).reserved = 0;
			}
		}
		for(int i = 0; i < dst_height; ++i)
		{
			for(int j = 0; j < dst_width; ++j)
			{
				(*(dstImg_4 + i * dst_width +j)).blue     = 0;
				(*(dstImg_4 + i * dst_width +j)).green    = 0;
				(*(dstImg_4 + i * dst_width +j)).red      = 0;
				(*(dstImg_4 + i * dst_width +j)).reserved = 0;
			}
		}
		
		// Read input data
		fread(srcImg_4, sizeof(struct IMAGEDATA_4) * src_width, src_height, fpi);
	} 
	
	
	// Bilinear interpolation
	for(int i = 0; i < dst_height; ++i)
	{
		for(int j = 0; j < dst_width; ++j)
		{
			// Higher bound
			x_h = ceil(x_ratio * j);
			y_h = ceil(y_ratio * i);
			
			// Lower bound
			x_l = floor(x_ratio * j);
			y_l = floor(y_ratio * i);
			
			// Weight
			x_weight = (x_ratio * j) - x_l;
			y_weight = (y_ratio * i) - y_l;
			
			
			if(strInfo.biBitCount == 24)
			{
				red_a   = (unsigned char)((*(srcImg_3 + y_l * src_width + x_l)).red);
				red_b   = (unsigned char)((*(srcImg_3 + y_l * src_width + x_h)).red);
				red_c   = (unsigned char)((*(srcImg_3 + y_h * src_width + x_l)).red);
				red_d   = (unsigned char)((*(srcImg_3 + y_h * src_width + x_h)).red);
				
				green_a = (unsigned char)((*(srcImg_3 + y_l * src_width + x_l)).green);
				green_b = (unsigned char)((*(srcImg_3 + y_l * src_width + x_h)).green);
				green_c = (unsigned char)((*(srcImg_3 + y_h * src_width + x_l)).green);
				green_d = (unsigned char)((*(srcImg_3 + y_h * src_width + x_h)).green);
				
				blue_a  = (unsigned char)((*(srcImg_3 + y_l * src_width + x_l)).blue);
				blue_b  = (unsigned char)((*(srcImg_3 + y_l * src_width + x_h)).blue);
				blue_c  = (unsigned char)((*(srcImg_3 + y_h * src_width + x_l)).blue);
				blue_d  = (unsigned char)((*(srcImg_3 + y_h * src_width + x_h)).blue);
				
				pixel_r = (unsigned char)(red_a   * (1 - x_weight) * (1 - y_weight) + 
						                  red_b   * (    x_weight) * (1 - y_weight) +
						                  red_c   * (1 - x_weight) * (    y_weight) +
						                  red_d   * (    x_weight) * (    y_weight)); 
						  
				pixel_g = (unsigned char)(green_a * (1 - x_weight) * (1 - y_weight) +
										  green_b * (    x_weight) * (1 - y_weight) +
										  green_c * (1 - x_weight) * (    y_weight) +
										  green_d * (    x_weight) * (    y_weight)); 
				
				pixel_b = (unsigned char)(blue_a  * (1 - x_weight) * (1 - y_weight) +
										  blue_b  * (    x_weight) * (1 - y_weight) +
										  blue_c  * (1 - x_weight) * (    y_weight) +
										  blue_d  * (    x_weight) * (    y_weight));
				
				// Write output bmp file
				fwrite( &pixel_r,   1, sizeof(BYTE), fpw);
				fwrite( &pixel_g,   1, sizeof(BYTE), fpw);
				fwrite( &pixel_b,   1, sizeof(BYTE), fpw);	
			}
			else if(strInfo.biBitCount == 32)
			{
				red_a   = (unsigned char)((*(srcImg_4 + y_l * src_width + x_l)).red);
				red_b   = (unsigned char)((*(srcImg_4 + y_l * src_width + x_h)).red);
				red_c   = (unsigned char)((*(srcImg_4 + y_h * src_width + x_l)).red);
				red_d   = (unsigned char)((*(srcImg_4 + y_h * src_width + x_h)).red);
				
				green_a = (unsigned char)((*(srcImg_4 + y_l * src_width + x_l)).green);
				green_b = (unsigned char)((*(srcImg_4 + y_l * src_width + x_h)).green);
				green_c = (unsigned char)((*(srcImg_4 + y_h * src_width + x_l)).green);
				green_d = (unsigned char)((*(srcImg_4 + y_h * src_width + x_h)).green);
				
				blue_a  = (unsigned char)((*(srcImg_4 + y_l * src_width + x_l)).blue);
				blue_b  = (unsigned char)((*(srcImg_4 + y_l * src_width + x_h)).blue);
				blue_c  = (unsigned char)((*(srcImg_4 + y_h * src_width + x_l)).blue);
				blue_d  = (unsigned char)((*(srcImg_4 + y_h * src_width + x_h)).blue);
				
				alpha_a = (unsigned char)((*(srcImg_4 + y_l * src_width + x_l)).reserved);
				alpha_b = (unsigned char)((*(srcImg_4 + y_l * src_width + x_h)).reserved);
				alpha_c = (unsigned char)((*(srcImg_4 + y_h * src_width + x_l)).reserved);
				alpha_d = (unsigned char)((*(srcImg_4 + y_h * src_width + x_h)).reserved);
		
				pixel_r = (unsigned char)(red_a   * (1 - x_weight) * (1 - y_weight) + 
						                  red_b   * (    x_weight) * (1 - y_weight) +
						                  red_c   * (1 - x_weight) * (    y_weight) +
						                  red_d   * (    x_weight) * (    y_weight)); 
						  
				pixel_g = (unsigned char)(green_a * (1 - x_weight) * (1 - y_weight) +
										  green_b * (    x_weight) * (1 - y_weight) +
										  green_c * (1 - x_weight) * (    y_weight) +
										  green_d * (    x_weight) * (    y_weight)); 
				
				pixel_b = (unsigned char)(blue_a  * (1 - x_weight) * (1 - y_weight) +
										  blue_b  * (    x_weight) * (1 - y_weight) +
										  blue_c  * (1 - x_weight) * (    y_weight) +
										  blue_d  * (    x_weight) * (    y_weight));

				pixel_a = (unsigned char)(alpha_a  * (1 - x_weight) * (1 - y_weight) +
										  alpha_b  * (    x_weight) * (1 - y_weight) +
										  alpha_c  * (1 - x_weight) * (    y_weight) +
										  alpha_d  * (    x_weight) * (    y_weight)); 

				// Write output bmp file
				fwrite( &pixel_r,   1, sizeof(BYTE), fpw);
				fwrite( &pixel_g,   1, sizeof(BYTE), fpw);
				fwrite( &pixel_b,   1, sizeof(BYTE), fpw);
				fwrite( &pixel_a,   1, sizeof(BYTE), fpw);
			}
			else
			{
				cout << "Can not write image data!" << endl;
				return false;
			}
			
			
		}
	}
	
	// Close output bitmap file
	fclose(fpi);
	fclose(fpw);
	cout << "Closed the output file..." << endl;
	
	delete[] srcImg_3;
	delete[] dstImg_3;
	delete[] srcImg_4;
	delete[] dstImg_4;
	
	end_time = clock();
	cout << "Processing time: "<< (end_time - start_time) / CLOCKS_PER_SEC << " s" << endl;
	return true;
}


// Up scaling
bool upScaling(FILE *fpi)
{
	scaling = (scaling > 1) ? scaling : float(1 / scaling);
	cout << "scaling = " << scaling << endl << endl;
	bilinear_resize(fpi);

 	return true;
}


bool downScaling(FILE *fpi)
{
	scaling = (scaling > 1) ? float(1 / scaling) : scaling;
	cout << "scaling = " << scaling << endl << endl;
	bilinear_resize(fpi);

 	return true;
}


// main function
int main()
{
	// 3 channels
	IMAGEDATA_3 *srcImg_3 = NULL;
	IMAGEDATA_3 *dstImg_3 = NULL;
	
	// 4 channels
	IMAGEDATA_4 *srcImg_4 = NULL;
	IMAGEDATA_4 *dstImg_4 = NULL;
	
	
	int width, height;
	
	
	
	
	
	
	// Enter the bmp file name
	// cout << "Please enter the bmp file name: " << endl;
	// cin  >> strFile;
	// cout << endl;
	
		
	// Select the processing mode
	/*cout << "Please select the modes: "   << endl;
	cout << "Horizontal Flip:         -f" << endl;
	cout << "Quantization Resolution: -q" << endl;
	cout << "Up-Scaling   (rate=1.5): -u" << endl;
	cout << "Down-Scaling (rate=1.5): -d" << endl;
	cin  >> mode;
	cout << endl;*/
	
	
	
	// Open bitmap file
	FILE *fpi, *fpw;

	mode = "-q";
	
	/////////
	image_num = 1;
	quanBit = 6;
	// Open bitmap file
	
	fpi = fopen("./input1.bmp","rb");
	if(fpi != NULL)
	{
		WORD bfType;
		cout << "Loading the file..." << endl;
		fread(&bfType, 1, sizeof(WORD), fpi);
		
		// Check if the file is bmp file
		if(bfType != BM) 
		{
			cout << "The file is not a bmp file!" << endl;
			return NULL;
		}
		
		
		// Read bitmap file header
		fread(&strHead, 1, sizeof(BMPFILEHEADER), fpi);
		showBMPHead(strHead);
		
		// Read bitmap info header
		fread(&strInfo, 1, sizeof(BMPINFOHEADER), fpi);
		showBMPInfoHead(strInfo);
		
		// Read palettue
		cout << "==== Read palette ===="<< endl;
		cout << "biClrUsed: " << strInfo.biClrUsed << endl << endl;
		
		if(strInfo.biBitCount == 24) // 3 channels
		{
			for(unsigned int i=0; i<strInfo.biClrUsed; i++)
			{
				fread((char *)&strPla[i].rgbBlue,  1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbGreen, 1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbRed,   1, sizeof(BYTE), fpi);
				
				cout << "strPla[i].rgbBlue"     << strPla[i].rgbBlue     << endl;
				cout << "strPla[i].rgbGreen"    << strPla[i].rgbGreen    << endl;
				cout << "strPla[i].rgbRed"      << strPla[i].rgbRed      << endl;
			} 
		}
		else if(strInfo.biBitCount == 32) // 4 channels
		{
			for(unsigned int i=0; i<strInfo.biClrUsed; i++)
			{
				fread((char *)&strPla[i].rgbBlue,     1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbGreen,    1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbRed,      1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbReserved, 1, sizeof(BYTE), fpi);
				
				cout << "strPla[i].rgbBlue"     << strPla[i].rgbBlue     << endl;
				cout << "strPla[i].rgbGreen"    << strPla[i].rgbGreen    << endl;
				cout << "strPla[i].rgbRed"      << strPla[i].rgbRed      << endl;
				cout << "strPla[i].rgbReserved" << strPla[i].rgbReserved << endl;
			} 
		}
		else
		{
			cout << "Can not read color table" << endl;
		}
		
		
		// Image Processing
		cout << "== Image Processing ==" << endl;
		
		
		/* Select the processing mode
		-f or -F: Horizontal Flip
		-q or -Q: Quantization Resolution
		-u or -U: Up-Scaling
		-d or -D: Down-Scaling
		*/
		if(mode == "-f" || mode == "-F") // Horizontal Flip
		{
			cout << "Horizontal Flip Start" << endl;
			if(flipHorizontal(fpi))
			{
				cout << endl;
				cout << "Horizontal Flip Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Horizontal Flip Failed!" << endl;
				return -1;
			}	
		}
		else if(mode == "-q" || mode == "-Q") // Quantization Resolution
		{
			cout << "Qunatization Resolution Start" << endl;
			// cout << "Enter the bit of quantiation (<=8): " << endl;
			
			// cin >> quanBit;
			if(quanBit <= 8)
			{
				if(quanResolution(fpi, quanBit))
				{
					cout << endl;
					cout << "Qunatization Resolution Success!" << endl;
				}
				else
				{
					cout << endl;
					cout << "Qunatization Resolution Failed!" << endl;
					return -1;
				}	
			}
			else
			{
				cout << "Qunatization Resolution Failed!" << endl;
				return -1;
			}	
		}
		else if(mode == "-u" || mode == "-U") // Up-Scaling
		{
			cout << "Up-Scaling" << endl;
			cout << "Enter the up-scaling ratio (>1): " << endl;
			cin >> scaling;
			if(upScaling(fpi))
			{
				cout << endl;
				cout << "Up-scaling Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Up-scaling Failed!" << endl;
				return -1;
			}
		}
		else if(mode == "-d" || mode == "-D") // Down-Scaling
		{
			cout << "Down-Scaling" << endl;
			cout << "Enter the down-scaling ratio (>1): " << endl;
			cin >> scaling;
			if(downScaling(fpi))
			{
				cout << endl;
				cout << "Down-scaling Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Down-scaling Failed!" << endl;
				return -1;
			}
		}
		else
		{
			cout << "Mode error!" << endl;
			return -1;
		}
		
		fclose(fpi);	
	}
	else
	{
		cout << "File open error!" << endl;
		return NULL;
	}
	
	/////////
	image_num = 1;
	quanBit = 4;
	// Open bitmap file
	
	fpi = fopen("./input1.bmp","rb");
	if(fpi != NULL)
	{
		WORD bfType;
		cout << "Loading the file..." << endl;
		fread(&bfType, 1, sizeof(WORD), fpi);
		
		// Check if the file is bmp file
		if(bfType != BM) 
		{
			cout << "The file is not a bmp file!" << endl;
			return NULL;
		}
		
		
		// Read bitmap file header
		fread(&strHead, 1, sizeof(BMPFILEHEADER), fpi);
		showBMPHead(strHead);
		
		// Read bitmap info header
		fread(&strInfo, 1, sizeof(BMPINFOHEADER), fpi);
		showBMPInfoHead(strInfo);
		
		// Read palettue
		cout << "==== Read palette ===="<< endl;
		cout << "biClrUsed: " << strInfo.biClrUsed << endl << endl;
		
		if(strInfo.biBitCount == 24) // 3 channels
		{
			for(unsigned int i=0; i<strInfo.biClrUsed; i++)
			{
				fread((char *)&strPla[i].rgbBlue,  1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbGreen, 1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbRed,   1, sizeof(BYTE), fpi);
				
				cout << "strPla[i].rgbBlue"     << strPla[i].rgbBlue     << endl;
				cout << "strPla[i].rgbGreen"    << strPla[i].rgbGreen    << endl;
				cout << "strPla[i].rgbRed"      << strPla[i].rgbRed      << endl;
			} 
		}
		else if(strInfo.biBitCount == 32) // 4 channels
		{
			for(unsigned int i=0; i<strInfo.biClrUsed; i++)
			{
				fread((char *)&strPla[i].rgbBlue,     1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbGreen,    1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbRed,      1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbReserved, 1, sizeof(BYTE), fpi);
				
				cout << "strPla[i].rgbBlue"     << strPla[i].rgbBlue     << endl;
				cout << "strPla[i].rgbGreen"    << strPla[i].rgbGreen    << endl;
				cout << "strPla[i].rgbRed"      << strPla[i].rgbRed      << endl;
				cout << "strPla[i].rgbReserved" << strPla[i].rgbReserved << endl;
			} 
		}
		else
		{
			cout << "Can not read color table" << endl;
		}
		
		
		// Image Processing
		cout << "== Image Processing ==" << endl;
		
		
		/* Select the processing mode
		-f or -F: Horizontal Flip
		-q or -Q: Quantization Resolution
		-u or -U: Up-Scaling
		-d or -D: Down-Scaling
		*/
		if(mode == "-f" || mode == "-F") // Horizontal Flip
		{
			cout << "Horizontal Flip Start" << endl;
			if(flipHorizontal(fpi))
			{
				cout << endl;
				cout << "Horizontal Flip Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Horizontal Flip Failed!" << endl;
				return -1;
			}	
		}
		else if(mode == "-q" || mode == "-Q") // Quantization Resolution
		{
			cout << "Qunatization Resolution Start" << endl;
			// cout << "Enter the bit of quantiation (<=8): " << endl;
			
			// cin >> quanBit;
			if(quanBit <= 8)
			{
				if(quanResolution(fpi, quanBit))
				{
					cout << endl;
					cout << "Qunatization Resolution Success!" << endl;
				}
				else
				{
					cout << endl;
					cout << "Qunatization Resolution Failed!" << endl;
					return -1;
				}	
			}
			else
			{
				cout << "Qunatization Resolution Failed!" << endl;
				return -1;
			}	
		}
		else if(mode == "-u" || mode == "-U") // Up-Scaling
		{
			cout << "Up-Scaling" << endl;
			cout << "Enter the up-scaling ratio (>1): " << endl;
			cin >> scaling;
			if(upScaling(fpi))
			{
				cout << endl;
				cout << "Up-scaling Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Up-scaling Failed!" << endl;
				return -1;
			}
		}
		else if(mode == "-d" || mode == "-D") // Down-Scaling
		{
			cout << "Down-Scaling" << endl;
			cout << "Enter the down-scaling ratio (>1): " << endl;
			cin >> scaling;
			if(downScaling(fpi))
			{
				cout << endl;
				cout << "Down-scaling Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Down-scaling Failed!" << endl;
				return -1;
			}
		}
		else
		{
			cout << "Mode error!" << endl;
			return -1;
		}
		
		fclose(fpi);	
	}
	else
	{
		cout << "File open error!" << endl;
		return NULL;
	}
	
	/////////
	image_num = 1;
	quanBit = 2;
	// Open bitmap file
	
	fpi = fopen("./input1.bmp","rb");
	if(fpi != NULL)
	{
		WORD bfType;
		cout << "Loading the file..." << endl;
		fread(&bfType, 1, sizeof(WORD), fpi);
		
		// Check if the file is bmp file
		if(bfType != BM) 
		{
			cout << "The file is not a bmp file!" << endl;
			return NULL;
		}
		
		
		// Read bitmap file header
		fread(&strHead, 1, sizeof(BMPFILEHEADER), fpi);
		showBMPHead(strHead);
		
		// Read bitmap info header
		fread(&strInfo, 1, sizeof(BMPINFOHEADER), fpi);
		showBMPInfoHead(strInfo);
		
		// Read palettue
		cout << "==== Read palette ===="<< endl;
		cout << "biClrUsed: " << strInfo.biClrUsed << endl << endl;
		
		if(strInfo.biBitCount == 24) // 3 channels
		{
			for(unsigned int i=0; i<strInfo.biClrUsed; i++)
			{
				fread((char *)&strPla[i].rgbBlue,  1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbGreen, 1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbRed,   1, sizeof(BYTE), fpi);
				
				cout << "strPla[i].rgbBlue"     << strPla[i].rgbBlue     << endl;
				cout << "strPla[i].rgbGreen"    << strPla[i].rgbGreen    << endl;
				cout << "strPla[i].rgbRed"      << strPla[i].rgbRed      << endl;
			} 
		}
		else if(strInfo.biBitCount == 32) // 4 channels
		{
			for(unsigned int i=0; i<strInfo.biClrUsed; i++)
			{
				fread((char *)&strPla[i].rgbBlue,     1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbGreen,    1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbRed,      1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbReserved, 1, sizeof(BYTE), fpi);
				
				cout << "strPla[i].rgbBlue"     << strPla[i].rgbBlue     << endl;
				cout << "strPla[i].rgbGreen"    << strPla[i].rgbGreen    << endl;
				cout << "strPla[i].rgbRed"      << strPla[i].rgbRed      << endl;
				cout << "strPla[i].rgbReserved" << strPla[i].rgbReserved << endl;
			} 
		}
		else
		{
			cout << "Can not read color table" << endl;
		}
		
		
		// Image Processing
		cout << "== Image Processing ==" << endl;
		
		
		/* Select the processing mode
		-f or -F: Horizontal Flip
		-q or -Q: Quantization Resolution
		-u or -U: Up-Scaling
		-d or -D: Down-Scaling
		*/
		if(mode == "-f" || mode == "-F") // Horizontal Flip
		{
			cout << "Horizontal Flip Start" << endl;
			if(flipHorizontal(fpi))
			{
				cout << endl;
				cout << "Horizontal Flip Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Horizontal Flip Failed!" << endl;
				return -1;
			}	
		}
		else if(mode == "-q" || mode == "-Q") // Quantization Resolution
		{
			cout << "Qunatization Resolution Start" << endl;
			// cout << "Enter the bit of quantiation (<=8): " << endl;
			
			// cin >> quanBit;
			if(quanBit <= 8)
			{
				if(quanResolution(fpi, quanBit))
				{
					cout << endl;
					cout << "Qunatization Resolution Success!" << endl;
				}
				else
				{
					cout << endl;
					cout << "Qunatization Resolution Failed!" << endl;
					return -1;
				}	
			}
			else
			{
				cout << "Qunatization Resolution Failed!" << endl;
				return -1;
			}	
		}
		else if(mode == "-u" || mode == "-U") // Up-Scaling
		{
			cout << "Up-Scaling" << endl;
			cout << "Enter the up-scaling ratio (>1): " << endl;
			cin >> scaling;
			if(upScaling(fpi))
			{
				cout << endl;
				cout << "Up-scaling Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Up-scaling Failed!" << endl;
				return -1;
			}
		}
		else if(mode == "-d" || mode == "-D") // Down-Scaling
		{
			cout << "Down-Scaling" << endl;
			cout << "Enter the down-scaling ratio (>1): " << endl;
			cin >> scaling;
			if(downScaling(fpi))
			{
				cout << endl;
				cout << "Down-scaling Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Down-scaling Failed!" << endl;
				return -1;
			}
		}
		else
		{
			cout << "Mode error!" << endl;
			return -1;
		}
		
		fclose(fpi);	
	}
	else
	{
		cout << "File open error!" << endl;
		return NULL;
	}
	
	/////////
	image_num = 1;
	quanBit = 2;
	// Open bitmap file
	
	fpi = fopen("./input1.bmp","rb");
	if(fpi != NULL)
	{
		WORD bfType;
		cout << "Loading the file..." << endl;
		fread(&bfType, 1, sizeof(WORD), fpi);
		
		// Check if the file is bmp file
		if(bfType != BM) 
		{
			cout << "The file is not a bmp file!" << endl;
			return NULL;
		}
		
		
		// Read bitmap file header
		fread(&strHead, 1, sizeof(BMPFILEHEADER), fpi);
		showBMPHead(strHead);
		
		// Read bitmap info header
		fread(&strInfo, 1, sizeof(BMPINFOHEADER), fpi);
		showBMPInfoHead(strInfo);
		
		// Read palettue
		cout << "==== Read palette ===="<< endl;
		cout << "biClrUsed: " << strInfo.biClrUsed << endl << endl;
		
		if(strInfo.biBitCount == 24) // 3 channels
		{
			for(unsigned int i=0; i<strInfo.biClrUsed; i++)
			{
				fread((char *)&strPla[i].rgbBlue,  1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbGreen, 1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbRed,   1, sizeof(BYTE), fpi);
				
				cout << "strPla[i].rgbBlue"     << strPla[i].rgbBlue     << endl;
				cout << "strPla[i].rgbGreen"    << strPla[i].rgbGreen    << endl;
				cout << "strPla[i].rgbRed"      << strPla[i].rgbRed      << endl;
			} 
		}
		else if(strInfo.biBitCount == 32) // 4 channels
		{
			for(unsigned int i=0; i<strInfo.biClrUsed; i++)
			{
				fread((char *)&strPla[i].rgbBlue,     1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbGreen,    1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbRed,      1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbReserved, 1, sizeof(BYTE), fpi);
				
				cout << "strPla[i].rgbBlue"     << strPla[i].rgbBlue     << endl;
				cout << "strPla[i].rgbGreen"    << strPla[i].rgbGreen    << endl;
				cout << "strPla[i].rgbRed"      << strPla[i].rgbRed      << endl;
				cout << "strPla[i].rgbReserved" << strPla[i].rgbReserved << endl;
			} 
		}
		else
		{
			cout << "Can not read color table" << endl;
		}
		
		
		// Image Processing
		cout << "== Image Processing ==" << endl;
		
		
		/* Select the processing mode
		-f or -F: Horizontal Flip
		-q or -Q: Quantization Resolution
		-u or -U: Up-Scaling
		-d or -D: Down-Scaling
		*/
		if(mode == "-f" || mode == "-F") // Horizontal Flip
		{
			cout << "Horizontal Flip Start" << endl;
			if(flipHorizontal(fpi))
			{
				cout << endl;
				cout << "Horizontal Flip Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Horizontal Flip Failed!" << endl;
				return -1;
			}	
		}
		else if(mode == "-q" || mode == "-Q") // Quantization Resolution
		{
			cout << "Qunatization Resolution Start" << endl;
			// cout << "Enter the bit of quantiation (<=8): " << endl;
			
			// cin >> quanBit;
			if(quanBit <= 8)
			{
				if(quanResolution(fpi, quanBit))
				{
					cout << endl;
					cout << "Qunatization Resolution Success!" << endl;
				}
				else
				{
					cout << endl;
					cout << "Qunatization Resolution Failed!" << endl;
					return -1;
				}	
			}
			else
			{
				cout << "Qunatization Resolution Failed!" << endl;
				return -1;
			}	
		}
		else if(mode == "-u" || mode == "-U") // Up-Scaling
		{
			cout << "Up-Scaling" << endl;
			cout << "Enter the up-scaling ratio (>1): " << endl;
			cin >> scaling;
			if(upScaling(fpi))
			{
				cout << endl;
				cout << "Up-scaling Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Up-scaling Failed!" << endl;
				return -1;
			}
		}
		else if(mode == "-d" || mode == "-D") // Down-Scaling
		{
			cout << "Down-Scaling" << endl;
			cout << "Enter the down-scaling ratio (>1): " << endl;
			cin >> scaling;
			if(downScaling(fpi))
			{
				cout << endl;
				cout << "Down-scaling Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Down-scaling Failed!" << endl;
				return -1;
			}
		}
		else
		{
			cout << "Mode error!" << endl;
			return -1;
		}
		
		fclose(fpi);	
	}
	else
	{
		cout << "File open error!" << endl;
		return NULL;
	}
	
	
	/////////
	image_num = 2;
	quanBit = 6;
	// Open bitmap file
	
	fpi = fopen("./input2.bmp","rb");
	if(fpi != NULL)
	{
		WORD bfType;
		cout << "Loading the file..." << endl;
		fread(&bfType, 1, sizeof(WORD), fpi);
		
		// Check if the file is bmp file
		if(bfType != BM) 
		{
			cout << "The file is not a bmp file!" << endl;
			return NULL;
		}
		
		
		// Read bitmap file header
		fread(&strHead, 1, sizeof(BMPFILEHEADER), fpi);
		showBMPHead(strHead);
		
		// Read bitmap info header
		fread(&strInfo, 1, sizeof(BMPINFOHEADER), fpi);
		showBMPInfoHead(strInfo);
		
		// Read palettue
		cout << "==== Read palette ===="<< endl;
		cout << "biClrUsed: " << strInfo.biClrUsed << endl << endl;
		
		if(strInfo.biBitCount == 24) // 3 channels
		{
			for(unsigned int i=0; i<strInfo.biClrUsed; i++)
			{
				fread((char *)&strPla[i].rgbBlue,  1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbGreen, 1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbRed,   1, sizeof(BYTE), fpi);
				
				cout << "strPla[i].rgbBlue"     << strPla[i].rgbBlue     << endl;
				cout << "strPla[i].rgbGreen"    << strPla[i].rgbGreen    << endl;
				cout << "strPla[i].rgbRed"      << strPla[i].rgbRed      << endl;
			} 
		}
		else if(strInfo.biBitCount == 32) // 4 channels
		{
			for(unsigned int i=0; i<strInfo.biClrUsed; i++)
			{
				fread((char *)&strPla[i].rgbBlue,     1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbGreen,    1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbRed,      1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbReserved, 1, sizeof(BYTE), fpi);
				
				cout << "strPla[i].rgbBlue"     << strPla[i].rgbBlue     << endl;
				cout << "strPla[i].rgbGreen"    << strPla[i].rgbGreen    << endl;
				cout << "strPla[i].rgbRed"      << strPla[i].rgbRed      << endl;
				cout << "strPla[i].rgbReserved" << strPla[i].rgbReserved << endl;
			} 
		}
		else
		{
			cout << "Can not read color table" << endl;
		}
		
		
		// Image Processing
		cout << "== Image Processing ==" << endl;
		
		
		/* Select the processing mode
		-f or -F: Horizontal Flip
		-q or -Q: Quantization Resolution
		-u or -U: Up-Scaling
		-d or -D: Down-Scaling
		*/
		if(mode == "-f" || mode == "-F") // Horizontal Flip
		{
			cout << "Horizontal Flip Start" << endl;
			if(flipHorizontal(fpi))
			{
				cout << endl;
				cout << "Horizontal Flip Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Horizontal Flip Failed!" << endl;
				return -1;
			}	
		}
		else if(mode == "-q" || mode == "-Q") // Quantization Resolution
		{
			cout << "Qunatization Resolution Start" << endl;
			// cout << "Enter the bit of quantiation (<=8): " << endl;
			
			// cin >> quanBit;
			if(quanBit <= 8)
			{
				if(quanResolution(fpi, quanBit))
				{
					cout << endl;
					cout << "Qunatization Resolution Success!" << endl;
				}
				else
				{
					cout << endl;
					cout << "Qunatization Resolution Failed!" << endl;
					return -1;
				}	
			}
			else
			{
				cout << "Qunatization Resolution Failed!" << endl;
				return -1;
			}	
		}
		else if(mode == "-u" || mode == "-U") // Up-Scaling
		{
			cout << "Up-Scaling" << endl;
			cout << "Enter the up-scaling ratio (>1): " << endl;
			cin >> scaling;
			if(upScaling(fpi))
			{
				cout << endl;
				cout << "Up-scaling Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Up-scaling Failed!" << endl;
				return -1;
			}
		}
		else if(mode == "-d" || mode == "-D") // Down-Scaling
		{
			cout << "Down-Scaling" << endl;
			cout << "Enter the down-scaling ratio (>1): " << endl;
			cin >> scaling;
			if(downScaling(fpi))
			{
				cout << endl;
				cout << "Down-scaling Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Down-scaling Failed!" << endl;
				return -1;
			}
		}
		else
		{
			cout << "Mode error!" << endl;
			return -1;
		}
		
		fclose(fpi);	
	}
	else
	{
		cout << "File open error!" << endl;
		return NULL;
	}
	
	/////////
	image_num = 2;
	quanBit = 4;
	// Open bitmap file
	
	fpi = fopen("./input2.bmp","rb");
	if(fpi != NULL)
	{
		WORD bfType;
		cout << "Loading the file..." << endl;
		fread(&bfType, 1, sizeof(WORD), fpi);
		
		// Check if the file is bmp file
		if(bfType != BM) 
		{
			cout << "The file is not a bmp file!" << endl;
			return NULL;
		}
		
		
		// Read bitmap file header
		fread(&strHead, 1, sizeof(BMPFILEHEADER), fpi);
		showBMPHead(strHead);
		
		// Read bitmap info header
		fread(&strInfo, 1, sizeof(BMPINFOHEADER), fpi);
		showBMPInfoHead(strInfo);
		
		// Read palettue
		cout << "==== Read palette ===="<< endl;
		cout << "biClrUsed: " << strInfo.biClrUsed << endl << endl;
		
		if(strInfo.biBitCount == 24) // 3 channels
		{
			for(unsigned int i=0; i<strInfo.biClrUsed; i++)
			{
				fread((char *)&strPla[i].rgbBlue,  1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbGreen, 1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbRed,   1, sizeof(BYTE), fpi);
				
				cout << "strPla[i].rgbBlue"     << strPla[i].rgbBlue     << endl;
				cout << "strPla[i].rgbGreen"    << strPla[i].rgbGreen    << endl;
				cout << "strPla[i].rgbRed"      << strPla[i].rgbRed      << endl;
			} 
		}
		else if(strInfo.biBitCount == 32) // 4 channels
		{
			for(unsigned int i=0; i<strInfo.biClrUsed; i++)
			{
				fread((char *)&strPla[i].rgbBlue,     1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbGreen,    1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbRed,      1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbReserved, 1, sizeof(BYTE), fpi);
				
				cout << "strPla[i].rgbBlue"     << strPla[i].rgbBlue     << endl;
				cout << "strPla[i].rgbGreen"    << strPla[i].rgbGreen    << endl;
				cout << "strPla[i].rgbRed"      << strPla[i].rgbRed      << endl;
				cout << "strPla[i].rgbReserved" << strPla[i].rgbReserved << endl;
			} 
		}
		else
		{
			cout << "Can not read color table" << endl;
		}
		
		
		// Image Processing
		cout << "== Image Processing ==" << endl;
		
		
		/* Select the processing mode
		-f or -F: Horizontal Flip
		-q or -Q: Quantization Resolution
		-u or -U: Up-Scaling
		-d or -D: Down-Scaling
		*/
		if(mode == "-f" || mode == "-F") // Horizontal Flip
		{
			cout << "Horizontal Flip Start" << endl;
			if(flipHorizontal(fpi))
			{
				cout << endl;
				cout << "Horizontal Flip Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Horizontal Flip Failed!" << endl;
				return -1;
			}	
		}
		else if(mode == "-q" || mode == "-Q") // Quantization Resolution
		{
			cout << "Qunatization Resolution Start" << endl;
			// cout << "Enter the bit of quantiation (<=8): " << endl;
			
			// cin >> quanBit;
			if(quanBit <= 8)
			{
				if(quanResolution(fpi, quanBit))
				{
					cout << endl;
					cout << "Qunatization Resolution Success!" << endl;
				}
				else
				{
					cout << endl;
					cout << "Qunatization Resolution Failed!" << endl;
					return -1;
				}	
			}
			else
			{
				cout << "Qunatization Resolution Failed!" << endl;
				return -1;
			}	
		}
		else if(mode == "-u" || mode == "-U") // Up-Scaling
		{
			cout << "Up-Scaling" << endl;
			cout << "Enter the up-scaling ratio (>1): " << endl;
			cin >> scaling;
			if(upScaling(fpi))
			{
				cout << endl;
				cout << "Up-scaling Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Up-scaling Failed!" << endl;
				return -1;
			}
		}
		else if(mode == "-d" || mode == "-D") // Down-Scaling
		{
			cout << "Down-Scaling" << endl;
			cout << "Enter the down-scaling ratio (>1): " << endl;
			cin >> scaling;
			if(downScaling(fpi))
			{
				cout << endl;
				cout << "Down-scaling Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Down-scaling Failed!" << endl;
				return -1;
			}
		}
		else
		{
			cout << "Mode error!" << endl;
			return -1;
		}
		
		fclose(fpi);	
	}
	else
	{
		cout << "File open error!" << endl;
		return NULL;
	}
	
	
	/////////
	image_num = 2;
	quanBit = 2;
	// Open bitmap file
	
	fpi = fopen("./input2.bmp","rb");
	if(fpi != NULL)
	{
		WORD bfType;
		cout << "Loading the file..." << endl;
		fread(&bfType, 1, sizeof(WORD), fpi);
		
		// Check if the file is bmp file
		if(bfType != BM) 
		{
			cout << "The file is not a bmp file!" << endl;
			return NULL;
		}
		
		
		// Read bitmap file header
		fread(&strHead, 1, sizeof(BMPFILEHEADER), fpi);
		showBMPHead(strHead);
		
		// Read bitmap info header
		fread(&strInfo, 1, sizeof(BMPINFOHEADER), fpi);
		showBMPInfoHead(strInfo);
		
		// Read palettue
		cout << "==== Read palette ===="<< endl;
		cout << "biClrUsed: " << strInfo.biClrUsed << endl << endl;
		
		if(strInfo.biBitCount == 24) // 3 channels
		{
			for(unsigned int i=0; i<strInfo.biClrUsed; i++)
			{
				fread((char *)&strPla[i].rgbBlue,  1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbGreen, 1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbRed,   1, sizeof(BYTE), fpi);
				
				cout << "strPla[i].rgbBlue"     << strPla[i].rgbBlue     << endl;
				cout << "strPla[i].rgbGreen"    << strPla[i].rgbGreen    << endl;
				cout << "strPla[i].rgbRed"      << strPla[i].rgbRed      << endl;
			} 
		}
		else if(strInfo.biBitCount == 32) // 4 channels
		{
			for(unsigned int i=0; i<strInfo.biClrUsed; i++)
			{
				fread((char *)&strPla[i].rgbBlue,     1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbGreen,    1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbRed,      1, sizeof(BYTE), fpi);
				fread((char *)&strPla[i].rgbReserved, 1, sizeof(BYTE), fpi);
				
				cout << "strPla[i].rgbBlue"     << strPla[i].rgbBlue     << endl;
				cout << "strPla[i].rgbGreen"    << strPla[i].rgbGreen    << endl;
				cout << "strPla[i].rgbRed"      << strPla[i].rgbRed      << endl;
				cout << "strPla[i].rgbReserved" << strPla[i].rgbReserved << endl;
			} 
		}
		else
		{
			cout << "Can not read color table" << endl;
		}
		
		
		// Image Processing
		cout << "== Image Processing ==" << endl;
		
		
		/* Select the processing mode
		-f or -F: Horizontal Flip
		-q or -Q: Quantization Resolution
		-u or -U: Up-Scaling
		-d or -D: Down-Scaling
		*/
		if(mode == "-f" || mode == "-F") // Horizontal Flip
		{
			cout << "Horizontal Flip Start" << endl;
			if(flipHorizontal(fpi))
			{
				cout << endl;
				cout << "Horizontal Flip Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Horizontal Flip Failed!" << endl;
				return -1;
			}	
		}
		else if(mode == "-q" || mode == "-Q") // Quantization Resolution
		{
			cout << "Qunatization Resolution Start" << endl;
			// cout << "Enter the bit of quantiation (<=8): " << endl;
			
			// cin >> quanBit;
			if(quanBit <= 8)
			{
				if(quanResolution(fpi, quanBit))
				{
					cout << endl;
					cout << "Qunatization Resolution Success!" << endl;
				}
				else
				{
					cout << endl;
					cout << "Qunatization Resolution Failed!" << endl;
					return -1;
				}	
			}
			else
			{
				cout << "Qunatization Resolution Failed!" << endl;
				return -1;
			}	
		}
		else if(mode == "-u" || mode == "-U") // Up-Scaling
		{
			cout << "Up-Scaling" << endl;
			cout << "Enter the up-scaling ratio (>1): " << endl;
			cin >> scaling;
			if(upScaling(fpi))
			{
				cout << endl;
				cout << "Up-scaling Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Up-scaling Failed!" << endl;
				return -1;
			}
		}
		else if(mode == "-d" || mode == "-D") // Down-Scaling
		{
			cout << "Down-Scaling" << endl;
			cout << "Enter the down-scaling ratio (>1): " << endl;
			cin >> scaling;
			if(downScaling(fpi))
			{
				cout << endl;
				cout << "Down-scaling Success!" << endl;
			}
			else
			{
				cout << endl;
				cout << "Down-scaling Failed!" << endl;
				return -1;
			}
		}
		else
		{
			cout << "Mode error!" << endl;
			return -1;
		}
		
		fclose(fpi);	
	}
	else
	{
		cout << "File open error!" << endl;
		return NULL;
	}
	
	return 0;
}













