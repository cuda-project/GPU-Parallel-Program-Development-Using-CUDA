#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "ImageStuff.h"


double** CreateBlankDouble()
{
    int i;

	double** img = (double **)malloc(ip.Vpixels * sizeof(double*)); 
    for(i=0; i<ip.Vpixels; i++){
		img[i] = (double *)malloc(ip.Hpixels*sizeof(double));
		memset((void *)img[i],0,(size_t)ip.Hpixels*sizeof(double));
    }
    return img;
}


double** CreateBWCopy(unsigned char** img)
{
    int i,j,k;

	double** imgBW = (double **)malloc(ip.Vpixels * sizeof(double*)); 
    for(i=0; i<ip.Vpixels; i++){
		imgBW[i] = (double *)malloc(ip.Hpixels*sizeof(double));
		for(j=0; j<ip.Hpixels; j++){
			// convert each pixel to B&W = (R+G+B)/3
			k=3*j;
			imgBW[i][j]=((double)img[i][k]+(double)img[i][k+1]+(double)img[i][k+2])/3.0;  
		}
    }	
    return imgBW;
}


unsigned char** CreateBlankBMP()
{
    int i,j;

	unsigned char** img = (unsigned char **)malloc(ip.Vpixels * sizeof(unsigned char*));
    for(i=0; i<ip.Vpixels; i++){
        img[i] = (unsigned char *)malloc(ip.Hbytes * sizeof(unsigned char));
		memset((void *)img[i],0,(size_t)ip.Hbytes); // zero out every pixel
    }
    return img;
}


unsigned char** ReadBMP(char* filename)
{
	int i;
	FILE* f = fopen(filename, "rb");
	if(f == NULL){
		printf("\n\n%s NOT FOUND\n\n",filename);
		exit(1);
	}

	unsigned char HeaderInfo[54];
	fread(HeaderInfo, sizeof(unsigned char), 54, f); // read the 54-byte header

	// extract image height and width from header
	int width = *(int*)&HeaderInfo[18];
	int height = *(int*)&HeaderInfo[22];

	//copy header for re-use
	for(i=0; i<54; i++) {
		ip.HeaderInfo[i] = HeaderInfo[i];
	}

	ip.Vpixels = height;
	ip.Hpixels = width;
	int RowBytes = (width*3 + 3) & (~3);
	ip.Hbytes = RowBytes;

	printf("\n   Input BMP File name: %20s  (%u x %u)",filename,ip.Hpixels,ip.Vpixels);

	unsigned char tmp;
	unsigned char **TheImage = (unsigned char **)malloc(height * sizeof(unsigned char*));
	for(i=0; i<height; i++) {
		TheImage[i] = (unsigned char *)malloc(RowBytes * sizeof(unsigned char));
	}

	for(i = 0; i < height; i++) {
		fread(TheImage[i], sizeof(unsigned char), RowBytes, f);
	}
	fclose(f);
	return TheImage;  // remember to free() it in caller!
}


void WriteBMP(unsigned char** img, char* filename)
{
	int i;
	FILE* f = fopen(filename, "wb");
	if(f == NULL){
		printf("\n\nFILE CREATION ERROR: %s\n\n",filename);
		exit(1);
	}

	unsigned long int x,y;
	unsigned char temp;

	//write header
	for(x=0; x<54; x++) {	fputc(ip.HeaderInfo[x],f);	}

	//write data fputc 
	//.....WriteBMP completed             ...     469 ms
	// for(x=0; x<ip.Vpixels; x++) {
	// 	for(y=0; y<ip.Hbytes; y++){
	// 		temp=img[x][y];
	// 		fputc(temp,f);
	// 	}
	// }

	//write data fwrite
	//.....WriteBMP completed             ...      69 ms
	for(i=0; i<ip.Vpixels; i++) {
		fwrite(img[i], sizeof(unsigned char), ip.Hbytes, f);
	}

	printf("\n  Output BMP File name: %20s  (%u x %u)",filename,ip.Hpixels,ip.Vpixels);
	fclose(f);
}




