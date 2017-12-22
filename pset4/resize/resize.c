/**
 * Copies a BMP piece by piece, just because.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // START input checking
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize n infile outfile\n");
        return 1;
    }

    // get requested factor
    float factor = atof(argv[1]);
    if (factor <= 0)
    {
        fprintf(stderr, "Please choose a positive number for size.\n");
        return 1;
    }
    //fprintf(stdout, "factor=%f\n", factor);

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    // DONE input checking

    // save info before overwriting
    int origPadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int origbiWidth = bi.biWidth;
    int origbiHeight = abs(bi.biHeight);

    // the bitmapheader and bitmapfileheader changes
    bi.biWidth *= factor; // not including padding
    bi.biHeight *= factor;

    // determine new padding for scanlines - to add
    int newbiWidth = bi.biWidth;
    int newPadding = (4 - (newbiWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + newPadding) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    int numRow = 0;
    int numPix = 0;

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(origbiHeight); i < biHeight; i++)
    {
        // if factor < 1
        //      print those lines/pixels where i % (1/factor) < 1
        // else if factor > 1 and is an integer
        //      print all lines/pixels factor number of times
        // else
        //      print all lines/pixels factor number of times PLUS (formula for factor < 1)

        // calculate if I need to print this row at all
        // and if so, how many times
        if (factor < 1)
        {
            //if (fmod((i+1), (1/factor)) < 1)
            if (fmod(i, (1/factor)) < 1)
                numRow = 1;
            else
                numRow = 0;
        }
        else
        {
            //if ((int)factor != factor && fmod((i+1), (1/factor)) < 1 - (1/factor))
            if ((int)factor != factor && fmod(i, (1/factor)) < 1 - (1/factor))
                numRow = factor + 1;
            else
                numRow = factor;
        }

        // if skipping a row, advance the file position
        if (numRow == 0)
            fseek(inptr, (origbiWidth * sizeof(RGBTRIPLE) + origPadding), SEEK_CUR);

        for (int r = 0; r < numRow; r++)
        {
            // put the pointer back to the beginning of this same row - in the original file
            if (r > 0)
            {
                //fprintf(stdout, "orig row [%i]; putting pointer back\n", i);
                fseek(inptr, -((origbiWidth * sizeof(RGBTRIPLE)) + origPadding), SEEK_CUR);
            }

            // iterate over pixels in scanline
            for (int j = 0; j < origbiWidth; j++)
            {
                // decide whether to print the pixel or not, and if so, how many times
                numPix = 0;
                if (factor < 1)
                {
                    if (fmod((j+1), (1/factor)) < 1)
                        numPix = 1;
                    else
                        numPix = 0;
                }
                else
                {
                    if ((int)factor != factor && fmod((j+1), (1/factor)) < 1 - (1/factor))
                        numPix = factor + 1;
                    else
                        numPix = factor;
                }

                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                //fprintf(stdout, "reading [%i][%i]: B=%i, G=%i, R=%i\n", i, j, triple.rgbtBlue, triple.rgbtGreen, triple.rgbtRed);
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                for (int k = 0; k < numPix; k++)
                {
                    // write RGB triple to outfile
                    //1fprintf(stdout, "writing [%i][%i]: B=%i, G=%i, R=%i\n", i, j, triple.rgbtBlue, triple.rgbtGreen, triple.rgbtRed);
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            // skip over padding, if any
            fseek(inptr, origPadding, SEEK_CUR);

            // then add new padding if needed
            for (int k = 0; k < newPadding; k++)
            {
                fputc(0x00, outptr);
            }
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
