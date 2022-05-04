#define EDGE		 0
#define NOEDGE       255
#define MAXTHREADS   128


struct ImgProp
{
	int Hpixels;
	int Vpixels;
	unsigned char HeaderInfo[54];
	unsigned long int Hbytes;
};

struct Pixel
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
};

struct PrPixel{
	unsigned char 	R;
	unsigned char 	G;
	unsigned char 	B;
	unsigned char 	x;    // unused. to make it an even 4B
	float 			BW;
	float 			BW2,BW4,BW5,BW9,BW12,BW15;
	float			Gauss, Gauss2;
	float			Theta,Gradient;
};

double** CreateBlankDouble();
double** CreateBWCopy(unsigned char** img);
unsigned char** CreateBlankBMP();

void WriteBMP(unsigned char** , char*);

unsigned char** ReadBMP(char* );
struct PrPixel** PrReadBMP(char*);
struct PrPixel** PrAMTReadBMP(char*);

extern struct   ImgProp 	ip;
extern long 	NumThreads, PrThreads;
extern int		ThreadCtr[];

