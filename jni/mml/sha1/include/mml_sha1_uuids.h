#ifndef MML_SHA1_UUIDS_HEADER_INCLUDED
#define MML_SHA1_UUIDS_HEADER_INCLUDED

#include "mmlIObject.h"

inline mmlUUID MML_SHA1_OUTPUT_STREAM_UUID()
{
	mmlUUID uuid = { 0xBD99D5DC, 0x7002, 0x1014, 0xBEBB , { 0x97, 0x5E, 0xFC, 0x4A, 0x83, 0x91 } };
	return uuid;
}

inline mmlUUID MML_SHA1_INPUT_STREAM_UUID()
{
	mmlUUID uuid = { 0xBF7763AF, 0x7002, 0x1014, 0xBDF7 , { 0xF8, 0x26, 0x13, 0x92, 0xDB, 0xDF } };
	return uuid;
}

#endif