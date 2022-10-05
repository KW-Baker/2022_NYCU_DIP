/**************************************************************************/
// FILE NAME: bitmap.h
// VERSRION: 1.0
// DATE: Oct 3, 2022
// AUTHOR: Kuan-Wei Chen / NYCU IEE Oasis Lab / 310510221
// CODE TYPE: CPP(Header)
// DESCRIPTION: 2022 Fall Digital Image Procssing / Homework1
// MODIFICATION HISTORY: 
// Date                 Description
//
/**************************************************************************/

#ifndef bitmap
#define bitmap

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned int   DWORD;
typedef long           LONG;

typedef struct BMPFILEHEADER
{
	//WORD  bfType; // must be 0x4d42
	DWORD bfSize;
	WORD  bfReserved1;
	WORD  bfReserved2;
	DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct BMPINFOHEADER
{
	DWORD biSize;
	LONG  biWidth;
	LONG  biHeight;
	WORD  biPlanes;
	WORD  biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG  biXPelsPerMeter;
	LONG  biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BMPINFOHEADER;

typedef struct RGBQUAD
{
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
	BYTE rgbReserved;
} RGBQUAD;



typedef struct IMAGEDATA_3
{
	BYTE red;
	BYTE green;
	BYTE blue;
} IMAGEDATA_3;

typedef struct IMAGEDATA_4
{
	BYTE red;
	BYTE green;
	BYTE blue;
	BYTE reserved;
} IMAGEDATA_4;

#endif
