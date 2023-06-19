#include "mmlWebSocketClient.h"
#include "mmlIHTTPClient.h"
#include "mmlIRandom.h"
#include "mml_components.h"
#include "mml_base64_uuids.h"
#include "mml_sha1_uuids.h"
#include "mmlIStreamHash.h"
#include "mmlIVariantDeserializer.h"
#include "mmlIVariantSerializer.h"
#include "mmlIMutex.h"
#include "mmlIFileSystem.h"
#include "mml_services.h"

#define IN_MEMORY_SIZE 1024*1024*16
#define MAX_PING_COUNT 5

#ifdef MML_WIN
#pragma pack( push , 1)
#endif
typedef struct  
{

	mmlUInt16 opcode:4;
	mmlUInt16 rsrv1:1;
	mmlUInt16 rsrv2:1;
	mmlUInt16 rsrv3:1;
	mmlUInt16 fin:1;

	mmlUInt16 len:7;
	mmlUInt16 mask:1;
}
#ifndef MML_WIN
__attribute__((packed)) 
#endif
	WEBSOCKET_FRAME;
#ifdef MML_WIN
#pragma pack(pop)
#endif


void log_websocket(const mmlChar * formatter, ...);

void log_websocket_v( const mmlChar * formatter , va_list vp );

void log_websocket_variant_log_func( const mmlInt32 level, void * arg, const mmlChar * formatter , ... )
{
	mmlChar _formatter[64];
	mmlMemorySet(_formatter, ' ', level * 2);
	mmlMemoryCopy(_formatter + level * 2, formatter, mmlStrLength(formatter) + 1 );
	va_list va;
	va_start(va, formatter);
	log_websocket_v(_formatter, va);
	va_end(va);
}

#define WEBSOCKET_MESSAGE_UUID { 0x119366C4, 0x6C30, 0x1014, 0xAB57 , { 0xCD, 0x12, 0x2B, 0x54, 0xEC, 0xE6 } }

class websocket_message : public mmlIObject
{
	MML_OBJECT_UUID_DECL(WEBSOCKET_MESSAGE_UUID)
public:
	typedef enum
	{
		PING,
		PONG,
		TEXT,
		DATA
	}TYPE_T;

	websocket_message (const TYPE_T type )
		:mType(type)
	{

	}

	TYPE_T GetType()
	{
		return mType;
	}
private:
	TYPE_T mType;
};

#define WEBSOCKET_MESSAGE_PING_UUID { 0x17E5C788, 0x6C30, 0x1014, 0xA85E , { 0x9B, 0xAD, 0x6E, 0x45, 0xC2, 0xDE } }

class websocket_message_ping : public websocket_message
{
	MML_OBJECT_DECL
	MML_OBJECT_UUID_DECL(WEBSOCKET_MESSAGE_PING_UUID)
private:
	mmlAutoPtr < mmlIBuffer > mBuffer;
public:
	mmlIBuffer * Get()
	{
		return mBuffer;
	}

	websocket_message_ping(mmlIBuffer * buffer)
		:websocket_message(PING), mBuffer(buffer)
	{

	}

};

MML_OBJECT_IMPL2(websocket_message_ping, websocket_message_ping, websocket_message)

#define WEBSOCKET_MESSAGE_PONG_UUID { 0x29A1B0F1, 0x6C30, 0x1014, 0xA97F , { 0x8A, 0x99, 0x2B, 0xB0, 0x07, 0x30 } }

class websocket_message_pong : public websocket_message
{
	MML_OBJECT_DECL
	MML_OBJECT_UUID_DECL(WEBSOCKET_MESSAGE_PONG_UUID)
private:
	mmlAutoPtr < mmlIBuffer > mBuffer;
public:
	mmlIBuffer * Get()
	{
		return mBuffer;
	}

	websocket_message_pong(mmlIBuffer * buffer)
		:websocket_message(PONG), mBuffer(buffer)
	{

	}

};

MML_OBJECT_IMPL2(websocket_message_pong, websocket_message_pong, websocket_message)

#define WEBSOCKET_MESSAGE_MSG_UUID { 0x32819816, 0x6C30, 0x1014, 0xA86C , { 0x87, 0x9D, 0x01, 0x7E, 0xDA, 0x27 } }


class websocket_message_msg : public websocket_message
{
	MML_OBJECT_DECL
	MML_OBJECT_UUID_DECL(WEBSOCKET_MESSAGE_MSG_UUID)
private:
	mmlAutoPtr < mmlIInputStream > mMsg;
	mmlAutoPtr < mmlIWebSocketMessageListener > mListener;
public:
	mmlIInputStream * GetMsg()
	{
		return mMsg;
	}

	mmlIWebSocketMessageListener * GetListener()
	{
		return mListener;
	}

	websocket_message_msg(const mmlBool binary, mmlIInputStream * msg, mmlIWebSocketMessageListener * listener)
		:websocket_message(binary == mmlFalse ? TEXT : DATA), mMsg(msg), mListener(listener)
	{

	}

};

MML_OBJECT_IMPL2(websocket_message_msg, websocket_message_msg, websocket_message)


#define SWAP16(n) (((((mmlUInt16)(n) & 0xFF)) << 8) | (((mmlUInt16)(n) & 0xFF00) >> 8))

#define SWAP64(n)  (((n & 0xFF00000000000000ull) >> 56) \
	              | ((n & 0x00FF000000000000ull) >> 40) \
	              | ((n & 0x0000FF0000000000ull) >> 24) \
	              | ((n & 0x000000FF00000000ull) >>  8) \
	              | ((n & 0x00000000FF000000ull) <<  8) \
	              | ((n & 0x0000000000FF0000ull) << 24) \
	              | ((n & 0x000000000000FF00ull) << 40) \
	              | ((n & 0x00000000000000FFull) << 56)) \


class websocket_output_stream : public mmlIOutputStream
{
	MML_OBJECT_DECL
private:
	mmlAutoPtr < mmlIOutputStream > mStream;
	mmlBool mUseMask;
	mmlBool mBinary;
	mmlSharedPtr < mmlIRandom > mRand;

	mmlUInt8 * mBuffer;
	mmlInt32 mBufferSize;
	mmlInt32 mBufferDataLen;

	mmlSharedPtr < mmlIMemoryOutputStream > mCache;

public:

	websocket_output_stream( mmlIOutputStream * output, const mmlBool binary, const mmlBool use_mask)
		:mStream(output), mUseMask(use_mask), mBinary(binary)
	{
		mRand = mmlNewObject(MML_OBJECT_UUID(mmlIRandom));
	}

	mmlInt64 Write ( const mmlInt64 size , const void * data )
	{
		if ( mCache == mmlNULL )
		{
			mCache = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));
		}
		return mCache->Write(size, data);
	}

	mmlBool IsFull ()
	{
		return mmlFalse;
	}

	mmlBool GetControl ( mmlIStreamControl ** control )
	{
		return mmlFalse;
	}

	mmlBool Close ()
	{
		return mmlTrue;
	}

	mmlInt64 Size ()
	{
		return 0;
	}

	mmlBool Flush ()
	{
		if ( mCache->Close() == mmlFalse )
		{
			return mmlFalse;
		}
		mmlSharedPtr < mmlIInputStream > input = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), mCache);
		mCache = mmlNULL;
		WEBSOCKET_FRAME frame = {0};
		frame.fin= 1;
		frame.mask = mUseMask == mmlTrue ? 1 : 0;
		frame.opcode =  mBinary == mmlFalse ? 0x01 : 0x02;
		mmlInt64 size = input->Size();
		frame.len = size < 126 ? size : 126;
		if ( size > 65536 )
		{
			frame.len = 127;
		}
		if ( mStream->Write(sizeof(WEBSOCKET_FRAME), &frame) != sizeof(WEBSOCKET_FRAME))
		{
			return mmlFalse;
		}
		if ( frame.len == 126 )
		{

			mmlUInt16 sz = SWAP16((mmlUInt16)size);
			if ( mStream->Write(sizeof(sz), &sz) != sizeof(sz))
			{
				return mmlFalse;
			}
		}
		else if ( frame.len == 127 )
		{
			mmlUInt64 sz = SWAP64((mmlUInt64)size);
			if ( mStream->Write(sizeof(size), &size) != sizeof(size))
			{
				return mmlFalse;
			}
		}
		if ( mUseMask == mmlTrue )
		{
			mmlUInt8 mask[4];
			mRand->NewRandomFill(mask, 4);
			if ( mStream->Write(4, mask) != 4 )
			{
				return mmlFalse;
			}
			mmlInt64 size = input->Size();
			for ( mmlInt32 index = 0; index < size ; index ++)
			{
				mmlUInt8 b;
				if ( input->Read(1, &b) != 1 )
				{
					return mmlFalse;
				}
				b = b ^ mask[index % 4];
				if ( mStream->Write(1, &b) != 1 )
				{
					return mmlFalse;
				}
			}
		}
		else 
		{
			mmlSharedPtr < mmlIStreamCopy > copy = mmlNewObject(MML_OBJECT_UUID(mmlIStreamCopy));
			if ( copy == mmlNULL )
			{
				return mmlFalse;
			}
			if ( copy->Copy(input, mStream) == mmlFalse )
			{
				return mmlFalse;
			}
		}
		return mStream->Flush();
	}
};

MML_OBJECT_IMPL1(websocket_output_stream, mmlIOutputStream)


class websocket_receiver : public mmlIThreadFunction
{
	MML_OBJECT_DECL
		MML_THREAD_DECLARE_SHUTDOWN_FLAG
private:
	mmlAutoPtr < mmlIInputStream > mInput;
	mmlBool mConnected;
	mmlAutoPtr < mmlIWebSocketClientListener > mListener;
	mmlSharedPtr < mmlIFileSystem > mFS;
	mmlAutoPtr < mmlIObjectQueue > mQueue;
	mmlIWebSocketClient * mClient;
public:

	mmlBool IsConnected ()
	{
		return mConnected;
	}

	websocket_receiver ( mmlIInputStream * stream , mmlIWebSocketClient * client, mmlIWebSocketClientListener * listener, mmlIObjectQueue * queue)
		:mInput(stream), mConnected(mmlTrue), mListener(listener), mQueue(queue), mClient(client)
	{
		mFS = mmlGetService(MML_OBJECT_UUID(mmlIFileSystem));
	}

	mmlBool ReadData ( void * data, const mmlInt32 size )
	{
		mmlUInt8 * ptr = (mmlUInt8 *)data;
		mmlInt32 off = 0;
		while ( off < size )
		{
			mmlInt64 rd = mInput->Read(size - off, ptr + off);
			if ( IsShutdown() == mmlTrue )
			{
				return mmlFalse;
			}
			if ( rd < 0 )
			{
				return mmlFalse;
			}
			if ( rd > 0 )
			{
				off += rd;
			}
		}
		return mmlTrue;
	}

	void Execute (mmlIThread * thread)
	{
		mmlAutoPtr < mmlIStreamControl > control;
		if ( mInput->GetControl(control.getterAddRef()) == mmlTrue )
		{
			control->SetTimeout(100);
		}
		mmlSharedPtr < mmlIInputStream > input_loopback;
		mmlSharedPtr < mmlIMemoryOutputStream > output_loopback;
		while ( IsShutdown() == mmlFalse )
		{
			WEBSOCKET_FRAME frame;
			if ( ReadData(&frame, sizeof(frame)) == mmlFalse ) 
			{
				log_websocket("Could not read websocket frame\n"); break; 
			}
			if ( frame.opcode == 0x08 )
			{
				if ( frame.len > 0 )
				{
					mmlChar * reason = (mmlChar *)mmlAlloc(frame.len + 1);
					ReadData(reason, frame.len);
					log_websocket("Connection has been closed by remote size with reason\"%s\"\n", reason);

					mmlFree(reason);
				}
				else
				{
					log_websocket("Connection has been closed by remote size\n");
				}
				break;
			}
			if ( frame.opcode == 0x01 || frame.opcode == 0x02)
			{
				output_loopback = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));
			}
			else if ( frame.opcode == 0x09 )
			{
				mmlAutoPtr < mmlIBuffer > ping_buffer = mmlAllocateBuffer(frame.len);
				if ( frame.len > 0 )
				{
					if ( ReadData(ping_buffer->Get(), frame.len) == mmlFalse )  {log_websocket("Could not read ping data\n"); break; }
				}
				mQueue->Post(new websocket_message_ping(ping_buffer));
				continue;
			}
			else if ( frame.opcode == 0x0A )
			{
				mmlAutoPtr < mmlIBuffer > pong_buffer = mmlAllocateBuffer(frame.len);
				if ( frame.len > 0 )
				{
					if ( ReadData(pong_buffer->Get(), frame.len) == mmlFalse )  {log_websocket("Could not read ping data\n"); break; }
				}
				mQueue->Post(new websocket_message_pong(pong_buffer));
				continue;
			}
			mmlInt64 size = frame.len;
			if ( size == 126 )
			{
				mmlUInt16 sz;
				if ( ReadData(&sz, sizeof(sz)) == mmlFalse )  {log_websocket("Could not read websocket data\n"); break; }
				size = SWAP16(sz);
			}
			else if ( size == 127 )
			{
				if ( ReadData(&size, sizeof(size)) == mmlFalse )  {log_websocket("Could not read websocket data\n"); break; }
				size = SWAP64(size);
			}
			mmlChar buffer[4096];
			//log_websocket("received frame with len %" MML_INT64 "\n", size);
			while (size > 0 )
			{
				mmlInt32 sz = size > sizeof(buffer) ? sizeof(buffer) : size;
				if ( ReadData(buffer,sz) == mmlFalse )  {log_websocket("Could not read websocket data\n"); break; }
				if ( output_loopback != mmlNULL )
				{
					if ( output_loopback->Write(sz, buffer) != sz )
					{
						log_websocket("Could not write data to loopback\n"); break;
					}
				}
				size -= sz;
			}
			if ( frame.fin == 1)
			{
				if ( output_loopback != mmlNULL )
				{
					if ( output_loopback->Close() != mmlTrue )
					{
						log_websocket("Could not close loopback\n"); break;
					}
					input_loopback = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), output_loopback);
					if ( mListener->OnReceiveMessage(mClient, input_loopback) == mmlFalse )
					{
						log_websocket("On receive message returned error\n"); break;
					}
				}
			}
		}
		mConnected = mmlFalse;
	}

};

MML_OBJECT_IMPL0(websocket_receiver)

class websocket_http_processor : public mmlIHTTPClientProcessorGet
{
	MML_OBJECT_DECL

private:

	mmlSharedPtr < mmlICString > mBase64;

	mmlAutoPtr < mmlIWebSocketClientListener > mListener;

	mmlAutoPtr < websocket_receiver > mReceiver;

	mmlSharedPtr < mmlIThread > mReceiverThread;

	mmlAutoPtr < mmlIVariantStruct > mConfig;

	mmlAutoPtr < mmlIObjectQueue > mQueue;

	mmlIWebSocketClient * mClient;

	mmlBool mHasConnected;


public:
	websocket_http_processor(mmlIWebSocketClient * client, mmlIVariantStruct * config, mmlIObjectQueue * queue, mmlIWebSocketClientListener * listener)
		:mListener(listener), mConfig(config), mClient(client), mQueue(queue), mHasConnected(mmlFalse)
	{
	}

	mmlBool GetHasConnected()
	{
		return mHasConnected;
	}

	HTTP_PROCESS_RESULT_T OnPrepare ( const mmlChar * scheme, mmlIHTTPRequest * request , mmlBool * use_ssl )
	{
		if ( mmlStrCompare(scheme, "wss", MML_CASE_INSENSITIVE) == 0 )
		{
			*use_ssl = mmlTrue;
		}
		request->SetMethod(METHOD_GET);

		mmlSharedPtr < mmlIRandom > rnd = mmlNewObject(MML_OBJECT_UUID(mmlIRandom));

		if ( rnd == mmlNULL )
		{
			return HTTP_ERROR;
		}

		mmlUInt8 nonce[16] = {0};
		rnd->NewRandomFill(nonce, 16);
		
		mBase64 = mmlNewObject(MML_OBJECT_UUID(mmlICString));
		mmlSharedPtr < mmlIOutputStream > base64s = mmlNewObject(MML_BASE64_OUTPUT_STREAM_UUID(), mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream), mBase64));
		if ( base64s->Write(16, nonce) != 16 || base64s->Flush() == mmlFalse || base64s->Close() == mmlFalse )
		{
			return HTTP_ERROR;
		}
		request->SetHeader("Upgrade", "websocket");
		request->SetHeader("Connection", "Upgrade");
		request->SetHeader("Sec-WebSocket-Version", "13");
		request->SetHeader("Sec-WebSocket-Key", mBase64->Get());
		log_websocket("Websocket key%s\n", mBase64->Get());
		request->GetReqURI()->SetVersion(1.1);
		return HTTP_SUCCESS;
	}


	HTTP_PROCESS_RESULT_T OnReceive ( mmlIHTTPResponse * response,
		                mmlIInputStream * input,
		                mmlIOutputStream * output )
	{
		if ( response->GetResponseCode() != HTTP_SWITCHING_PROTOCOL )
		{
			return HTTP_ERROR;
		}
		const mmlChar * connection = response->GetHeader("Connection");
		if ( connection == mmlNULL )
		{
			return HTTP_ERROR;
		}
		if ( mmlStrCompare(connection, "upgrade", MML_CASE_INSENSITIVE) != 0 )
		{
			return HTTP_ERROR;
		}
		const mmlChar * upgrade = response->GetHeader("upgrade");
		if ( upgrade == mmlNULL )
		{
			return HTTP_ERROR;
		}
		if ( mmlStrCompare(upgrade, "websocket", MML_CASE_INSENSITIVE) != 0 )
		{
			return HTTP_ERROR;
		}
		const mmlChar * websocket_accept = response->GetHeader("Sec-WebSocket-Accept");
		if ( websocket_accept == mmlNULL )
		{
			return HTTP_ERROR;
		}
		const mmlChar *UUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
		mmlSharedPtr < mmlIOutputStreamHash > sha1 = mmlNewObject(MML_SHA1_OUTPUT_STREAM_UUID());
		if ( sha1 == mmlNULL )
		{
			return HTTP_ERROR;
		}
		if ( sha1->Write(mBase64->Length(), mBase64->Get()) != mBase64->Length())
		{
			return HTTP_ERROR;
		}
		if ( sha1->Write(mmlStrLength(UUID), UUID) != mmlStrLength(UUID))
		{
			return HTTP_ERROR;
		}
		mmlAutoPtr < mmlIBuffer > response_sha1;
		if ( sha1->GetHash(response_sha1.getterAddRef()) == mmlFalse )
		{
			return HTTP_ERROR;
		}
		mmlSharedPtr < mmlICString > response_base64 = mmlNewObject(MML_OBJECT_UUID(mmlICString));
		mmlSharedPtr < mmlIOutputStream > base64s = mmlNewObject(MML_BASE64_OUTPUT_STREAM_UUID(), mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream), response_base64));
		if ( base64s->Write(response_sha1->Size(), response_sha1->Get()) != response_sha1->Size() || base64s->Flush() == mmlFalse || base64s->Close() == mmlFalse )
		{
			return HTTP_ERROR;
		}
		mmlBool eq = mmlFalse;
		if ( response_base64->CompareStr(MML_EQUAL, websocket_accept, eq, MML_CASE_SENSITIVE) == mmlFalse )
		{
			return HTTP_ERROR;
		}
		if ( eq == mmlFalse )
		{
			return HTTP_ERROR;
		}
		if ( mListener == mmlNULL )
		{
			return HTTP_ERROR;
		}
		mReceiverThread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
		mReceiver = new websocket_receiver(input, mClient, mListener, mQueue);
		mReceiverThread->Create(mReceiver, PriorityNormal, MML_DEFAULT_STACK_SIZE);
		mmlAutoPtr < mmlIVariantInteger > ping_period;
		mmlAutoPtr < mmlIVariantInteger > ping_timeout;
		if ( mConfig->Get("ping_period", mmlRelativePtrAddRef < mmlIVariantInteger , mmlIVariant>(ping_period)) == mmlFalse )
		{
			ping_period = mmlNewVariantInteger(60000);
		}
		if ( mConfig->Get("ping_timeout", mmlRelativePtrAddRef < mmlIVariantInteger , mmlIVariant>(ping_timeout)) == mmlFalse )
		{
			ping_timeout = mmlNewVariantInteger(30000);
		}
		mml_timespec next_ping_request;
		mml_clock_gettime_monotonic(&next_ping_request);
		mml_clock_add(&next_ping_request, ping_period->Get());
		mmlInt32 ping_sent = 0;

		mml_timespec ping_response_expire;
		log_websocket("Connection has been established\n");
		mHasConnected = mmlTrue;
		class connection_holder
		{
		private:
			mmlIWebSocketClientListener * mListener;
			mmlIWebSocketClient * mClient;
		public:
			connection_holder ( mmlIWebSocketClient * client, mmlIWebSocketClientListener * listener )
				:mListener(listener), mClient(client)
			{
				mListener->OnConnect(mClient);
			}

			~connection_holder()
			{
				mListener->OnDisconnect();
			}

		}ch(mClient, mListener) ;
		while ( mListener->IsShutdown() == mmlFalse && mReceiver->IsConnected() == mmlTrue)
		{
			mml_timespec ct;
			mml_clock_gettime_monotonic(&ct);
			if ( ping_sent < MAX_PING_COUNT && mml_clock_cmp(&next_ping_request, &ct) < 0 )
			{
				ping_sent ++;
                if ( ping_sent > 1 )
                {
				    log_websocket("Sent %d ping\n", ping_sent);
                }

				mml_clock_gettime_monotonic(&next_ping_request);
				mml_clock_add(&next_ping_request, ping_period->Get());

				mml_clock_gettime_monotonic(&ping_response_expire);
				mml_clock_add(&ping_response_expire, ping_timeout->Get());


				WEBSOCKET_FRAME frame = {0};
				frame.fin= 1;
				frame.mask = 0;
				frame.opcode = 0x09;
				frame.len = 0;

				if ( output->Write(sizeof(WEBSOCKET_FRAME), &frame) != sizeof(WEBSOCKET_FRAME))
				{
					return HTTP_ERROR;
				}
				if ( output->Flush() == mmlFalse )
				{
					return HTTP_ERROR;
				}
				if ( mListener->OnPingSent() == mmlFalse )
				{
					return HTTP_ABORTED;
				}
			}
			if (ping_sent == MAX_PING_COUNT )
			{
				if ( mml_clock_cmp(&ping_response_expire, &ct) < 0 )
				{
					log_websocket("pong wait time is over\n");
					return HTTP_ERROR;
				}
			}
			mmlAutoPtr < websocket_message > msg;
			if ( mQueue->Get(100, msg.getterAddRef()) == mmlTrue )
			{
				if ( msg->GetType() == websocket_message::PING )
				{
					mmlSharedPtr < websocket_message_ping > ping = (websocket_message*)msg;
					WEBSOCKET_FRAME frame = {0};
					frame.fin= 1;
					frame.opcode = 0x0A;
					frame.mask = 0;
					frame.len = ping->Get()->Size();
					if ( frame.len > 125 )
					{
						log_websocket("pong frame size to big %d\n", ping->Get()->Size());
						return HTTP_ERROR;
					}
					if ( output->Write(sizeof(WEBSOCKET_FRAME), &frame) != sizeof(WEBSOCKET_FRAME))
					{
						return HTTP_ERROR;
					}
					if ( frame.len > 0 )
					{
						mmlSharedPtr < mmlIStreamCopy > copy = mmlNewObject(MML_OBJECT_UUID(mmlIStreamCopy));
						mmlSharedPtr < mmlIInputStream > buffer = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), ping->Get());
						if ( copy->Copy(buffer, output) == mmlFalse)
						{
							return HTTP_ERROR;
						}
					}
					if ( output->Flush() == mmlFalse )
					{
						return HTTP_ERROR;
					}
					if ( mListener->OnPingReceived() == mmlFalse )
					{
						return HTTP_ABORTED;
					}
				}
				else if ( msg->GetType() == websocket_message::PONG )
				{
					mml_clock_gettime_monotonic(&next_ping_request);
					mml_clock_add(&next_ping_request, ping_period->Get());
                    if ( ping_sent > 1 )
                    {
                        log_websocket("pong received\n");
                    }
					ping_sent = 0;
					

				}
				else if ( msg->GetType() == websocket_message::DATA || msg->GetType() == websocket_message::TEXT)
				{
					mmlSharedPtr < websocket_message_msg > message = (websocket_message*)msg; 
					if ( message->GetMsg() != mmlNULL )
					{
						mmlSharedPtr < mmlIOutputStream > serializer_output = new websocket_output_stream(output, msg->GetType() == websocket_message::DATA ? mmlTrue: mmlFalse, mmlFalse);
						mmlSharedPtr < mmlIStreamCopy > copy = mmlNewObject(MML_OBJECT_UUID(mmlIStreamCopy));
						//log_websocket("send frame with len %" MML_INT64 "\n", message->GetMsg()->Size());
						if ( copy->Copy(message->GetMsg(), serializer_output) == mmlFalse )
						{
							if ( message->GetListener() != mmlNULL ) message->GetListener()->OnMessageResult(mmlIWebSocketMessageListener::ERROR);
						}
						else if ( serializer_output->Flush() == mmlFalse )
						{
							if ( message->GetListener() != mmlNULL ) message->GetListener()->OnMessageResult( mmlIWebSocketMessageListener::ERROR);
						}
						else
						{
							if ( message->GetListener() != mmlNULL ) message->GetListener()->OnMessageResult(mmlIWebSocketMessageListener::OK);
						}
					}
				}
			}
		}
		return HTTP_SUCCESS;
	}

};

MML_OBJECT_IMPL1(websocket_http_processor,  mmlIHTTPClientProcessorGet)

mmlBool mmlWebSocketClient::Connect (mmlIVariantStruct * config, const mmlChar * url , mmlIWebSocketClientListener * listener )
{
	mmlSharedPtr < mmlIHTTPClient > client = mmlNewObject(MML_OBJECT_UUID(mmlIHTTPClient));
	if ( client == mmlNULL )
	{
		return mmlFalse;
	}
	mQueue = mmlNewObject(MML_OBJECT_UUID(mmlIObjectQueue));
	mmlAutoPtr < websocket_http_processor > processor = new websocket_http_processor(this, config, mQueue, listener);
	client->Process(config, url, processor);
	return processor->GetHasConnected();
}

mmlBool mmlWebSocketClient::SendMessage ( const mmlBool binary, mmlIInputStream * message , mmlIWebSocketMessageListener * listener )
{
	return mQueue->Post(new websocket_message_msg(binary, message, listener));
}

MML_OBJECT_IMPL1(mmlWebSocketClient, mmlIWebSocketClient)