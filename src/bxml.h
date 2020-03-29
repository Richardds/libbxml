/**
 * Tiny BinaryXML parser library
 * 
 * Richard Boldiš <richard@boldis.dev>
 * https://github.com/Richardds
 * 
 */

#ifndef __BXML_H
#define __BXML_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#define FOURCC(a,b,c,d) ((uint32_t)(((a)<<24) | ((b)<<16) | ((c)<<8) | (d)))

#define BinaryXMLHeaderMagic FOURCC('B', 'X', 'M', 'L')
#define BinaryXMLInvalidIndex INT_MAX

enum BinaryXMLStatus {
    NO_ERROR,
    INVALID_FILE_HEADER_MAGIC,
    MEMORY_ALLOCATION_FAILED,
    FILE_CORRUPTED
};

struct BinaryXMLFileHeader
{
    uint32_t magic;
    int32_t attributesCount;
    int32_t nodesCount;
    int32_t stringsCount;
} __attribute__ ((packed));

struct BinaryXMLFileAttribute
{
    int32_t nameIndex;
    int32_t valueIndex;
} __attribute__ ((packed));

struct BinaryXMLFileNode
{
    int32_t nameIndex;
    int32_t firstChildIndex;
    int32_t nextSiblingIndex;
    int32_t parentIndex;
    int32_t attributeIndex;
    int32_t attributesCount;
} __attribute__ ((packed));

struct BinaryXML
{
    struct BinaryXMLFileHeader header;
    struct BinaryXMLFileAttribute * attributes;
    struct BinaryXMLFileNode * nodes;
    char * stringsBuffer;
    char ** stringsTable;
};

enum BinaryXMLStatus readBinaryXML(FILE * fileHandle, size_t fileSize, struct BinaryXML * binaryXML);

void freeBinaryXML(struct BinaryXML * binaryXML);

void printBinaryXMLNode(struct BinaryXML * binaryXML, struct BinaryXMLFileNode * node, int_fast8_t depth);

void printBinaryXMLDepth(int_fast8_t depth);

#endif
