#ifndef MML_CRC32_UUIDS_HEADER_INCLUDED
#define MML_CRC32_UUIDS_HEADER_INCLUDED

#include "mmlIObject.h"

inline mmlUUID MML_CRC32_OUTPUT_STREAM_UUID()
{
	mmlUUID uuid = { 0x5E7E495F, 0x7002, 0x1014, 0x9317 , { 0xA9, 0x49, 0xCA, 0xA1, 0xBF, 0x30 } };
	return uuid;
}

inline mmlUUID MML_CRC32_INPUT_STREAM_UUID()
{
	mmlUUID uuid = { 0x647F99CA, 0x7002, 0x1014, 0xBA73 , { 0x90, 0x06, 0xC0, 0xEF, 0x0D, 0x59 } };
	return uuid;
}

#endif