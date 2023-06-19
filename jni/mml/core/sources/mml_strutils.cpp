#include "mml_strutils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "mml_memory.h"


typedef struct  
{
	const mmlUInt8 * lower;
	const mmlUInt8 * upper;
}MML_COLLATE_T;


static const mmlUInt8 mml_latin_lower_table[256] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};


static const mmlUInt8 mml_latin_upper_table[256] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};


static const mmlUInt8 mml_cyrillic_lower_table[256] =
{
	0x00, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};


static const mmlUInt8 mml_cyrillic_upper_table[256] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x50, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

static const MML_COLLATE_T mml_collate_table [256] = 
{
	{ mml_latin_lower_table       ,   mml_latin_upper_table     }, //00
	{ mmlNULL                     ,   mmlNULL                   }, //01
	{ mmlNULL                     ,   mmlNULL                   }, //02
	{ mmlNULL                     ,   mmlNULL                   }, //03
	{ mml_cyrillic_lower_table    ,   mml_cyrillic_upper_table  }, //04
	{ mmlNULL                     ,   mmlNULL                   }, //05
	{ mmlNULL                     ,   mmlNULL                   }, //06
	{ mmlNULL                     ,   mmlNULL                   }, //07
	{ mmlNULL                     ,   mmlNULL                   }, //08
	{ mmlNULL                     ,   mmlNULL                   }, //09
	{ mmlNULL                     ,   mmlNULL                   }, //0A
	{ mmlNULL                     ,   mmlNULL                   }, //0B
	{ mmlNULL                     ,   mmlNULL                   }, //0C
	{ mmlNULL                     ,   mmlNULL                   }, //0D
	{ mmlNULL                     ,   mmlNULL                   }, //0E
	{ mmlNULL                     ,   mmlNULL                   }, //0F

	{ mmlNULL                     ,   mmlNULL                   }, //10
	{ mmlNULL                     ,   mmlNULL                   }, //11
	{ mmlNULL                     ,   mmlNULL                   }, //12
	{ mmlNULL                     ,   mmlNULL                   }, //13
	{ mmlNULL                     ,   mmlNULL                   }, //14
	{ mmlNULL                     ,   mmlNULL                   }, //15
	{ mmlNULL                     ,   mmlNULL                   }, //16
	{ mmlNULL                     ,   mmlNULL                   }, //17
	{ mmlNULL                     ,   mmlNULL                   }, //18
	{ mmlNULL                     ,   mmlNULL                   }, //19
	{ mmlNULL                     ,   mmlNULL                   }, //1A
	{ mmlNULL                     ,   mmlNULL                   }, //1B
	{ mmlNULL                     ,   mmlNULL                   }, //1C
	{ mmlNULL                     ,   mmlNULL                   }, //1D
	{ mmlNULL                     ,   mmlNULL                   }, //1E
	{ mmlNULL                     ,   mmlNULL                   }, //1F

	{ mmlNULL                     ,   mmlNULL                   }, //20
	{ mmlNULL                     ,   mmlNULL                   }, //21
	{ mmlNULL                     ,   mmlNULL                   }, //22
	{ mmlNULL                     ,   mmlNULL                   }, //23
	{ mmlNULL                     ,   mmlNULL                   }, //24
	{ mmlNULL                     ,   mmlNULL                   }, //25
	{ mmlNULL                     ,   mmlNULL                   }, //26
	{ mmlNULL                     ,   mmlNULL                   }, //27
	{ mmlNULL                     ,   mmlNULL                   }, //28
	{ mmlNULL                     ,   mmlNULL                   }, //29
	{ mmlNULL                     ,   mmlNULL                   }, //2A
	{ mmlNULL                     ,   mmlNULL                   }, //2B
	{ mmlNULL                     ,   mmlNULL                   }, //2C
	{ mmlNULL                     ,   mmlNULL                   }, //2D
	{ mmlNULL                     ,   mmlNULL                   }, //2E
	{ mmlNULL                     ,   mmlNULL                   }, //2F

	{ mmlNULL                     ,   mmlNULL               }, //30
	{ mmlNULL                     ,   mmlNULL               }, //31
	{ mmlNULL                     ,   mmlNULL               }, //32
	{ mmlNULL                     ,   mmlNULL               }, //33
	{ mmlNULL                     ,   mmlNULL               }, //34
	{ mmlNULL                     ,   mmlNULL               }, //35
	{ mmlNULL                     ,   mmlNULL               }, //36
	{ mmlNULL                     ,   mmlNULL               }, //37
	{ mmlNULL                     ,   mmlNULL               }, //38
	{ mmlNULL                     ,   mmlNULL               }, //39
	{ mmlNULL                     ,   mmlNULL               }, //3A
	{ mmlNULL                     ,   mmlNULL               }, //3B
	{ mmlNULL                     ,   mmlNULL               }, //3C
	{ mmlNULL                     ,   mmlNULL               }, //3D
	{ mmlNULL                     ,   mmlNULL               }, //3E
	{ mmlNULL                     ,   mmlNULL               }, //3F

	{ mmlNULL                     ,   mmlNULL               }, //40
	{ mmlNULL                     ,   mmlNULL               }, //41
	{ mmlNULL                     ,   mmlNULL               }, //42
	{ mmlNULL                     ,   mmlNULL               }, //43
	{ mmlNULL                     ,   mmlNULL               }, //44
	{ mmlNULL                     ,   mmlNULL               }, //45
	{ mmlNULL                     ,   mmlNULL               }, //46
	{ mmlNULL                     ,   mmlNULL               }, //47
	{ mmlNULL                     ,   mmlNULL               }, //48
	{ mmlNULL                     ,   mmlNULL               }, //49
	{ mmlNULL                     ,   mmlNULL               }, //4A
	{ mmlNULL                     ,   mmlNULL               }, //4B
	{ mmlNULL                     ,   mmlNULL               }, //4C
	{ mmlNULL                     ,   mmlNULL               }, //4D
	{ mmlNULL                     ,   mmlNULL               }, //4E
	{ mmlNULL                     ,   mmlNULL               }, //4F

	{ mmlNULL                     ,   mmlNULL               }, //50
	{ mmlNULL                     ,   mmlNULL               }, //51
	{ mmlNULL                     ,   mmlNULL               }, //52
	{ mmlNULL                     ,   mmlNULL               }, //53
	{ mmlNULL                     ,   mmlNULL               }, //54
	{ mmlNULL                     ,   mmlNULL               }, //55
	{ mmlNULL                     ,   mmlNULL               }, //56
	{ mmlNULL                     ,   mmlNULL               }, //57
	{ mmlNULL                     ,   mmlNULL               }, //58
	{ mmlNULL                     ,   mmlNULL               }, //59
	{ mmlNULL                     ,   mmlNULL               }, //5A
	{ mmlNULL                     ,   mmlNULL               }, //5B
	{ mmlNULL                     ,   mmlNULL               }, //5C
	{ mmlNULL                     ,   mmlNULL               }, //5D
	{ mmlNULL                     ,   mmlNULL               }, //5E
	{ mmlNULL                     ,   mmlNULL               }, //5F

	{ mmlNULL                     ,   mmlNULL               }, //60
	{ mmlNULL                     ,   mmlNULL               }, //61
	{ mmlNULL                     ,   mmlNULL               }, //62
	{ mmlNULL                     ,   mmlNULL               }, //63
	{ mmlNULL                     ,   mmlNULL               }, //64
	{ mmlNULL                     ,   mmlNULL               }, //65
	{ mmlNULL                     ,   mmlNULL               }, //66
	{ mmlNULL                     ,   mmlNULL               }, //67
	{ mmlNULL                     ,   mmlNULL               }, //68
	{ mmlNULL                     ,   mmlNULL               }, //69
	{ mmlNULL                     ,   mmlNULL               }, //6A
	{ mmlNULL                     ,   mmlNULL               }, //6B
	{ mmlNULL                     ,   mmlNULL               }, //6C
	{ mmlNULL                     ,   mmlNULL               }, //6D
	{ mmlNULL                     ,   mmlNULL               }, //6E
	{ mmlNULL                     ,   mmlNULL               }, //6F

	{ mmlNULL                     ,   mmlNULL               }, //70
	{ mmlNULL                     ,   mmlNULL               }, //71
	{ mmlNULL                     ,   mmlNULL               }, //72
	{ mmlNULL                     ,   mmlNULL               }, //73
	{ mmlNULL                     ,   mmlNULL               }, //74
	{ mmlNULL                     ,   mmlNULL               }, //75
	{ mmlNULL                     ,   mmlNULL               }, //76
	{ mmlNULL                     ,   mmlNULL               }, //77
	{ mmlNULL                     ,   mmlNULL               }, //78
	{ mmlNULL                     ,   mmlNULL               }, //79
	{ mmlNULL                     ,   mmlNULL               }, //7A
	{ mmlNULL                     ,   mmlNULL               }, //7B
	{ mmlNULL                     ,   mmlNULL               }, //7C
	{ mmlNULL                     ,   mmlNULL               }, //7D
	{ mmlNULL                     ,   mmlNULL               }, //7E
	{ mmlNULL                     ,   mmlNULL               }, //7F


	{ mmlNULL                     ,   mmlNULL               }, //80
	{ mmlNULL                     ,   mmlNULL               }, //81
	{ mmlNULL                     ,   mmlNULL               }, //82
	{ mmlNULL                     ,   mmlNULL               }, //83
	{ mmlNULL                     ,   mmlNULL               }, //84
	{ mmlNULL                     ,   mmlNULL               }, //85
	{ mmlNULL                     ,   mmlNULL               }, //86
	{ mmlNULL                     ,   mmlNULL               }, //87
	{ mmlNULL                     ,   mmlNULL               }, //88
	{ mmlNULL                     ,   mmlNULL               }, //89
	{ mmlNULL                     ,   mmlNULL               }, //8A
	{ mmlNULL                     ,   mmlNULL               }, //8B
	{ mmlNULL                     ,   mmlNULL               }, //8C
	{ mmlNULL                     ,   mmlNULL               }, //8D
	{ mmlNULL                     ,   mmlNULL               }, //8E
	{ mmlNULL                     ,   mmlNULL               }, //8F

	{ mmlNULL                     ,   mmlNULL               }, //90
	{ mmlNULL                     ,   mmlNULL               }, //91
	{ mmlNULL                     ,   mmlNULL               }, //92
	{ mmlNULL                     ,   mmlNULL               }, //93
	{ mmlNULL                     ,   mmlNULL               }, //94
	{ mmlNULL                     ,   mmlNULL               }, //95
	{ mmlNULL                     ,   mmlNULL               }, //96
	{ mmlNULL                     ,   mmlNULL               }, //97
	{ mmlNULL                     ,   mmlNULL               }, //98
	{ mmlNULL                     ,   mmlNULL               }, //99
	{ mmlNULL                     ,   mmlNULL               }, //9A
	{ mmlNULL                     ,   mmlNULL               }, //9B
	{ mmlNULL                     ,   mmlNULL               }, //9C
	{ mmlNULL                     ,   mmlNULL               }, //9D
	{ mmlNULL                     ,   mmlNULL               }, //9E
	{ mmlNULL                     ,   mmlNULL               }, //9F

	{ mmlNULL                     ,   mmlNULL               }, //A0
	{ mmlNULL                     ,   mmlNULL               }, //A1
	{ mmlNULL                     ,   mmlNULL               }, //A2
	{ mmlNULL                     ,   mmlNULL               }, //A3
	{ mmlNULL                     ,   mmlNULL               }, //A4
	{ mmlNULL                     ,   mmlNULL               }, //A5
	{ mmlNULL                     ,   mmlNULL               }, //A6
	{ mmlNULL                     ,   mmlNULL               }, //A7
	{ mmlNULL                     ,   mmlNULL               }, //A8
	{ mmlNULL                     ,   mmlNULL               }, //A9
	{ mmlNULL                     ,   mmlNULL               }, //AA
	{ mmlNULL                     ,   mmlNULL               }, //AB
	{ mmlNULL                     ,   mmlNULL               }, //AC
	{ mmlNULL                     ,   mmlNULL               }, //AD
	{ mmlNULL                     ,   mmlNULL               }, //AE
	{ mmlNULL                     ,   mmlNULL               }, //AF


	{ mmlNULL                     ,   mmlNULL               }, //B0
	{ mmlNULL                     ,   mmlNULL               }, //B1
	{ mmlNULL                     ,   mmlNULL               }, //B2
	{ mmlNULL                     ,   mmlNULL               }, //B3
	{ mmlNULL                     ,   mmlNULL               }, //B4
	{ mmlNULL                     ,   mmlNULL               }, //B5
	{ mmlNULL                     ,   mmlNULL               }, //B6
	{ mmlNULL                     ,   mmlNULL               }, //B7
	{ mmlNULL                     ,   mmlNULL               }, //B8
	{ mmlNULL                     ,   mmlNULL               }, //B9
	{ mmlNULL                     ,   mmlNULL               }, //BA
	{ mmlNULL                     ,   mmlNULL               }, //BB
	{ mmlNULL                     ,   mmlNULL               }, //BC
	{ mmlNULL                     ,   mmlNULL               }, //BD
	{ mmlNULL                     ,   mmlNULL               }, //BE
	{ mmlNULL                     ,   mmlNULL               }, //BF

	{ mmlNULL                     ,   mmlNULL               }, //C0
	{ mmlNULL                     ,   mmlNULL               }, //C1
	{ mmlNULL                     ,   mmlNULL               }, //C2
	{ mmlNULL                     ,   mmlNULL               }, //C3
	{ mmlNULL                     ,   mmlNULL               }, //C4
	{ mmlNULL                     ,   mmlNULL               }, //C5
	{ mmlNULL                     ,   mmlNULL               }, //C6
	{ mmlNULL                     ,   mmlNULL               }, //C7
	{ mmlNULL                     ,   mmlNULL               }, //C8
	{ mmlNULL                     ,   mmlNULL               }, //C9
	{ mmlNULL                     ,   mmlNULL               }, //CA
	{ mmlNULL                     ,   mmlNULL               }, //CB
	{ mmlNULL                     ,   mmlNULL               }, //CC
	{ mmlNULL                     ,   mmlNULL               }, //CD
	{ mmlNULL                     ,   mmlNULL               }, //CE
	{ mmlNULL                     ,   mmlNULL               }, //CF

	{ mmlNULL                     ,   mmlNULL               }, //D0
	{ mmlNULL                     ,   mmlNULL               }, //D1
	{ mmlNULL                     ,   mmlNULL               }, //D2
	{ mmlNULL                     ,   mmlNULL               }, //D3
	{ mmlNULL                     ,   mmlNULL               }, //D4
	{ mmlNULL                     ,   mmlNULL               }, //D5
	{ mmlNULL                     ,   mmlNULL               }, //D6
	{ mmlNULL                     ,   mmlNULL               }, //D7
	{ mmlNULL                     ,   mmlNULL               }, //D8
	{ mmlNULL                     ,   mmlNULL               }, //D9
	{ mmlNULL                     ,   mmlNULL               }, //DA
	{ mmlNULL                     ,   mmlNULL               }, //DB
	{ mmlNULL                     ,   mmlNULL               }, //DC
	{ mmlNULL                     ,   mmlNULL               }, //DD
	{ mmlNULL                     ,   mmlNULL               }, //DE
	{ mmlNULL                     ,   mmlNULL               }, //DF

	{ mmlNULL                     ,   mmlNULL               }, //E0
	{ mmlNULL                     ,   mmlNULL               }, //E1
	{ mmlNULL                     ,   mmlNULL               }, //E2
	{ mmlNULL                     ,   mmlNULL               }, //E3
	{ mmlNULL                     ,   mmlNULL               }, //E4
	{ mmlNULL                     ,   mmlNULL               }, //E5
	{ mmlNULL                     ,   mmlNULL               }, //E6
	{ mmlNULL                     ,   mmlNULL               }, //E7
	{ mmlNULL                     ,   mmlNULL               }, //E8
	{ mmlNULL                     ,   mmlNULL               }, //E9
	{ mmlNULL                     ,   mmlNULL               }, //EA
	{ mmlNULL                     ,   mmlNULL               }, //EB
	{ mmlNULL                     ,   mmlNULL               }, //EC
	{ mmlNULL                     ,   mmlNULL               }, //ED
	{ mmlNULL                     ,   mmlNULL               }, //EE
	{ mmlNULL                     ,   mmlNULL               }, //EF

	{ mmlNULL                     ,   mmlNULL               }, //F0
	{ mmlNULL                     ,   mmlNULL               }, //F1
	{ mmlNULL                     ,   mmlNULL               }, //F2
	{ mmlNULL                     ,   mmlNULL               }, //F3
	{ mmlNULL                     ,   mmlNULL               }, //F4
	{ mmlNULL                     ,   mmlNULL               }, //F5
	{ mmlNULL                     ,   mmlNULL               }, //F6
	{ mmlNULL                     ,   mmlNULL               }, //F7
	{ mmlNULL                     ,   mmlNULL               }, //F8
	{ mmlNULL                     ,   mmlNULL               }, //F9
	{ mmlNULL                     ,   mmlNULL               }, //FA
	{ mmlNULL                     ,   mmlNULL               }, //FB
	{ mmlNULL                     ,   mmlNULL               }, //FC
	{ mmlNULL                     ,   mmlNULL               }, //FD
	{ mmlNULL                     ,   mmlNULL               }, //FE
	{ mmlNULL                     ,   mmlNULL               }  //FF
};

mml_core mmlUChar mmlToLower ( const mmlUChar chr )
{
	if ( mml_collate_table[(chr >> 8) & 0x00FF].lower == mmlNULL )
	{
		return chr;
	}
	return mml_collate_table[(chr >> 8) & 0x00FF].lower[(chr) & 0x00FF];
}

mml_core mmlUChar mmlToUpper ( const mmlUChar chr )
{
	if ( mml_collate_table[(chr >> 8) & 0x00FF].upper == mmlNULL )
	{
		return chr;
	}
	return mml_collate_table[(chr >> 8) & 0x00FF].upper[(chr) & 0x00FF];
}

mml_core mmlInt32 mmlSprintf ( mmlChar * buffer , mmlInt32 len, const mmlChar * formatter , ... )
{
	mmlInt32 result = 0;
	va_list lst;
	va_start(lst, formatter);
	result = vsnprintf(buffer , len , formatter, lst);
	va_end(lst);
	return result;
}

mml_core mmlInt32 mmlVSprintf ( mmlChar * buffer , mmlInt32 len , const mmlChar * formatter , va_list lst )
{
	mmlInt32 result = 0;
	result = vsnprintf(buffer , len , formatter, lst);
	return result;
}


mml_core mmlInt32 mml_utf8_mbtowc (mmlUChar *pwc, const mmlUInt8 *s, const mmlInt32 n)
{
	mmlUInt8 c = s[0];

	if (c < 0x80) 
	{
		if ( pwc != mmlNULL ) *pwc = c;
		return 1;
	} 
	else if (c < 0xc2) 
	{
		return 0;
	} 
	else if (c < 0xe0) 
	{
		if (pwc != mmlNULL && n < 2)
		{
			return 0;
		}
		if (!((s[1] ^ 0x80) < 0x40))
		{
			return 0;
		}
		if ( pwc != mmlNULL ) *pwc = ((mmlUChar) (c & 0x1f) << 6) | (mmlUChar) (s[1] ^ 0x80);
		return 2;
	} 
	else if (c < 0xf0) 
	{
		if (pwc != mmlNULL && n < 3)
		{
			return 0;
		}
		if (!((s[1] ^ 0x80) < 0x40 && (s[2] ^ 0x80) < 0x40
			&& (c >= 0xe1 || s[1] >= 0xa0)))
		{
			return 0;
		}
		if ( pwc != mmlNULL ) 
		*pwc = ((mmlUChar) (c & 0x0f) << 12)
			| ((mmlUChar) (s[1] ^ 0x80) << 6)
			|  (mmlUChar) (s[2] ^ 0x80);
		return 3;
	} 
	else if (c < 0xf8 && sizeof(mmlUChar)*8 >= 32) 
	{
		if (pwc != mmlNULL && n < 4)
		{
			return 0;
		}
		if (!((s[1] ^ 0x80) < 0x40 && (s[2] ^ 0x80) < 0x40
			&& (s[3] ^ 0x80) < 0x40
			&& (c >= 0xf1 || s[1] >= 0x90)))
		{
			return 0;
		}
		if ( pwc != mmlNULL ) 
		*pwc = ((mmlUChar) (c & 0x07) << 18)
			| ((mmlUChar) (s[1] ^ 0x80) << 12)
			| ((mmlUChar) (s[2] ^ 0x80) << 6)
			| (mmlUChar) (s[3] ^ 0x80);
		return 4;
	} 
	else if (c < 0xfc && sizeof(mmlUChar)*8 >= 32) 
	{
		if (pwc != mmlNULL &&n < 5)
		{
			return 0;
		}
		if (!((s[1] ^ 0x80) < 0x40 && (s[2] ^ 0x80) < 0x40
			&& (s[3] ^ 0x80) < 0x40 && (s[4] ^ 0x80) < 0x40
			&& (c >= 0xf9 || s[1] >= 0x88)))
		{
			return 0;
		}
		if ( pwc != mmlNULL ) 
		*pwc = ((mmlUChar) (c & 0x03) << 24)
			| ((mmlUChar) (s[1] ^ 0x80) << 18)
			| ((mmlUChar) (s[2] ^ 0x80) << 12)
			| ((mmlUChar) (s[3] ^ 0x80) << 6)
			| (mmlUChar) (s[4] ^ 0x80);
		return 5;
	} 
	else if (c < 0xfe && sizeof(mmlUInt16)*8 >= 32) 
	{
		if (pwc != mmlNULL &&n < 6)
		{
			return 0;
		}
		if (!((s[1] ^ 0x80) < 0x40 && (s[2] ^ 0x80) < 0x40
			&& (s[3] ^ 0x80) < 0x40 && (s[4] ^ 0x80) < 0x40
			&& (s[5] ^ 0x80) < 0x40
			&& (c >= 0xfd || s[1] >= 0x84)))
		{
			return 0;
		}
		if ( pwc != mmlNULL ) 
		*pwc = ((mmlUChar) (c & 0x01) << 30)
			| ((mmlUChar) (s[1] ^ 0x80) << 24)
			| ((mmlUChar) (s[2] ^ 0x80) << 18)
			| ((mmlUChar) (s[3] ^ 0x80) << 12)
			| ((mmlUChar) (s[4] ^ 0x80) << 6)
			| (mmlUChar) (s[5] ^ 0x80);
		return 6;
	} 
	return 0;
} 


mml_core mmlInt32 mml_utf8_wctomb (mmlUInt8 *r, mmlUChar wc, mmlInt32 n)
{
    mmlInt32 count;
    if (wc < 0x80)
    {
        count = 1;
    }
    else if (wc < 0x800)
    {
        count = 2;
    }
    else if (wc < 0x10000)
    {
        count = 3;
    }
    else if (wc < 0x200000)
    {
        count = 4;
    }
    else if (wc < 0x4000000)
    {
        count = 5;
    }
    else if (wc <= 0x7fffffff)
    {
        count = 6;
    }
    else
    {
        return -1;
    }
    if (n < count && r != mmlNULL)
    {
        return -1;
    }
	if ( r != mmlNULL )
	{
		switch (count) 
		{
			case 6: r[5] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x4000000;
			case 5: r[4] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x200000;
			case 4: r[3] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x10000;
			case 3: r[2] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x800;
			case 2: r[1] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0xc0;
			case 1: r[0] = wc;
		}
	}
    return count;
} 



mml_core mmlInt32 mmlStrCompare ( const mmlChar * v1 , const mmlChar * v2 , mml_char_conversion_t rule)
{
	const mmlUInt8 * str1_mb = (const mmlUInt8*)v1;
	const mmlUInt8 * str2_mb = (const mmlUInt8*)v2;
	mmlInt32 str1_len = strlen(v1);
	mmlInt32 str2_len = strlen(v2);

	while ( str1_len > 0 && str2_len > 0 )
	{
		mmlUChar c1;
		mmlInt32 offset = mml_utf8_mbtowc(&c1 , str1_mb , str1_len);
		if ( offset == 0 )
		{
			return -1;
		}
		str1_mb += offset; str1_len -= offset;
		c1 = rule != mmlNULL ? rule (c1) : c1;

		mmlUChar c2;
		offset = mml_utf8_mbtowc(&c2 , str2_mb , str2_len);
		if ( offset == 0 )
		{
			return 1;
		}
		str2_mb += offset; str2_len -= offset;
		c2 = rule != mmlNULL ? rule (c2) : c2;
		if ( c1 > c2 )
		{
			return 1;
		}
		else if ( c1 < c2 )
		{
			return -1;
		}
	}

	if ( str1_len > str2_len )
	{
		return 1;
	}
	else if ( str1_len < str2_len )
	{
		return -1;
	}
	return 0;
}


mml_core mmlInt32 mmlStrNCompare ( const mmlChar * v1 , const mmlChar * v2 , const mmlInt32 len  , mml_char_conversion_t rule)
{
	const mmlUInt8 * str1_mb = (const mmlUInt8*)v1;
	const mmlUInt8 * str2_mb = (const mmlUInt8*)v2;
	mmlInt32 str1_len = strlen(v1);
	mmlInt32 str2_len = strlen(v2);
	mmlInt32 str_offset = 0;

	while ( str1_len > 0 && str2_len > 0)
	{
		mmlUChar c1;
		mmlInt32 offset = mml_utf8_mbtowc(&c1 , str1_mb , str1_len);
		if ( offset == 0 )
		{
			return -1;
		}
		str1_mb += offset; str1_len -= offset;
		c1 = rule != mmlNULL ? rule (c1) : c1;

		str_offset += offset;

		mmlUChar c2;
		offset = mml_utf8_mbtowc(&c2 , str2_mb , str2_len);
		if ( offset == 0 )
		{
			return 1;
		}
		str2_mb += offset; str2_len -= offset;
		c2 = rule != mmlNULL ? rule (c2) : c2;
		if ( c1 > c2 )
		{
			return 1;
		}
		else if ( c1 < c2 )
		{
			return -1;
		}

		if ( str_offset >= len )
		{
			return 0;
		}
	}

	if ( str1_len > str2_len )
	{
		return 1;
	}
	else if ( str1_len < str2_len )
	{
		return -1;
	}
	return 0;
}

mml_core void mmlStrCopy ( mmlChar * dst , const mmlChar * src , mmlInt32 len )
{
	strncpy ( dst , src , len );
}

mml_core mmlInt64 mmlStoD ( const mmlChar * buffer , mmlBool * result )
{
#ifdef MML_WIN
	if ( result ) *result = mmlTrue;
	mmlChar * end_ptr;
	return _strtoi64(buffer , &end_ptr , 10);
#else //MML_WIN
        mmlChar * end_ptr;
	if ( result ) *result = mmlTrue;
        return strtoll(buffer, &end_ptr , 10);
#endif //MML_WIN
}

mml_core mmlUInt64 mmlStoUD ( const mmlChar * buffer , mmlBool * result )
{
#ifdef MML_WIN
	if ( result ) *result = mmlTrue;
	mmlChar * end_ptr;
	return _strtoui64(buffer , &end_ptr , 10);
#else //MML_WIN
        if ( result ) *result = mmlTrue;
        mmlChar * end_ptr;
        return strtoull(buffer ,&end_ptr , 10);
#endif //MML_WIN

}

mml_core mmlFloat64 mmlStoF ( const mmlChar * buffer , mmlBool * result )
{
	if ( result ) *result = mmlTrue;
	return (mmlFloat64)atof(buffer);
}

mml_core mmlInt32 mmlStrLength ( const mmlChar * str )
{
	if ( !str ) return 0;
	return (mmlInt32)strlen(str);
}

mml_core mmlChar * mmlStrDup( const mmlChar * val )
{
	mmlChar * new_str = (mmlChar *)mmlAlloc(mmlStrLength(val) + 1);
	mmlStrCopy(new_str, val, mmlStrLength(val));
	return new_str;
}

mml_core mmlInt32 mmlStrUTF8Length ( const mmlChar * str )
{
	mmlInt32 utf8_len = 0;
	mmlInt32 str_len = strlen(str);
	mmlUInt8 * utf8_str = (mmlUInt8*)str;
	while ( str_len > 0)
	{
		mmlInt32 offset = mml_utf8_mbtowc(mmlNULL , utf8_str , str_len);
		if ( offset == 0 )
		{
			return -1;
		}
		str_len -= offset;
		utf8_str += offset;
		utf8_len ++;
	}
	return utf8_len;
}

typedef enum
{
	PATTERN_ANY,
	PATTERN_ALL,
	PATTERN_CHAR
}PATTERN_T;

typedef struct  __pattern
{
	PATTERN_T type;
	mmlUChar ch;
	__pattern * next;
}MATCH_PATTERN_T;

static void append_pattern ( const PATTERN_T type, const mmlUChar ch, MATCH_PATTERN_T ** lst )
{
	MATCH_PATTERN_T * pattern = new MATCH_PATTERN_T();
	pattern->type = type;
	pattern->ch = ch;
	pattern->next = mmlNULL;
	if ( *lst == mmlNULL )
	{
		*lst = pattern;
	}
	else
	{
		MATCH_PATTERN_T * iter;
		for ( iter = *lst; iter ->next != NULL; iter = iter->next ){}
		iter->next = pattern;
	}
}

static void free_patterns (MATCH_PATTERN_T * lst)
{
	if ( lst != mmlNULL )
	{
		MATCH_PATTERN_T * current;
		for ( MATCH_PATTERN_T * iter = lst; iter != NULL; )
		{
			current = iter;
			iter = iter->next;
			delete(current);
		}
		
	}
}

mmlBool match_pattern(MATCH_PATTERN_T * pattern, const mmlChar * str, const mmlInt32 len, mml_char_conversion_t rule)
{
	if ( pattern->type == PATTERN_CHAR )
	{
		mmlUChar ch;
		mmlInt32 offset = mml_utf8_mbtowc(&ch, (const mmlUInt8*) str, len);
		if ( offset == 0)
		{
			return mmlFalse;
		}
		if ( (rule == mmlNULL ? ch : rule(ch)) == (rule == mmlNULL ? pattern->ch : rule(pattern->ch)))
		{
			if ( pattern->next == mmlNULL )
			{
				return mmlTrue;
			}
			return match_pattern(pattern->next, str + offset, len - offset, rule);
		}
	}
	else if ( pattern->type == PATTERN_ANY )
	{
		mmlUChar ch;
		mmlInt32 offset = mml_utf8_mbtowc(&ch, (const mmlUInt8*) str, len);
		if ( offset == 0)
		{
			return mmlFalse;
		}
		if ( pattern->next == mmlNULL )
		{
			return mmlTrue;
		}
		return match_pattern(pattern->next, str + offset, len - offset, rule);
	}
	else if ( pattern->type == PATTERN_ALL )
	{
		mmlInt32 str_offset = 0;
		while (str_offset < len )
		{
			if ( pattern->next == mmlNULL )
			{
				return mmlTrue;
			}
			if ( match_pattern(pattern->next, str + str_offset , len - str_offset, rule) == mmlTrue )
			{
				return mmlTrue;
			}
			mmlUChar ch;
			mmlInt32 offset = mml_utf8_mbtowc(&ch, (const mmlUInt8*) str, len);
			if ( offset == 0)
			{
				return mmlFalse;
			}
			str_offset += offset;
		}
	}
	return mmlFalse;
}

mml_core void * mmlStrMatchPrepare (const mmlChar * pattern)
{
	MATCH_PATTERN_T * lst = mmlNULL;
	mmlInt32 plen = mmlStrLength(pattern);
	while ( *pattern != 0 && plen > 0 )
	{
		mmlUChar ch;
		mmlInt32 offset = mml_utf8_mbtowc(&ch, (const mmlUInt8*) pattern, plen);
		if ( offset == 0 )
		{
			free_patterns((MATCH_PATTERN_T*)lst);
			return mmlNULL;
		}

		if (ch == (mmlUChar)'?' )
		{
			append_pattern(PATTERN_ANY, 0 , &lst);
		}
		else if (ch == (mmlUChar)'*')
		{
			append_pattern(PATTERN_ALL, 0 , &lst);
		}
		else
		{
			append_pattern(PATTERN_CHAR, *pattern, &lst);
		}
		pattern += offset;
		plen -= offset;
	}
	return lst;
}
mml_core void mmlStrMatchFree ( void * pattern )
{
	free_patterns((MATCH_PATTERN_T*)pattern);
}

mml_core mmlBool mmlStrMatchPrepared ( const mmlChar * str , void * pattern, mml_char_conversion_t rule)
{
	return match_pattern((MATCH_PATTERN_T*)pattern, str, mmlStrLength(str), rule);
}

mml_core mmlBool mmlStrMatch ( const mmlChar * str, const mmlChar * pattern, mml_char_conversion_t rule)
{
	void * cpattern = mmlStrMatchPrepare(pattern);
	if ( cpattern == mmlNULL )
	{
		return mmlFalse;
	}
	mmlBool result = mmlStrMatchPrepared(str, cpattern, rule);
	mmlStrMatchFree(cpattern);
	return result;
}

mml_core mmlInt32 mmlStrFind ( const mmlChar * str , const mmlChar * substr , mml_char_conversion_t rule )
{
	mmlInt32 str_len = mmlStrLength(str);
	mmlInt32 substr_len = mmlStrLength(substr);
	mmlInt32 offset = 0;
	for (; ; )
	{
		if ( offset >= str_len || offset + substr_len >= str_len )
		{
			return -1;
		}
		const mmlUInt8 * str1_mb = (mmlUInt8*)(str + offset);
		const mmlUInt8 * str2_mb = (mmlUInt8*)(substr);
		mmlInt32 str1_len = str_len - offset;
		mmlInt32 str2_len = substr_len;
		mmlInt32 sub_offset = 0;
		mmlInt32 first_sym_len = 0;
		for (; sub_offset < substr_len; sub_offset ++ )
		{
			mmlUChar c1;
			mmlInt32 offset = mml_utf8_mbtowc(&c1 , str1_mb , str1_len);
			if ( offset == 0 )
			{
				return -1;
			}
			if ( first_sym_len == 0 )
			{
				first_sym_len = offset;
			}
			str1_mb += offset; str1_len -= offset;
			c1 = rule != mmlNULL ? rule (c1) : c1;

			mmlUChar c2;
			offset = mml_utf8_mbtowc(&c2 , str2_mb , str2_len);
			if ( offset == 0 )
			{
				return -1;
			}
			str2_mb += offset; str2_len -= offset;
			c2 = rule != mmlNULL ? rule (c2) : c2;
			if ( c1 != c2 )
			{
				break;
			}
		}
		if ( sub_offset == substr_len )
		{
			break;
		}
		offset += first_sym_len;
	}
	return offset;
}