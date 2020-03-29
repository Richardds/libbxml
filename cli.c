/**
 * Tiny BinaryXML parser library usage example
 * 
 * Richard Boldiš <richard@boldis.dev>
 * https://github.com/Richardds
 * 
 */

#include <stdio.h>
#include "src/bxml.h"

int main(int argc, char * argv[])
{
    struct BinaryXML binaryXML;

    if (argc < 2) {
        printf("bxml <filename>\n");
        return 1;
    }

    FILE * fileHandle = fopen(argv[1], "rb");

    if (fileHandle == NULL) {
        printf("Input file does not exist\n");
        return 1;
    }

    fseek(fileHandle, 0, SEEK_END);
    size_t fileSize = ftell(fileHandle);
    fseek(fileHandle, 0, SEEK_SET);

    enum BinaryXMLStatus parseStatus = readBinaryXML(fileHandle, fileSize, &binaryXML);

    if (parseStatus == NO_ERROR) {
        printBinaryXMLNode(&binaryXML, binaryXML.nodes, 0);
    } else {
        switch (parseStatus) {
            case INVALID_FILE_HEADER_MAGIC:
                fprintf(stderr, "File format magic mismatch\n");
                break;
            case MEMORY_ALLOCATION_FAILED:
                fprintf(stderr, "Failed to allocate memory\n");
                break;
            case FILE_CORRUPTED:
                fprintf(stderr, "Input file is corrupted\n");
                break;
            default:
                fprintf(stderr, "Unknown error\n");
                break;
        }

        fclose(fileHandle);
        freeBinaryXML(&binaryXML);

        return 1;
    }

    fclose(fileHandle);
    freeBinaryXML(&binaryXML);

    return 0;
}
