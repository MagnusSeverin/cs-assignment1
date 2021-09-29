//To compile (linux/mac): gcc cbmp.c main.c -o main.out -std=c99
//To run (linux/mac): ./main.out example.bmp example_inv.bmp

//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): ./main.exe example.bmp example_inv.bmp

#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"

int scope(int x, int y, int erosions, unsigned char eroded_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);
void capture(int x, int y, int erosions, unsigned char eroded_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);
int erosions = 0;
int *ptr = &erosions;

//Function to invert pixels of an image (negative)
void toGreyScale(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS])
{
  int sum;
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      for (int c = 0; c < BMP_CHANNELS; c++)
      {
        sum += input_image[x][y][c];
      }
      output_image[x][y][0] = sum / 3;
      output_image[x][y][1] = sum / 3;
      output_image[x][y][2] = sum / 3;
      sum = 0;
    }
  }
}

void binaryThreshold(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS])
{
  int Th = 90;
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      if (input_image[x][y][0] <= Th)
      {
        input_image[x][y][0] = 0;
        input_image[x][y][1] = 0;
        input_image[x][y][2] = 0;
      }
      else
      {
        input_image[x][y][0] = 255;
        input_image[x][y][1] = 255;
        input_image[x][y][2] = 255;
      }
    }
  }
}

void erosion(int *erosions, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char eroded_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS])
{

  int eroded = 0;

  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      if (input_image[x][y][0] > 0)
      {
        /*
        for(int i = -1; i<2; i++){
            if (x + i >= 0 && x + i < BMP_WIDTH)
          {
            for(int j = -1; j<2; j++){
            if (y + j >= 0 && y + j < BMP_HEIGTH) {
            if (input_image[x + i][y+j][0] == 0)
            {
              eroded = 1;
              break;
            }
            }
            }
          }
        }
        */

        
        for (int i = -1; i < 2; i++)
        {
          if (x + i >= 0 && x + i < BMP_WIDTH)
          {
            if (input_image[x + i][y][0] == 0)
            {
              eroded = 1;
              break;
            }
          }
          if (y + i >= 0 && y + i < BMP_HEIGTH)
          {
            if (input_image[x][y + i][0] == 0)
            {
              eroded = 1;
              break;
            }
          }
        }
        
      }
      if (eroded == 0 && input_image[x][y][0] > 0)
      {

        eroded_image[x][y][0] = 255;
        eroded_image[x][y][1] = 255;
        eroded_image[x][y][2] = 255;
      }
      else
      {
        eroded_image[x][y][0] = 0;
        eroded_image[x][y][1] = 0;
        eroded_image[x][y][2] = 0;
      }
      eroded = 0;
    }
  }

 for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGTH; y++) {
      input_image[x][y][0] = eroded_image[x][y][0];
      input_image[x][y][1] = eroded_image[x][y][1];
      input_image[x][y][2] = eroded_image[x][y][2];
    }
  }

  (*erosions)+1;
}

int detect(unsigned char centers_image[BMP_WIDTH*BMP_HEIGTH],unsigned char eroded_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS])
{
  int finished = 1;
  int loc;


  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      if (eroded_image[x][y][0] > 0)
      {
        finished = 0;
        if (scope(x, y, erosions, eroded_image) == 1)
        {
          capture(x, y, erosions, eroded_image);
          loc = (y*BMP_WIDTH) + x;
          printf("Punkt %d,%d bliver til bit nummer %d\n",x,y,loc);
          centers_image[loc/8] |= (1 << (loc%8));
        }
      }
    }
  }
  return finished;
}

int scope(int x, int y, int erosions, unsigned char eroded_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS])
{

  //int size = 27 - 2 * erosions;
  int size = 19;

  int borderPix = 0;

  for (int i = -((size - 1) / 2); i <= (size - 1) / 2; i = i + size - 1)
  {
    if (x + i >= 0 && x + i < BMP_WIDTH)
    {
      for (int j = -((size - 1) / 2); j <= (size - 1) / 2; j++)
      {
        if (y + j >= 0 && y + j < BMP_HEIGTH)
        {
          if (eroded_image[x + i][y + j][0] > 0)
          {
            return 0;
          }
        }
      }
    }
  }

  for (int j = -((size - 1) / 2); j <= (size - 1) / 2; j = j + size - 1)
  {
    if (y + j >= 0 && y + j < BMP_HEIGTH)
    {
      for (int i = -((size - 1) / 2); i <= (size - 1) / 2; i++)
      {
        if (x + i >= 0 && x + i < BMP_WIDTH)
        {
          if (eroded_image[x + i][y + j][0] > 0)
          {
            return 0;
          }
        }
      }
    }
  }

  /*
  if (borderPix > 0 && borderPix < 4) {

    int framePix = 0; 

    for (int j = -((size - 1) / 2); j <= (size - 1) / 2; j ++)
  {
    if (y + j >= 0 && y + j < BMP_HEIGTH)
    {
      for (int i = -((size - 1) / 2); i <= (size - 1) / 2; i++)
      {
        if (x + i >= 0 && x + i < BMP_WIDTH)
        {
          if (eroded_image[x + i][y + j][0] > 0)
          {
            framePix++;
          }
        }
      }
    }
  }
  if (borderPix*size < framePix) { 
    return 1;
    }
    else {
      return 0;
    }
  }

  if (borderPix > 0) {
    return 0;
  }
  else {
    return 1;
  }
  */
return 1;
}

void capture(int x, int y, int erosions, unsigned char eroded_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS])
{
  //int size = 27 - 2 * erosions;
  int size = 19;

  for (int i = -((size - 1) / 2); i <= (size - 1) / 2; i++)
  {
    if (x + i >= 0 && x + i < BMP_WIDTH)
    {
      for (int j = -((size - 1) / 2); j <= (size - 1) / 2; j++)
      {
        if (y + j >= 0 && y + j < BMP_HEIGTH)
        {
          eroded_image[x + i][y + j][0] = 0;
          eroded_image[x + i][y + j][1] = 0;
          eroded_image[x + i][y + j][2] = 0;
        }
      }
    }
  }
}


void cross(unsigned char centers_image[BMP_WIDTH*BMP_HEIGTH], unsigned char original_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]) {
  int count = 0;
  int xloc;
  int yloc;
  for (int x = 0; x < (BMP_WIDTH*BMP_HEIGTH); x++){
      if (centers_image[x/8] & (1 << (x%8)) != 0) {
        count++;
        xloc = x%BMP_WIDTH;
        yloc = (x-x%BMP_WIDTH)/BMP_WIDTH;
        for (int i = -10; i < 10; i++) {
          for (int j = -10; j < 11; j++) {

          if (yloc+i >= 0 && yloc+i < BMP_HEIGTH) {
            original_image[xloc][yloc+i][0] = 255;
            original_image[xloc][yloc+i][1] = 0;
            original_image[xloc][yloc+i][2] = 0;
            //Den gemmer de samme punkter virkelig mange gange
            printf("celle: %d %d\n",xloc,yloc);
          }

          if (xloc+i >= 0 && xloc+i < BMP_WIDTH) {
          original_image[xloc+i][yloc][0] = 255;
          original_image[xloc+i][yloc][1] = 0;
          original_image[xloc+i][yloc][2] = 0;
          printf("celle: %d %d\n",xloc,yloc);
          }

          }
        }
      }
      //Det her print stikker helt af
  //printf("%d\n",count);
  }
}

//Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char eroded_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char centers_image[BMP_WIDTH*BMP_HEIGTH];

//Main function
int main(int argc, char **argv)
{
  //argc counts how may arguments are passed
  //argv[0] is a string with the name of the program
  //argv[1] is the first command line argument (input image)
  //argv[2] is the second command line argument (output image)

  //Checking that 2 arguments are passed
  if (argc != 4)
  {
    fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
    exit(1);
  }

  printf("Example program - 02132 - A1\n");

  //Load image from file
  read_bitmap(argv[1], input_image);

  //To grey
  toGreyScale(input_image, output_image);

  //to two colors
  binaryThreshold(output_image);

  write_bitmap(output_image, argv[3]);

  int finish = 0;

  while (finish == 0)
  {
    erosion(ptr, output_image, eroded_image);
    finish = detect(centers_image,output_image);
  }

  cross(centers_image,input_image);

  //Save image to file
  write_bitmap(input_image, argv[2]);

  printf("Done!\n");
  return 0;
}
