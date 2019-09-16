/**
 * Tiny BinaryXML parser library
 * 
 * Richard Boldiš <richard.boldis@protonmail.com>
 * https://github.com/Richardds
 * 
 */

#ifndef __BXML_H
#define __BXML_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define FOURCC(a,b,c,d) ((unsigned int)(((a)<<24) | ((b)<<16) | ((c)<<8) | (d)))

enum BinaryXMLStatus {
    NO_ERROR,
    INVALID_FILE_HEADER_MAGIC,
    MEMORY_ALLOCATION_FAILED,
    FILE_CORRUPTED
};

struct BinaryXMLFileHeader
{
    unsigned int magic;
    int attributesCount;
    int nodesCount;
    int stringsCount;
} __attribute__ ((packed));

struct BinaryXMLFileAttribute
{
    int nameIndex;
    int valueIndex;
} __attribute__ ((packed));

struct BinaryXMLFileNode
{
    int nameIndex;
    int firstChildIndex;
    int nextSiblingIndex;
    int parentIndex;
    int attributeIndex;
    int attributesCount;
} __attribute__ ((packed));

struct BinaryXML
{
    struct BinaryXMLFileHeader header;
    struct BinaryXMLFileAttribute * attributes;
    struct BinaryXMLFileNode * nodes;
    char * stringsBuffer;
    char ** stringsTable;
};

const int InvalidIndex = INT_MAX;

enum BinaryXMLStatus readBinaryXML(FILE * fileHandle, size_t fileSize, struct BinaryXML * binaryXML);

void freeBinaryXML(struct BinaryXML * binaryXML);

void printBinaryXMLNode(struct BinaryXML * binaryXML, struct BinaryXMLFileNode * node, unsigned short depth);

void printBinaryXMLDepth(unsigned short depth);

#endif
