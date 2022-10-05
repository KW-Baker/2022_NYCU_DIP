/**************************************************************************/
// FILE NAME: processing.h
// VERSRION: 1.0
// DATE: Oct 3, 2022
// AUTHOR: Kuan-Wei Chen / NYCU IEE Oasis Lab / 310510221
// CODE TYPE: CPP(Header)
// DESCRIPTION: 2022 Fall Digital Image Procssing / Homework1
// MODIFICATION HISTORY: 
// Date                 Description
//
/**************************************************************************/

#ifndef processing
#define processing

void showBMPHead(BMPFILEHEADER BMPHead);
void showBMPInfoHead(BMPINFOHEADER BMPInfoHead);

// Horizontal flip
bool flipHorizontal(FILE *fpi);

// Resolution Quantization
bool quanResolution(FILE *fpi, unsigned char quanBit);
unsigned char quantizer(unsigned char quanBit, unsigned char src);

// Image Scaling
bool upScaling(FILE *fpi);
bool downScaling(FILE *fpi);
bool bilinear_resize(FILE *fpi);

#endif
