#ifndef MML_MD5_UUIDS_HEADER_INCLUDED
#define MML_MD5_UUIDS_HEADER_INCLUDED

#include "mmlIObject.h"

inline mmlUUID MML_MD5_OUTPUT_STREAM_UUID()
{
	mmlUUID uuid = { 0x7FF65E3A, 0x7002, 0x1014, 0xA5EC , { 0xF3, 0x6B, 0x5A, 0x95, 0x8F, 0x75 } };
	return uuid;
}

inline mmlUUID MML_MD5_INPUT_STREAM_UUID()
{
	mmlUUID uuid = { 0x81DC3CC5, 0x7002, 0x1014, 0xB2CB , { 0x97, 0x7A, 0xB5, 0x02, 0xE7, 0x44 } };
	return uuid;
}

#endif