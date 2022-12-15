/**************************************************************************/
// DATE: Dec 10, 2022
// AUTHOR: Kuan-Wei Chen / NYCU IEE Oasis Lab / 310510221
// CODE TYPE: CPP
// DESCRIPTION: 2022 Fall Digital Image Procssing / Homework4
// MODIFICATION HISTORY: 
// Date                 Description
//
/**************************************************************************/

[Enviroment]
 VSCode
 gcc.exe (MinGW-W64 x86_64-posix-seh, built by Brecht Sanders) 11.2.0

[Requirement] (important!)
 opencv-4.6.0-vc14_vc15
 mingw-w64-v10.0.0

[File Description]
 ./hw4_310510221
	- image1_orgi.bmp
	- input1.bmp
	- input2.bmp
	- bitmap.h
	- CMakeLists.txt
		// link opencv

	- dip_hw4_report_310510221.pdf
		// This file is the DIP hw3 report

	- main.cpp
		// This file is for image restoration
		// Method1 (suggestion - need to link opencv)
		// Step 1: Build the selected target: [ALL Build] 
		// Step 2: Launch the selected target in terminal
		// Step 3: Check the results 
				output1.bmp
				output2.bmp
		
		// Method2
		// Step1: g++ -std=c++11 main.cpp -o main.o
		// Step2: main.o

	- psnr.cpp
		// Calculate PSNR
		g++ -std=c++11 psnr.cpp -o psnr.o

	- psnr.o
		psnr.o image1_ori.bmp output1.bmp
			

	
	


