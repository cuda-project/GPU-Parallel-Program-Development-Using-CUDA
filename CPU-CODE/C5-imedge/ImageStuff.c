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


// This function calculates the pre-processed pixels while reading from disk
struct PrPixel** PrReadBMP(char* filename)
{
	int i,j,k;
	unsigned char r, g, b;
	float R, G, B, BW, BW2, BW3, BW4, BW5, BW9, BW12, Z=0.0;
	unsigned char Buffer[24576];
	FILE* f = fopen(filename, "rb");
	if(f == NULL){
		printf("\n\n%s NOT FOUND\n\n",filename);
		exit(1);
	}
	unsigned char HeaderInfo[54];
	fread(HeaderInfo, sizeof(unsigned char), 54, f); // read the 54-byte header

	// extract image height and width from header
	int width = *(int*)&HeaderInfo[18];			ip.Hpixels = width;
	int height = *(int*)&HeaderInfo[22];		ip.Vpixels = height;
	int RowBytes = (width*3 + 3) & (~3);		ip.Hbytes  = RowBytes;
	//copy header for re-use
	for(i=0; i<54; i++) {	ip.HeaderInfo[i] = HeaderInfo[i];	}
	printf("\n   Input BMP File name: %20s  (%u x %u)",filename,ip.Hpixels,ip.Vpixels);
	// allocate memory to store the main image
	struct PrPixel **PrIm = (struct PrPixel **)malloc(height * sizeof(struct PrPixel *));
	for(i=0; i<height; i++) {
		PrIm[i] = (struct PrPixel *)malloc(width * sizeof(struct PrPixel));
	}
	// read the image from disk and pre-calculate the PRImage pixels
	for(i = 0; i < height; i++) {
		fread(Buffer, sizeof(unsigned char), RowBytes, f);
		for(j=0,k=0; j<width; j++, k+=3){
			b=PrIm[i][j].B=Buffer[k];		B=(float)b;
			g=PrIm[i][j].G=Buffer[k+1];		G=(float)g;
			r=PrIm[i][j].R=Buffer[k+2];		R=(float)r;
			BW3=R+G+B;
			PrIm[i][j].BW   = BW   = BW3*0.3333333;
			PrIm[i][j].BW2  = BW2  = BW+BW;
			PrIm[i][j].BW4  = BW4  = BW2+BW2;
			PrIm[i][j].BW5  = BW5  = BW4+BW;
			PrIm[i][j].BW9  = BW9  = BW5+BW4;
			PrIm[i][j].BW12 = BW12 = BW9+BW3;
			PrIm[i][j].BW15 = BW12+BW3;
			PrIm[i][j].Gauss = PrIm[i][j].Gauss2 = Z;
			PrIm[i][j].Theta = PrIm[i][j].Gradient = Z;
		}
	}
	fclose(f);
	return PrIm;  // return the pointer to the main image
}
