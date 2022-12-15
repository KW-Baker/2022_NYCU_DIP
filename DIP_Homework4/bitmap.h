/**************************************************************************/
// FILE NAME: bitmap.h
// VERSRION: 1.0
// DATE: Oct 3, 2022
// AUTHOR: Kuan-Wei Chen / NYCU IEE Oasis Lab / 310510221
// CODE TYPE: CPP(Header)
// DESCRIPTION: 2022 Fall Digital Image Procssing / Homework4
// MODIFICATION HISTORY: 
// Date                 Description
//
/**************************************************************************/
#ifndef bitmap
#define bitmap


#define BYTE  unsigned char
#define WORD  unsigned short
#define DWORD unsigned int

#pragma pack(push)
#pragma pack(2)
typedef struct BMPFILEHEADER{
	WORD  bfType; // must be 0x4d42
	DWORD bfSize;
	WORD  bfReserved1;
	WORD  bfReserved2;
	DWORD bfOffBits;
} BITMAPFILEHEADER;
#pragma pack(pop)

typedef struct BMPINFOHEADER{
	DWORD biSize;
	DWORD biWidth;
	DWORD biHeight;
	WORD  biPlanes;
	WORD  biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	DWORD biXPelsPerMeter;
	DWORD biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BMPINFOHEADER;

typedef struct RGB{
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;
} RGB;


typedef struct RGB_d{
	double b;
	double g;
	double r;
	double a;
} RGB_d;



#endif
