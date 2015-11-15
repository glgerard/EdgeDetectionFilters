/*! \file imageUtilities.c
 *  \brief Reads information from header file. Allows for reading and writing in PGM P2 - P5 format.
 *  \author Piercarlo Dondi
 *  \author Alessandro Gaggia
 *  \author Gianluca Gerard
 */
 
#include "imageUtilities.h"

//******************* I/O FUNCTIONS *********************//

/*! \fn int skipComments(char* buf, FILE* fp)
 * \brief Skip commented lines.
 * \param buf Pointer to the buffer where the file lines are stored.
 * \param fp Pointer to the image file.
 * \return Always 0
 */
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

/*! \fn Pgm* newPGM(int width, int height, int max_val)
 * \brief Create a new empty pgm image.
 * \param width Image's width.
 * \param height Image's height.
 * \param max_val Maximum pixel value in the image.
 * \return Pointer to the new created image.
 */
Pgm* newPGM(int width, int height, int max_val)
{	
	Pgm* newPgm = (Pgm*)malloc(1*sizeof(Pgm));
	newPgm->width = width;
	newPgm->height = height;
	newPgm->max_val = max_val;
	newPgm->pixels = (int*) calloc(width*height, sizeof(int));	
	
	return newPgm;
}

/*! \fn void freePGM(Pgm** pgm)
 * \brief Free Pgm structure.
 * \param pgm Pointer to a Pgm structure pointer.
 */
void freePGM(Pgm** pgm)
{
	free((*pgm)->pixels);
	(*pgm)->pixels = NULL;
	free(*pgm);
	*pgm = NULL;
}

/*! \fn void resetPGM(Pgm* pgm)
 * \brief Set to zero all the pixels of a pgm image.
 * \param pgm Pointer to a Pgm structure.
 */
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

/*! \fn Pgm* readPGM(char* filename)
 * \brief Read Pixels From Different FileType.
 * \param filename Name of the file with the image.
 * \return Pointer to the Pgm structure containing the read image.
 */
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
	printf("\nPGM Filename: %s\nPGM Width & Height: %d, %d\nPGM Max Val & Type: %d, %s\n", 
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
	
	printf("\nImage \"%s\" correctly loaded.\n", filename);
	
	return pgm;
}

/*! \fn int writePGM(Pgm* pgm, char* filename)
 * \brief Write Pixels inside images for Different FileType.
 * \param pgm Pointer to the Pgm structure with the image.
 * \param filename Name of the file where the image is written.
 * \return 0 on success. -1 if no Pgm structure pointer is provided.
 */
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

/*! \fn int invertPGM(Pgm* pgmIn, Pgm* pgmOut)
 * \brief Invert Pixels GrayScale value inside images.
 * \param pgmIn Pointer to the input Pgm structure with the image.
 * \param pgmOut Pointer to the inverted Pgm structure.
 * \return 0 on success. -1 if no Pgm structure pointer is provided.
 */
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

/*! \fn int hflipPGM(Pgm* pgmIn, Pgm* pgmOut)
 * \brief Flip Image Horizontally.
 * \param pgmIn Pointer to the input Pgm structure with the image.
 * \param pgmOut Pointer to the flipped Pgm structure.
 * \return 0 on success. -1 if no Pgm structure pointer is provided.
 */
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

/*! \fn int copyPGM(Pgm* pgmIn, Pgm* pgmOut)
 * \brief Copy a PGM Image.
 * \param pgmIn Pointer to the input Pgm structure with the image.
 * \param pgmOut Pointer to the copied Pgm structure.
 * \return 0 on success. -1 if no Pgm structure pointer is provided.
 */
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
    
    pgmOut->max_val = pgmIn->max_val;
	
	return 0;
}

/*! \fn int normalizePGM(Pgm* pgmIn, Pgm* pgmOut)
 * \brief Normalize the Image.
 * \param pgmIn Pointer to the input Pgm structure with the image.
 * \param pgmOut Pointer to the Pgm structure with the normalized image.
 * \return 0 on success. -1 if either input pointers is null.
 */
int normalizePGM(Pgm* pgmIn, Pgm* pgmOut)
{
    if(!pgmIn || !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    int top_val = 0;
    
    // compute the histogram
    
    Histogram* histogram = histogramPGM(pgmIn);
    
    // find the min and max values
    int min_val = histogram->min_val;

    for (int i = histogram->size-1; i > 0; i--) {
        if (histogram->channels[i] > 0) {
            top_val = i+min_val;
            break;
        }
    }
    
    for(int i = 0; i<width*height; i++) {
        pgmOut->pixels[i] = (int)255*(pgmIn->pixels[i] - min_val ) / (top_val-min_val);
    }
    
    pgmOut->max_val = 255;

    freeHistogram(&histogram);

    return 0;
}

/*! \fn int equalizePGM(Pgm* pgmIn, Pgm* pgmOut)
 * \brief Equalize the Image.
 * \param pgmIn Pointer to the input Pgm structure with the image.
 * \param pgmOut Pointer to the Pgm structure with the equalized image.
 * \return 0 on success. -1 if either input pointers is null.
 */
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
    
    Histogram* histogram = histogramPGM(pgmIn);

    int tot = 0;
    for (int s=0; s<histogram->size; s++) {
        tot += histogram->channels[s];
    }
    for(int i = 0; i<width*height; i++) {
        sum = 0;
        for (int s=0; s <= pgmIn->pixels[i]; s++) {
            sum += histogram->channels[s];
        }
        pgmOut->pixels[i] = (int) 255*sum/tot;
    }
    
    freeHistogram(&histogram);
    return 0;
}

/*! \fn Histogram* histogramPGM(Pgm* pgm)
 * \brief Computes the Histogram of an image in \a pgm.
 * \param pgm Pointer to the input Pgm structure with the image.
 * \return Pointer to the Histogram structure for the image.
 */
Histogram* histogramPGM(Pgm* pgm)
{
    if(!pgm)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return NULL;
    }
    
    int i,index, pixel;
    int width = pgm->width;
    int height = pgm->height;
    int max_val = pgm->max_val;
    
    Histogram* histo = (Histogram*)calloc(1, sizeof(Histogram));
    histo->max_val = max_val;
    
    int min_val = max_val;
    for(i=0; i<width*height; i++)
    {
        pixel = pgm->pixels[i];
        if ( pixel < min_val )
            min_val = pixel;
    }
    histo->min_val = min_val;
    histo->size = max_val - min_val + 1;
    
    // if max_val is 255 each pixel of the image can have a value between [0;255]
    // so histogram have a dimension of 256
    histo->channels = (int*)calloc(histo->size,sizeof(int));
    
    for(i=0; i<width*height; i++)
    {
        index = pgm->pixels[i]-min_val;
        histo->channels[index]++;
    }
    
    return histo;
}

/*! \fn void freeHistogram(Histogram** handle)
 * \brief Free an Histogram structure.
 * \param handle Pointer to the Histogram structure pointer.
 */
void freeHistogram(Histogram** handle)
{
    free((*handle)->channels);
    (*handle)->channels = NULL;
    free(*handle);
    *handle = NULL;
}