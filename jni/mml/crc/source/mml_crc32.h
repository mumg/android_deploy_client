#ifndef CRC32_HDR
#define CRC32_HDR

class mml_crc32
{
private:
	mmlUInt32 mCRC32_tab[256];
	mmlUInt64 mCRC32;
public:

	mml_crc32 ()
	{
		mmlUInt64 crc, poly;
		mmlInt32 i, j;

		poly = 0xEDB88320L;
		for (i = 0; i < 256; i++)
		{
			crc = i;
			for (j = 8; j > 0; j--)
			{
				if (crc & 1)
				{
					crc = (crc >> 1) ^ poly;
				}
				else
				{
					crc >>= 1;
				}
			}
			mCRC32_tab[i] = crc;
		}
		mCRC32 = 0xFFFFFFFF;
	}

	mmlUInt32 Get()
	{
		return (mCRC32 ^ 0xFFFFFFFF);
	}

	void Update (mmlUInt8 *block, mmlUInt32 length)
	{
		for (mmlUInt32 i = 0; i < length; i++)
		{
			mCRC32 = ((mCRC32 >> 8) & 0x00FFFFFF) ^ mCRC32_tab[(mCRC32 ^ *block++) & 0xFF];
		}
	}

	void Reset ()
	{
		mCRC32 = 0xFFFFFFFF; 
	}
};


#endif