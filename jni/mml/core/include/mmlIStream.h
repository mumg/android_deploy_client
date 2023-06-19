#ifndef MML_STREAM_HEADER_INCLUDED
#define MML_STREAM_HEADER_INCLUDED

#include "mmlIObject.h"
#include "mmlIVariant.h"

typedef enum
{
	MML_SEEK_SET,
	MML_SEEK_CUR,
	MML_SEEK_END
}MML_SEEK_T;

class mmlIStreamSignal : public mmlIObject
{
public:
	virtual void Signal() = 0;
};

#define MML_STREAM_CONTROL_UUID { 0x9E9B9216, 0x6DA9, 0x1014, 0xB47C , { 0xFB, 0x50, 0xFF, 0x1F, 0xF7, 0xA0 } }

class mmlIStreamControl : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_STREAM_CONTROL_UUID)
public:

	virtual mmlBool SetSignal(mmlIStreamSignal * signal) = 0;

	virtual mmlBool SetTimeout ( const mmlInt32 timeout ) = 0;

	virtual mmlBool GetTimeout ( mmlInt32 * timeout ) = 0;

	virtual mmlInt64 GetPosition () = 0;

	virtual mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset ) = 0;

};

#define MML_INPUT_STREAM_UUID { 0x0EDAB4CB, 0x6CA1, 0x1014, 0x9E39 , { 0xDA, 0xCA, 0xC4, 0x95, 0xCF, 0xE6 } }

class mmlIInputStream : public mmlIObject
{
    MML_OBJECT_UUID_DECL(MML_INPUT_STREAM_UUID)
public:

	virtual mmlBool GetControl ( mmlIStreamControl ** control ) = 0;

    virtual mmlInt64 Read ( const mmlInt64 size , void * data ) = 0;

	virtual mmlBool IsEmpty () = 0;

	virtual mmlInt64 Size () = 0;

	virtual mmlBool Close () = 0;

};

#define MML_OUTPUT_STREAM_CONTROL_UUID { 0x6C024537, 0x6DA9, 0x1014, 0xBF2C , { 0xB4, 0x75, 0x5F, 0xCF, 0xDE, 0x9C } }

class mmlIOutputStreamControl : public mmlIStreamControl
{
	MML_OBJECT_UUID_DECL(MML_OUTPUT_STREAM_CONTROL_UUID)
public:

	virtual mmlBool SetMaxSize ( const mmlInt64 size ) = 0;

	virtual mmlInt64 GetMaxSize () =0;

};


#define MML_OUTPUT_STREAM_UUID { 0x50F6A959, 0x6C0F, 0x1014, 0xBDCB , { 0x93, 0xD0, 0xD4, 0xFE, 0xEA, 0x79 } }

class mmlIOutputStream : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_OUTPUT_STREAM_UUID)
public:
	virtual mmlInt64 Write ( const mmlInt64 size , const void * data ) = 0;

	virtual mmlBool IsFull () = 0;

	virtual mmlBool GetControl ( mmlIStreamControl ** control ) = 0;

	virtual mmlBool Close () = 0;

	virtual mmlInt64 Size () = 0;

	virtual mmlBool Flush () = 0;
};


#define MML_BUFFERED_INPUT_STREAM_UUID { 0x0EDAB4CB, 0x6CA1, 0x1014, 0x9E39 , { 0xDA, 0xCA, 0xC4, 0x95, 0xCF, 0xE7 } }

class mmlIBufferedInputStream : public mmlIInputStream
{
	MML_OBJECT_UUID_DECL(MML_BUFFERED_INPUT_STREAM_UUID)
public:

	virtual mmlBool SetBufferSize (const mmlInt32 size) = 0;

};


#define MML_BUFFERED_OUTPUT_STREAM_UUID { 0x50F6A959, 0x6C0F, 0x1014, 0xBDCB , { 0x93, 0xD0, 0xD4, 0xFE, 0xEA, 0x80 } }

class mmlIBufferedOutputStream : public mmlIOutputStream
{
	MML_OBJECT_UUID_DECL(MML_BUFFERED_OUTPUT_STREAM_UUID)
public:

	virtual mmlBool SetBufferSize (const mmlInt32 size) = 0;

};

#define MML_STREAM_SERVER_HANDLER_UUID { 0xF3653022, 0x6DCF, 0x1014, 0xA3BC , { 0xFE, 0xE2, 0x8A, 0x29, 0x57, 0xD0 } }

class mmlIStreamServerHandler : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_STREAM_SERVER_HANDLER_UUID)
public:
	virtual mmlBool OnConnect ( mmlICString * iface, mmlICString * source, mmlInt32 * chunk_size, mmlIOutputStream ** input, mmlIInputStream ** output) = 0;
};


#define MML_STREAM_SERVER_UUID { 0xE5471465, 0x6DCF, 0x1014, 0x9D5F , { 0xB4, 0x60, 0x2C, 0xDD, 0x3D, 0x69 } }

class mmlIStreamServer : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_STREAM_SERVER_UUID)
public:
	virtual mmlBool Start ( mmlIVariantStruct * config, mmlIStreamServerHandler * handler ) = 0;
	virtual mmlBool Stop () = 0;
};

#define MML_STREAM_CLIENT_UUID { 0x522538FD, 0x6DD9, 0x1014, 0xB76E , { 0xBD, 0xEA, 0x92, 0xC9, 0xEE, 0xE4 } }

typedef enum
{
	NSC_OK,
	NSC_REFUSED,
	NSC_ERROR
}NSC_RESULT_T;

class mmlIStreamClient : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_STREAM_CLIENT_UUID)
public:
	virtual NSC_RESULT_T Connect ( mmlIVariantStruct * config,
		                      mmlIInputStream ** input,
							  mmlIOutputStream ** output ) = 0;
	virtual mmlBool IsConnected () = 0;
};

class mmlIStreamCopyProgress : public mmlIObject
{
public:
	virtual void OnProgress (const mmlInt64 copied) = 0;
};

#define MML_STREAM_COPY_UUID { 0x66B05CA5, 0x6FB1, 0x1014, 0x8724 , { 0xFD, 0x76, 0x15, 0x11, 0x23, 0x5D } }

class mmlIStreamCopy : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_STREAM_COPY_UUID)
public:
	virtual void SetProgressListener ( mmlIStreamCopyProgress * listener ) = 0;
	virtual mmlBool SetChunkSize ( const mmlInt32 size ) = 0;
	virtual mmlBool Copy ( mmlIInputStream * in, mmlIOutputStream * out, const mmlInt64 size = 0) = 0;
};

#endif//
