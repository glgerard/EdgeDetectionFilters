// ----------------------------------------------------- //
// Reads information from header file                    //
// Allows for reading and writing in PGM P2 - P5 format  //
// Version 1.1 Piercarlo Dondi & Alessandro Gaggia       //
// ----------------------------------------------------- //
#include "imageUtilities.h"


//******************* I/O FUNCTIONS *********************//


//-------------------------------------------------------//
//--------------- Skip Commented Lines ------------------//
//-------------------------------------------------------// 
int skipComments(char* buf, FILE* fp)
{
	int i;
	int loop = 1;
	while(buf[0]=='#')
	{
		loop = 1;
		for(i=1; i<64; i++)
		{
			if(buf[i] == '\n')
				loop = 0;
		}
		while(loop)
		{
			fgets(buf,2,fp);
			if(buf[0] == '\n')
				loop = 0;
		}
		fgets(buf, 64, fp);
	}
    return 0;
}


//---------------------------------------------------------// 
//------------- Create a new empty pgm image --------------//
//---------------------------------------------------------// 
Pgm* newPGM(int width, int height, int max_val)
{	
	Pgm* newPgm = (Pgm*)malloc(1*sizeof(Pgm));
	newPgm->width = width;
	newPgm->height = height;
	newPgm->max_val = max_val;
	newPgm->pixels = (int*) calloc(width*height, sizeof(int));	
	
	return newPgm;
}

//---------------------------------------------------------// 
//------------------ Free Pgm structure -------------------//
//---------------------------------------------------------// 
void freePGM(Pgm** pgm)
{
	free((*pgm)->pixels);
	(*pgm)->pixels = NULL;
	free(*pgm);
	*pgm = NULL;
}



//---------------------------------------------------------// 
//------- Set to zero all the pixels of a pgm image -------//
//---------------------------------------------------------// 
void resetPGM(Pgm* pgm)
{
	int width = pgm->width;
	int height = pgm->height;
	int i;
	 
	// set to zero all the pixels
	for(i=0; i<width*height; i++)
	{
		pgm->pixels[i] = 0;
	}
}

//---------------------------------------------------------//
//--------- Read Pixels From Different FileType -----------//
//---------------------------------------------------------// 
Pgm* readPGM(char* filename)
{
	int binary, width, height, max_val;
	FILE *fp = fopen(filename, "rb");
	Pgm* pgm;
	
	if(fp == NULL)
	{
		fprintf(stderr, "File not found. Please Check.\n");
		return NULL;
	}

	char buffer[64];
	// Read a line till \n or 64 char
	fgets(buffer, 64, fp);
     	
     if(strcmp(buffer,"P2\n")==0) 
     {
	   binary = 0;
	   printf ("\nFORMAT: P2\n");
     }
     else if (strcmp(buffer,"P5\n")==0) 
     {
	   binary = 1;
	   printf ("\nFORMAT: P5\n");
     }
     else 
     {
	   fprintf (stderr, "ERROR: incorrect file format\n\n");
	   fclose (fp);
	   return NULL;
     } 
		
	// Getting Information about the file
	fgets(buffer, 64, fp);

	// Jump commented lines
	skipComments(buffer,fp);

	// Read width, height and max grayscale value
     sscanf(buffer, "%d %d", &width, &height);
     fgets(buffer, 64, fp);
     sscanf(buffer, "%d", &max_val);

	// Printing information on screen
	printf("\nPGM Filename: %s\nPGM Width & Height: %d, %d\nPGM Max Val & Type: %d, %s\n\n", 
		   filename, width, height, max_val, binary ? "P5" : "P2");
	
	// Inizialize pgm
	pgm = newPGM(width, height, max_val);

	// Reading Pixels
	int i;
	if(binary) // P5 case
	{
		for(i=0; i<pgm->width*pgm->height; i++)
		{
			fread(&pgm->pixels[i], sizeof(unsigned char), 1, fp);
		}

	}
	else // P2 case
	{		
		for(i=0; i<pgm->width*pgm->height; i++)
		{
			fscanf(fp, "%s", buffer);
			pgm->pixels[i] = atoi(buffer);
		}
	}
	
	// Ok close the file
	fclose(fp);
	
	printf("\nImage correctly loaded\n");
	
	return pgm;
}

//---------------------------------------------------------//
//--- Write Pixels inside images for Different FileType ---//
//---------------------------------------------------------// 
int writePGM(Pgm* pgm, char* filename)
{
	if(!pgm)
	{
		fprintf(stderr, "Error! No data to write. Please Check.\n");
		return -1;
	}
	
	FILE *fp = fopen(filename, "w");
	fprintf(fp, "P2\n%d %d\n%d\n", pgm->width, pgm->height, pgm->max_val);
	
	int i,j;
    int l=0;
	int width = pgm->width;
	int height = pgm->height;
	
	// Write image
    for(i=0; i<height; i++) {
        for (j=0; j<width; j++) {
            fprintf(fp, "%d ", pgm->pixels[l++]);
        }
        fprintf(fp, "\n");
	}

	printf("\nImage \"%s\" correctly written.\n", filename);
	
	// Ok close the file
	fclose(fp);
	
	return 0;
}



//*************** SOME BASIC OPERATIONS *****************//

//-------------------------------------------------------//
//----- Invert Pixels GrayScale value inside images -----//
//--------------- for Different FileType ----------------//
//-------------------------------------------------------// 
int invertPGM(Pgm* pgmIn, Pgm* pgmOut)
{	
	if(!pgmIn || !pgmOut)
	{
		fprintf(stderr, "Error! No input data. Please Check.\n");
		return -1;
	}

	int i, inv;
	int max = pgmIn->max_val;
	int width = pgmIn->width;
	int height = pgmIn->height;
    
    pgmOut->max_val = max;
		
	// Writing Pixels
	for(i=0; i<width*height; i++)
	{
		// Invert GrayScale Value
		inv = max - pgmIn->pixels[i];
		pgmOut->pixels[i] = inv;
	}	
	
	return 0;
}

//-------------------------------------------------------//
//---------------- Flip Image Horizontally --------------//
//-------------------------------------------------------// 
int hflipPGM(Pgm* pgmIn, Pgm* pgmOut)
{		
	if(!pgmIn || !pgmOut)
	{
		fprintf(stderr, "Error! No input data. Please Check.\n");
		return -1;
	}
	
	int i, j;
	int hfp;
	
	int* inputPixels = pgmIn->pixels;
	int* flipPixels = pgmOut->pixels;
		
	int width = pgmIn->width;
	int height = pgmIn->height;

	// Modify Pixels
	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			// Flip GrayScale Value on width
			hfp = inputPixels[i*width + j];
			flipPixels[i*width + (width -j -1)] = hfp;
		}	
	}
	
	return 0;
}


//-------------------------------------------------------//
//------------------ Copy a PGM Image -------------------//
//-------------------------------------------------------// 
int copyPGM(Pgm* pgmIn, Pgm* pgmOut)
{		

	if(!pgmIn || !pgmOut)
	{
		fprintf(stderr, "Error! No input data. Please Check.\n");
		return -1;
	}
	
	int i;
	
	int* inPixels = pgmIn->pixels;
	int* outPixels = pgmOut->pixels;
		
	int width = pgmIn->width;
	int height = pgmIn->height;

	// Copy image
	for(i=0; i<width*height; i++)
	{
		// Copy image
		outPixels[i] = inPixels[i];
	}
	
	return 0;
}


//--------------------------------------------------------//
//------------------ Calculate Histogram -----------------//
//--------------------------------------------------------// 
int* histogramPGM(Pgm* pgm)
{
	if(!pgm)
	{
		fprintf(stderr, "Error! No input data. Please Check.\n");
		return NULL;
	}
	
	int i,index;
	int width = pgm->width;
	int height = pgm->height;
	int max_val = pgm->max_val;
	
	// if max_val is 255 each pixel of the image can have a value between [0;255]
	// so histogram have a dimension of 256
	int* histogram = (int*)calloc(max_val+1,sizeof(int));

	for(i=0; i<width*height; i++)
	{
		index = pgm->pixels[i];
		histogram[index]++;
	}
	
	return histogram;
}


//--------------------------------------------------------//
//------------------ Normalize the Image -----------------//
//--------------------------------------------------------//
int normalizePGM(Pgm* pgmIn, Pgm* pgmOut)
{
    if(!pgmIn || !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    // compute the histogram
    
    int* histogram = histogramPGM(pgmIn);
    
    // find the min and max values
    int lowVal = 0;
    int topVal = pgmIn->max_val;
    for (int i = 0; i < pgmIn->max_val+1; i++) {
        if (histogram[i] > 0) {
            lowVal = i;
            break;
        }
    }
    for (int i = pgmIn->max_val+1; i > 0; i--) {
        if (histogram[i] > 0) {
            topVal = i;
            break;
        }
    }
    
    for(int i = 0; i<width*height; i++) {
        pgmOut->pixels[i] = (int)255*(pgmIn->pixels[i] - lowVal ) / (topVal-lowVal);
    }
    
    return 0;
}

//--------------------------------------------------------//
//------------------- Equalize the Image -----------------//
//--------------------------------------------------------//
int equalizePGM(Pgm* pgmIn, Pgm* pgmOut)
{
    if(!pgmIn || !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int sum;
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    // compute the histogram
    
    int* histogram = histogramPGM(pgmIn);

    int tot = 0;
    for (int s=0; s<pgmIn->max_val+1; s++) {
        tot += histogram[s];
    }
    for(int i = 0; i<width*height; i++) {
        sum = 0;
        for (int s=0; s <= pgmIn->pixels[i]; s++) {
            sum += histogram[s];
        }
        pgmOut->pixels[i] = (int) 255*sum/tot;
    }
    return 0;
}