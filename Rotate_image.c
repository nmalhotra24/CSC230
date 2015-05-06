#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXROW 50
#define MAXCOL 50
#define AP '&' /* 1 becomes '&' */
#define PL '+' /* 0 becomes '+' */

/* Global variable */
FILE *fpin1; /*pointer to the input file */


/* ****************************************************************** */
/* **************** FILE *OpenInputFile(*fileName) ****************** */
/*
 ----------------------------------------------------------------------
 Purpose:
 Opens a given file for input. Returns the file pointer
 ----------------------------------------------------------------------
 */

FILE *OpenInputFile(char *fileName)
{
	FILE	*result;
	result = fopen(fileName, "r");  /* open the file for reading */
	if (result == NULL)   			/* an error opening the file? */
		fprintf(stderr, "cannot read %s\n", fileName);
	return (result);
}


/* ****************************************************************** */
/* **************** void PrImage(Image, Nrows,Ncols) ***************** */
/*
 ----------------------------------------------------------------------
 Purpose:
 Prints a 2D array row by row.
 ----------------------------------------------------------------------
 */

void PrImage( char Image[MAXROW][MAXCOL], int Nrows, int Ncols)
{
	int ii, jj;
	for(ii = 0; ii < Nrows; ii++)
    {
		for(jj = 0; jj < Ncols; jj++)
        {
			fprintf(stdout, " %2c", Image[ii][jj]);
		}
		fprintf(stdout, "\n");
	}
}


/* ****************************************************************** */
/* ************ void RotR(Image1, Image2, Nrows, Ncols) ************* */
/*
 ----------------------------------------------------------------------
 Purpose:
 Given the 2D char array of Image1 and its dimensions, construct the 
 rotated by 90 degree image in Image2.
 ----------------------------------------------------------------------
 Method:
 Loop for each column of Image1. Copy that column into a row of Image 2
 by calling CopyColRow
 ----------------------------------------------------------------------
 Note:
 Dimensions of Image2 are inverted from Image1. It may call CopyColRow 
 (if so chosen) in a loop to copy each column of Image1 into its 
 corresponding row into Image 2
 ----------------------------------------------------------------------
 */

void RotR( char Image1[MAXROW][MAXCOL], char Image2[MAXROW][MAXCOL],
          int Nrows, int Ncols)
{
    int i, j;
    for (i = 0; i < Ncols; i++)
    {
        for (j = 0; j < Nrows; j++)
        {
            /* rotating the image 90 degrees to the right */
            /* and copying it to the Image2 */
            Image2[i][Nrows-1-j] = Image1[j][i];
        }
    }
}


/* ****************************************************************** */
/* ********** void ScaleUp2(Image1, Image2, Nrows, Ncols) *********** */
/*
 ----------------------------------------------------------------------
 Purpose:
 Given the 2D char array of Image 1 and its dimensions, construct 
 Image 2 of twice its size.
 ----------------------------------------------------------------------
 Method:
 Loop for each element [i][j] of Image 1 starting at i=0 and j=0 and
 let r=0 and c=0 be the indeces into elements [r][c] of Image2.
 Each element at position [i][j] of Image1 must be replicated into 4 
 positions of Image2: Image2[r][c], Image2[r][c+1],
                    Image2[r+1][c], Image2[r+1][c+1]
 Increase or reset r and c appropriately where needed.
 ----------------------------------------------------------------------
 */

void ScaleUp2( char Image1[MAXROW][MAXCOL], char Image2[MAXROW][MAXCOL],
              int Nrows, int Ncols)
{
    int i,j;
    int r = 0; /* rows of the new Image2 */
    int c = 0; /* columns of the new Image2 */
    
    for (i=0; i < Nrows; i++)
    {
        for (j = 0; j < Ncols; j++)
        {
            Image2[r][c] = Image1[i][j];
            Image2[r][c+1] = Image1[i][j];
            Image2[r+1][c] = Image1[i][j];
            Image2[r+1][c+1] = Image1[i][j];
            c = c + 2;
        }
        r = r + 2;
        c = 0;
    }
}


/* ****************************************************************** */
/* ********** void ScaleUp2a(Image1, Image2, Nrows, Ncols) *********** */
/*
 ----------------------------------------------------------------------
 Purpose:
 Given the 2D char array of Image 1 and its dimensions, construct
 Image 2 of twice its size.
 ----------------------------------------------------------------------
 Method:
 1. Makes the elements of the 2D array into one 1D vector and operates 
 on it. Uses an interpolation algorithm based on simple near-neighbour to
 replicate each character/pixel into 4 of them in a square. Takes advantage
 of the storage viewed as a one 1D vector and interpolates each elements 
 into another 1D vector which can be viewed as the correct 2D enlarged image.
 2. Given the static allocation used here, the 2D image must first be stored 
 as the expected 1D vector on which the algorithm can then operate. Not 
 efficient, but useful for this programming exercise.
 3. The original near-neighbour algorithm uses the x_ratio and y_ratio 
 (which can be arbitrary) as floating point numbers. This version replaces 
 all floating point variables with integers and multiplies the original
 values by 65636=2^16 before calculating the scaling ratios. The << operator 
 is the bitwise shift left operator, and x<<16 is equivalent to x*65536, 
 since shifting left by 16 bits is equivalent to multiplication by 2^16 in 
 binary. The >>16 is equivalent to division by 65536. This is one way of 
 extending integers and using the lower 16 bits as equivalent to the values
 of the fractional part.
 ----------------------------------------------------------------------
 */

void ScaleUp2a(char Image1[MAXROW][MAXCOL],char Image2[MAXROW][MAXCOL],
               int Nrows, int Ncols)
{
	int i,j;
	char temp1[MAXROW*MAXCOL];
	char temp2[(MAXROW*2)*(MAXCOL*2)];
	int x_ratio,y_ratio,x2,y2,Nrows2,Ncols2;
    
	Nrows2=Nrows*2;
	Ncols2=Ncols*2;
    
	/*Copy current elements of 2D image into the 1D vector temp1*/
    for (i = 0; i < Nrows; i++)
    {
        for (j = 0; j < Ncols; j++)
        {
            temp1[Nrows*Ncols] = Image1[i][j];
        }
    }
    
	/*Compute the scaling ratio as integers (here always
     doubling the size however) */
	/*Note: added +1 to account for rounding problem*/
	/*Precise conversion from floating to int*/
	x_ratio = (int)((Ncols<<16)/Ncols2)+1;
	y_ratio = (int)((Nrows<<16)/Nrows2)+1;
    
	/*Compose the new 1D vector of correct size - twice here*/
	for (i=0;i<(Nrows2);i++) {
		for (j=0;j<Ncols2;j++) {
			x2 = ((j*x_ratio)>>16);
			y2 = ((i*y_ratio)>>16);
			temp2[(i*Ncols2)+j] = temp1[(y2*Ncols)+x2];
		}
	}
    
	/*Copy the new 1D longer vector temp2 into new 2D scaled up Image2*/
  
}


/* ****************************************************************** */
/* ******************* int RdSize(*Nrows,*Ncols) ******************** */
/*
 ----------------------------------------------------------------------
 Purpose:
 Read from an input file two integers for the number of rows and number 
 of columns of the image to be processed.
 ----------------------------------------------------------------------
 Method:
 Read Row dimension into Nrows. 
 If not end-of-file (as returned by scanf) then read Column size into Ncols.
 Return end-of-file flag to caller function.
 ----------------------------------------------------------------------
 Note:
 scanf returns an integer equal to the number of integers read in - in 
 this case it should be 1. In case of end of file, it returns an 
 integer < 0 (probably -1).
 ----------------------------------------------------------------------
 */

int RdSize(FILE *fpin1,int *Nrows, int *Ncols)
{
    
    int eof_flag = 1;
    
    eof_flag = fscanf (fpin1,"%d", Nrows);
    if (eof_flag == -1)
    {
        return -1;
    } else
    {
        fscanf(fpin1,"%d", Ncols);
    }
    return eof_flag;
               
}


/* ****************************************************************** */
/* **************** void RdImage(Image,Nrows,Ncols) ***************** */
/* 
   ----------------------------------------------------------------------
   Purpose:
   Read from an input file the integers describing the image to be 
   processed and store the corresponding character in the 2D array.
   ----------------------------------------------------------------------
   Method:
   Loop through the 2D Image for each element [i][j]
   Read an integer from a file using scanf
   If the integer = 0, then place character PL into Image[i][j]
   Else place character AP.
   ----------------------------------------------------------------------
   Note:
   Input file is guaranteed to contain the correct number of elements and 
   thus there is no need to check here for end of file
   ---------------------------------------------------------------------- 
 */

void RdImage(FILE *fpin1,char Image1[MAXROW][MAXCOL],int Nrows, int Ncols)
{
    int result,value,i,j;
    
    /* Looping through the 2D Image and replacing each element with either */
    /* AP or PL */
    for (i = 0; i < Nrows; i++)
    {
        for (j = 0; j < Ncols; j++)
        {
            result = fscanf(fpin1, "%d", &value);
            if (result != 1) return; /* error */
            if (value == 0)
            {
                Image1[i][j] = PL; /* integer is 0, so place PL in the Image */
            } else
            {
                Image1[i][j] = AP; /* integer is 1, so place AP in the Image */
            }
        }
    }
}


int main()
{
    int eof; /*end of file*/
    int Rsize1, Csize1;
    char IMchr1 [MAXROW][MAXCOL]; /*Original image*/
    char IMchrUP[MAXROW][MAXCOL]; /*resulting image after scaling up*/
	char IMchrRot[MAXROW][MAXCOL]; /*resulting image after rotating*/
    
    fprintf(stdout, "\n");
	fprintf(stdout, "Hello: \n\n");		/*start of program*/
	/*open all input and output files*/
	fpin1 = OpenInputFile("Image1.txt"); /*file name hardcoded here*/
	if (fpin1 == NULL)
    {
		fprintf(stdout, "Cannot open input file - Bye\n");
		return(0); /*if problem, exit program*/
	}
    
	fprintf(stdout, " Nikita Malhotra - Student Number V00790483 \n");
	fprintf(stdout, " File = A1csc230.c - Spring 2014 \n");
	fprintf(stdout, " CSC 230, Assignment 1, Part 2 \n\n");
	fprintf(stdout, "Starting: \n");
    
	/*Read in the dimensions for the image*/
	eof = RdSize(fpin1,&Rsize1,&Csize1);
    
	while (eof > 0) {
		/*Read in the image*/
		RdImage(fpin1,IMchr1,Rsize1,Csize1);
        
		/*Print the initial image*/
		fprintf(stdout, "\n \n Initial IMchr1 contains: \n");
		PrImage(IMchr1,Rsize1,Csize1);
        
        /*Rotate original image right by 90 degrees and print*/
        RotR(IMchr1,IMchrRot,Rsize1,Csize1);
		fprintf(stdout, "\n Rotated IMchrRot contains: \n");
		PrImage(IMchrRot,Csize1,Rsize1);
        
		/*Scale image up to twice the size*/
		ScaleUp2(IMchr1,IMchrUP,Rsize1,Csize1);
		fprintf(stdout, "\n Scaled Up IMchrUP contains: \n");
		PrImage(IMchrUP,Rsize1*2,Csize1*2);
        
		/*Scale image up to twice the size using the second algorithm*/
		ScaleUp2a(IMchr1,IMchrUP,Rsize1,Csize1);
		fprintf(stdout, "\n Scaled Up (NEW) IMchrUP contains: \n");
		PrImage(IMchrUP,Rsize1*2,Csize1*2);
        
		/* read the size for the next image, if any*/
        eof = RdSize(fpin1,&Rsize1,&Csize1);
	}
    
	/* Closure */
	fprintf(stdout, "\n The program is all done - Bye! \n");
    
	fclose(fpin1);  /* close the files */
    
	return (0);
}

