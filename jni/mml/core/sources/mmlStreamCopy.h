#ifndef MML_STREAM_COPY_HEADER_INCLUDED
#define MML_STREAM_COPY_HEADER_INCLUDED

#include "mmlIStream.h"

class mmlStreamCopy : public mmlIStreamCopy
{
	MML_OBJECT_DECL
public:

	mmlStreamCopy();

	mmlBool SetChunkSize ( const mmlInt32 size );
	mmlBool Copy ( mmlIInputStream * in, mmlIOutputStream * out, const mmlInt64 size);
	void SetProgressListener ( mmlIStreamCopyProgress * listener ) { mListener = listener; }
private:

	mmlInt32 mChunkSize;
	mmlAutoPtr < mmlIStreamCopyProgress > mListener;
};


#endif //MML_STREAM_COPY_HEADER_INCLUDED
