/**
 * Tiny BinaryXML parser library
 * 
 * Richard Boldiš <richard.boldis@protonmail.com>
 * https://github.com/Richardds
 * 
 */

#include "bxml.h"

enum BinaryXMLStatus readBinaryXML(FILE * fileHandle, size_t fileSize, struct BinaryXML * binaryXML)
{
    // Read file header
    fread(&binaryXML->header, sizeof(struct BinaryXMLFileHeader), 1, fileHandle);

    if (binaryXML->header.magic != FOURCC('B', 'X', 'M', 'L')) {
        return INVALID_FILE_HEADER_MAGIC;
    }

    size_t attributesSize = sizeof(struct BinaryXMLFileAttribute) * binaryXML->header.attributesCount;
    size_t nodesSize = sizeof(struct BinaryXMLFileNode) * binaryXML->header.nodesCount;
    size_t minimalFileSize = sizeof(struct BinaryXMLFileHeader) + attributesSize + nodesSize + binaryXML->header.stringsCount;

    if (fileSize < minimalFileSize) {
        return FILE_CORRUPTED;
    }

    // Read node attributes
    binaryXML->attributes = (struct BinaryXMLFileAttribute *)malloc(sizeof(struct BinaryXMLFileAttribute) * binaryXML->header.attributesCount);
    if (binaryXML->attributes == NULL) {
        return MEMORY_ALLOCATION_FAILED;
    }
    fread(binaryXML->attributes, sizeof(struct BinaryXMLFileAttribute), binaryXML->header.attributesCount, fileHandle);

    // Read nodes
    binaryXML->nodes = (struct BinaryXMLFileNode *)malloc(sizeof(struct BinaryXMLFileNode) * binaryXML->header.nodesCount);
    if (binaryXML->nodes == NULL) {
        return MEMORY_ALLOCATION_FAILED;
    }
    fread(binaryXML->nodes, sizeof(struct BinaryXMLFileNode), binaryXML->header.nodesCount, fileHandle);

    // Read strings buffer
    size_t stringsBufferSize = fileSize - ftell(fileHandle);
    binaryXML->stringsBuffer = (char *)malloc(stringsBufferSize);
    if (binaryXML->stringsBuffer == NULL) {
        return MEMORY_ALLOCATION_FAILED;
    }
    fread(binaryXML->stringsBuffer, stringsBufferSize, 1, fileHandle);

    // Map strings to table
    binaryXML->stringsTable = (char **)malloc(sizeof(char *) * binaryXML->header.stringsCount);
    if (binaryXML->stringsTable == NULL) {
        return MEMORY_ALLOCATION_FAILED;
    }
    char * stringPtr = binaryXML->stringsBuffer;
    for (int index = 0; index < binaryXML->header.stringsCount; index++) {
        binaryXML->stringsTable[index] = stringPtr;
        stringPtr += strlen((const char *)stringPtr) + 1;
    }

    return NO_ERROR;
}

void freeBinaryXML(struct BinaryXML * binaryXML)
{
    free(binaryXML->attributes);
    binaryXML->attributes = NULL;

    free(binaryXML->nodes);
    binaryXML->nodes = NULL;

    free(binaryXML->stringsBuffer);
    binaryXML->stringsBuffer = NULL;

    free(binaryXML->stringsTable);
    binaryXML->stringsTable = NULL;
}

void printBinaryXMLNode(struct BinaryXML * binaryXML, struct BinaryXMLFileNode * node, unsigned short depth)
{
    char * name = binaryXML->stringsTable[node->nameIndex];

    printBinaryXMLDepth(depth);
    
    printf("<%s", name);

    if (node->attributesCount > 0) {
        for (int index = node->attributeIndex; index < node->attributeIndex + node->attributesCount; index++) {
            struct BinaryXMLFileAttribute * attribute = &binaryXML->attributes[index];
            printf(" %s=\"%s\"", binaryXML->stringsTable[attribute->nameIndex], binaryXML->stringsTable[attribute->valueIndex]);
        }
    }

    putchar('>');

    if (node->firstChildIndex != InvalidIndex) {
        putchar('\n');
        printBinaryXMLNode(binaryXML, &binaryXML->nodes[node->firstChildIndex], depth + 1);
        printBinaryXMLDepth(depth);
    }

    printf("</%s>\n", name);

    if (node->nextSiblingIndex != InvalidIndex) {
        printBinaryXMLNode(binaryXML, &binaryXML->nodes[node->nextSiblingIndex], depth);
    }
}

void printBinaryXMLDepth(unsigned short depth)
{
    if (depth > 0) {
        putchar('\t');
        printBinaryXMLDepth(depth - 1);
    }
}
