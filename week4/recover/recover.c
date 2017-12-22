#include <stdio.h>
#include <stdint.h>

typedef uint8_t  BYTE;

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[1];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // the first 3 bytes of JPEGs are:
    //      0xff 0xd8 0xff
    // and the 4th byte's first four bits are
    //      1110

    int sizeOfBuffer = 512; // buffer size for FAT file system's block size
    char buffer[sizeOfBuffer];
    int readBytes = 0;
    int fileCount = 0;
    int signature = 0;
    char outfile[8] = "000.jpg";
    FILE *outptr = NULL;

    readBytes = fread(&buffer, sizeof(BYTE), sizeOfBuffer, inptr);

    while (readBytes == sizeOfBuffer)
    {
        // check each block for JPEG signature
        if ((BYTE)buffer[0] == 0xff &&
            (BYTE)buffer[1] == 0xd8 &&
            (BYTE)buffer[2] == 0xff &&
            ((BYTE)buffer[3] & 0xf0) == 0xe0
            )
        {
            signature = 1;
        }

        if (signature == 1)
        {
            // close previous output file - if this is not the first one
            if (fileCount > 0)
                fclose(outptr);

            // construct file name
            snprintf(outfile, sizeof(outfile), "%03d.jpg", fileCount);

            outptr = fopen(outfile, "w");
            if (outptr == NULL)
            {
                fclose(inptr);
                fprintf(stderr, "Could not create %s.\n", outfile);
                return 3;
            }

            fileCount++;
        }

        // whether it is a new file or not, we're appending the block
        // but not if the first few blocks are not JPEGs!
        if (fileCount > 0)
            fwrite(&buffer, sizeof(BYTE), sizeOfBuffer, outptr);

        readBytes = fread(&buffer, sizeof(BYTE), sizeOfBuffer, inptr);
        signature = 0;
    }

    // Let's check if there are more bytes at this end
    // and make sure to append
    if (readBytes > 0)
        fwrite(&buffer, sizeof(BYTE), readBytes, outptr);

    // close infile
    fclose(inptr);
    // close outfile
    fclose(outptr);

    return 0;
}
