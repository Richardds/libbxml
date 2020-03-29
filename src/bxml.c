/**
 * Tiny BinaryXML parser library
 * 
 * Richard Boldiš <richard@boldis.dev>
 * https://github.com/Richardds
 * 
 */

#include <assert.h>
#include "bxml.h"

enum BinaryXMLStatus readBinaryXML(FILE * fileHandle, size_t fileSize, struct BinaryXML * binaryXML)
{
    // Read file header
    size_t headerRead = fread(&binaryXML->header, sizeof(struct BinaryXMLFileHeader), 1, fileHandle);
    assert(headerRead == 1);

    if (binaryXML->header.magic != BinaryXMLHeaderMagic) {
        return INVALID_FILE_HEADER_MAGIC;
    }

    // Validate expected file size
    size_t minimalFileSize = sizeof(struct BinaryXMLFileHeader)
                           + sizeof(struct BinaryXMLFileAttribute) * binaryXML->header.attributesCount
                           + sizeof(struct BinaryXMLFileNode) * binaryXML->header.nodesCount
                           + binaryXML->header.stringsCount;

    if (fileSize < minimalFileSize) {
        return FILE_CORRUPTED;
    }

    // Read node attributes
    binaryXML->attributes = (struct BinaryXMLFileAttribute *)malloc(sizeof(struct BinaryXMLFileAttribute) * binaryXML->header.attributesCount);
    if (binaryXML->attributes == NULL) {
        return MEMORY_ALLOCATION_FAILED;
    }
    int32_t attributesRead = fread(binaryXML->attributes, sizeof(struct BinaryXMLFileAttribute), binaryXML->header.attributesCount, fileHandle);
    assert(attributesRead == binaryXML->header.attributesCount);

    // Read nodes
    binaryXML->nodes = (struct BinaryXMLFileNode *)malloc(sizeof(struct BinaryXMLFileNode) * binaryXML->header.nodesCount);
    if (binaryXML->nodes == NULL) {
        return MEMORY_ALLOCATION_FAILED;
    }
    int32_t nodesRead = fread(binaryXML->nodes, sizeof(struct BinaryXMLFileNode), binaryXML->header.nodesCount, fileHandle);
    assert(nodesRead == binaryXML->header.nodesCount);

    // Read strings buffer
    size_t stringsBufferSize = fileSize - ftell(fileHandle);
    binaryXML->stringsBuffer = (char *)malloc(stringsBufferSize);
    if (binaryXML->stringsBuffer == NULL) {
        return MEMORY_ALLOCATION_FAILED;
    }
    size_t stringsBufferRead = fread(binaryXML->stringsBuffer, stringsBufferSize, 1, fileHandle);
    assert(stringsBufferRead == 1);

    // Map strings to table
    binaryXML->stringsTable = (char **)malloc(sizeof(char *) * binaryXML->header.stringsCount);
    if (binaryXML->stringsTable == NULL) {
        return MEMORY_ALLOCATION_FAILED;
    }
    char * stringPtr = (char *)binaryXML->stringsBuffer;
    for (int32_t index = 0; index < binaryXML->header.stringsCount; index++) {
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

void printBinaryXMLNode(struct BinaryXML * binaryXML, struct BinaryXMLFileNode * node, int_fast8_t depth)
{
    char * name = binaryXML->stringsTable[node->nameIndex];

    printBinaryXMLDepth(depth);
    
    printf("<%s", name);

    if (node->attributesCount > 0) {
        for (int32_t index = node->attributeIndex; index < node->attributeIndex + node->attributesCount; index++) {
            struct BinaryXMLFileAttribute * attribute = &binaryXML->attributes[index];
            printf(" %s=\"%s\"", binaryXML->stringsTable[attribute->nameIndex], binaryXML->stringsTable[attribute->valueIndex]);
        }
    }

    putchar('>');

    if (node->firstChildIndex != BinaryXMLInvalidIndex) {
        putchar('\n');
        printBinaryXMLNode(binaryXML, &binaryXML->nodes[node->firstChildIndex], depth + 1);
        printBinaryXMLDepth(depth);
    }

    printf("</%s>\n", name);

    if (node->nextSiblingIndex != BinaryXMLInvalidIndex) {
        printBinaryXMLNode(binaryXML, &binaryXML->nodes[node->nextSiblingIndex], depth);
    }
}

void printBinaryXMLDepth(int_fast8_t depth)
{
    if (depth > 0) {
        putchar('\t');
        printBinaryXMLDepth(depth - 1);
    }
}
