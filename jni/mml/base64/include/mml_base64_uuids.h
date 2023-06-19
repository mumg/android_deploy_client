#ifndef MML_BASE64_UUIDS_HEADER_INCLUDED
#define MML_BASE64_UUIDS_HEADER_INCLUDED

#include "mmlIObject.h"

inline mmlUUID MML_BASE64_OUTPUT_STREAM_UUID()
{
	mmlUUID uuid = { 0x88FA440A, 0x7005, 0x1014, 0x8678 , { 0x89, 0x70, 0x77, 0x72, 0x7F, 0x5A } };
	return uuid;
}

inline mmlUUID MML_BASE64_INPUT_STREAM_UUID()
{
	mmlUUID uuid = { 0x8B0DD01B, 0x7005, 0x1014, 0x8BE9 , { 0x8A, 0xC7, 0x98, 0x3B, 0x45, 0xD7 } };
	return uuid;
}

#endif