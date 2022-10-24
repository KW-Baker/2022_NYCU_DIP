/**************************************************************************/
// FILE NAME: bitmap.h
// VERSRION: 1.0
// DATE: Oct 3, 2022
// AUTHOR: Kuan-Wei Chen / NYCU IEE Oasis Lab / 310510221
// CODE TYPE: CPP(Header)
// DESCRIPTION: 2022 Fall Digital Image Procssing / Homework2
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

typedef struct BMPFILEHEADER{
	//WORD  bfType; // must be 0x4d42
	DWORD bfSize;
	WORD  bfReserved1;
	WORD  bfReserved2;
	DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct BMPINFOHEADER{
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

typedef struct RGB{
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;
} RGB;


#endif
