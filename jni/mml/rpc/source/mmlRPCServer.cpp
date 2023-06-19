#include "mmlRPCServer.h"
#include "mmlIThread.h"
#include "mmlIObjectQueue.h"
#include "mmlIMemoryBufferedQueue.h"
#include "mmlIMemoryStream.h"
#include "mmlIStreamHash.h"
#include "mml_components.h"
#include "mml_crc32_uuids.h"
#include "mmlIVariantSerializer.h"
#include "mmlIVariantDeserializer.h"

void log_rpc(const mmlChar * formatter, ...);

#define  MML_RPC_CMD_UUID { 0x0AD4FF4A, 0x6D8F, 0x1014, 0x9C3E , { 0xD3, 0x29, 0x19, 0xD5, 0x47, 0x9F } }

static mmlUUID binary_serializer = { 0x6392D37F, 0x6C6E, 0x1014, 0xAE5B ,{ 0xDC, 0x81, 0x31, 0xDC, 0xEA, 0xA1 } };

enum
{
	HANDSHAKE = 0,
	REQUEST = 1,
	RESPONSE = 2,
	RESPONSE_EXCEPTION = 3,
	PING = 4,
	PONG = 5
};

class mml_rpc_cmd : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_RPC_CMD_UUID)
	MML_OBJECT_DECL
private:
	mmlAutoPtr < mmlIOutputMemoryBufferedQueueStream > mOutput;


	void _reponse(mmlUInt8 type, mmlIVariant * value)
	{
		mmlSharedPtr < mmlIMemoryOutputStream > output = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));
		mmlUInt32 header = 0xDEADBEAF;
		output->Write(sizeof(header), &header);
		output->Write(sizeof(type), &type);
		mmlSharedPtr < mmlIMemoryOutputStream > body_stream = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));

		mmlSharedPtr < mmlIOutputStreamHash > crc32 = mmlNewObject(MML_CRC32_OUTPUT_STREAM_UUID(), body_stream);

		crc32->Write(sizeof(mmlUUID), &id);

		mmlSharedPtr < mmlIVariantSerializer > serializer = mmlNewObject(binary_serializer);

		serializer->Write(crc32, value);

		crc32->Flush();

		mmlAutoPtr < mmlIVariantInteger > crc32_value;
		crc32->GetHash(mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(crc32_value));

		crc32->Close();

		mmlAutoPtr < mmlIBuffer > body_data;
		body_stream->GetData(body_data.getterAddRef());

		//size
		header = body_data->Size();
		output->Write(sizeof(header), &header);
		output->Write(body_data->Size(), body_data->Get());
		//crc32
		header = crc32_value->Get();
		output->Write(sizeof(header), &header);
		output->Close();
		mmlAutoPtr < mmlIBuffer > response_data;
		output->GetData(response_data.getterAddRef());
		mOutput->Write(response_data->Size(), response_data->Get());
	}

public:
	mmlAutoPtr < mmlIVariant > request;
	mmlUUID id;
	mmlUUID connection_id;
	mmlAutoPtr < mmlIRPCServerFunction > function;

	mml_rpc_cmd(mmlIRPCServerFunction * _function, mmlUUID & _id, mmlUUID & _connection_id, mmlIVariant * _request, mmlIOutputMemoryBufferedQueueStream * output)
		:function(_function), id(_id), connection_id(_connection_id), request(_request), mOutput(output)
	{

	}

	void respond(mmlIVariant * response)
	{
		_reponse(RESPONSE, response);
	}

	void exception(mmlIVariantString * exception)
	{
		_reponse(RESPONSE_EXCEPTION, exception);
	}
};

MML_OBJECT_IMPL1(mml_rpc_cmd, mml_rpc_cmd)


mml_rpc_cmd_worker::mml_rpc_cmd_worker(mmlIObjectQueue * queue)
{
	mQueue = queue;
	mThread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
	mThread->Create(new mmlThreadFunctionProxy<mml_rpc_cmd_worker>(this), PriorityNormal, MML_DEFAULT_STACK_SIZE);
}

mml_rpc_cmd_worker::~mml_rpc_cmd_worker()
{
	mThread->Shutdown();
	mThread->Join();
}

void mml_rpc_cmd_worker::Execute(mmlIThread * thread)
{
	while (IsShutdown() == mmlFalse)
	{
		mmlAutoPtr < mml_rpc_cmd > cmd;
		if (mQueue->Get(100, cmd.getterAddRef()) == mmlTrue)
		{
			mmlAutoPtr < mmlIVariant > response;
			mmlAutoPtr < mmlIVariantString > exception;
			mmlBool store = mmlTrue;
			mmlAutoPtr < mmlICString > reason;
			if (cmd->function->run(cmd->request, response.getterAddRef(), exception.getterAddRef()) == mmlTrue)
			{
				cmd->respond(response);
			}
			else
			{
				cmd->exception(exception);
			}
		}
	}
}

MML_OBJECT_IMPL0(mml_rpc_cmd_worker)



static mmlUUID binary_deserializer = { 0x6E1257B1, 0x6C6E, 0x1014, 0x8067 ,{ 0x8F, 0x47, 0x3C, 0xAD, 0xC9, 0x8C } };


class mmlRCPServerBufferedQueueReader : public mmlIInputMemoryBufferedQueueReader
{
	MML_OBJECT_DECL
private:
	mmlAutoPtr < mmlIObjectQueue > mRequestQueue;
	mmlAutoPtr < mmlIRPCServerAuthenticator > mAuthenticator;
	mmlSharedPtr < mmlIOutputMemoryBufferedQueueStream > mOutput;
	mml_timespec mCreated;
	mmlBool mEstablished;
	mmlAutoPtr < mmlICString > mSource;
	mmlAutoPtr < mmlICString > mIface;
	mmlAutoPtr < mmlRPCServerStreamHandler > mHandler;
	mmlSharedPtr < mmlIVariantDeserializer > mDeserializer;
	
	mmlUUID mConnectionId;

	
public:

	mmlRCPServerBufferedQueueReader(mmlRPCServerStreamHandler * handler,
		                            mmlICString * source,
		                            mmlICString * iface,
		                            mmlIRPCServerAuthenticator * authenticator, 
		                            mmlIObjectQueue * request_queue, 
		                            mmlIOutputMemoryBufferedQueue * output)
		:mAuthenticator(authenticator), mRequestQueue(request_queue), mSource(source), mIface(iface), mHandler(handler)
	{
		mml_clock_gettime_monotonic(&mCreated);
		mEstablished = mmlFalse;
		mDeserializer = mmlNewObject(binary_deserializer);
		mOutput = mmlNewObject(MML_OBJECT_UUID(mmlIOutputMemoryBufferedQueueStream), output);
		mmlUUIDGenerate(mConnectionId);
	}

	mmlBool Read(void * data, const mmlInt32 data_size, mmlInt32 * processed)
	{
		if (mDeserializer == mmlNULL)
		{
			return mmlFalse;
		}
		//header 9 bytes
		if (data_size < 9)
		{
			return mmlTrue;
		}
		mmlUInt32 * size = (mmlUInt32*)(((mmlUInt8*)data) + 5);
		mmlAutoPtr < mmlICString > function;
		mmlSharedPtr < mmlIInputStream > body;
		if (*size > 0)
		{
			if (data_size < *size + 9 /* header */ + 4 /*crc*/)
			{
				//waiting for whole packet
				return mmlTrue;
			}
			if (*((mmlUInt32*)data) != 0xDEADBEAF)
			{
				//invalid header, drop connection
				return mmlFalse;
			}
			mmlSharedPtr < mmlIOutputStreamHash > crc32_stream = mmlNewObject(MML_CRC32_OUTPUT_STREAM_UUID());
			if (crc32_stream == mmlNULL)
			{
				return mmlFalse;
			}
			if (crc32_stream->Write(*size, (mmlUInt8*)data + 9) != *size)
			{
				return mmlFalse;
			}
			mmlUInt32 *msg_crc32 = (mmlUInt32*)(((mmlUInt8*)data) + 9 + *size);
			mmlAutoPtr < mmlIVariantInteger > calc_crc32;
			crc32_stream->GetHash(mmlRelativePtrAddRef<mmlIVariantInteger, mmlIVariant>(calc_crc32));
			if (calc_crc32 == mmlNULL)
			{
				return mmlFalse;
			}
			if (*msg_crc32 != calc_crc32->Get())
			{
				//crc doesn't match
				return mmlFalse;
			}
			body = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), mmlNewStaticBuffer((mmlUInt8*)data + 9, *size));
			if (body == mmlNULL)
			{
				return mmlFalse;
			}
		}
		*processed = 9 + *size + 4;
		mmlUInt8 * type = (mmlUInt8*)data + 4; //type
		if (mEstablished == mmlFalse)
		{
			if (*type != HANDSHAKE)
			{
				return mmlFalse;
			}
			if (mAuthenticator != mmlNULL)
			{
				mmlAutoPtr < mmlIVariantString > token;
				if (body != mmlNULL)
				{
					if (mDeserializer->Read(body, mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(token)) == mmlFalse)
					{
						return mmlFalse;
					}
					if (token == mmlNULL)
					{
						return mmlFalse;
					}
				}

				if (mAuthenticator->Authenticate(mSource != mmlNULL ? mSource->Get() : mmlNULL,
					mIface != mmlNULL ? mIface->Get() : mmlNULL,
					token != mmlNULL ? token->Get()->Get() : mmlNULL) == mmlFalse)
				{
					return mmlFalse;
				}
			}
			mmlSharedPtr < mmlIBufferedOutputStream > output = mmlNewObject(MML_OBJECT_UUID(mmlIBufferedOutputStream), mOutput);
			mmlUInt32 header = 0xDEADBEAF;
			output->Write(sizeof(header), &header);
			mmlUInt8 type = HANDSHAKE;
			output->Write(sizeof(type), &type);
			//size
			header = 0;
			output->Write(sizeof(header), &header);
			mEstablished = mmlTrue;
			return output->Flush();

		}
		else if (*type == PING )
		{
			mmlSharedPtr < mmlIBufferedOutputStream > output = mmlNewObject(MML_OBJECT_UUID(mmlIBufferedOutputStream), mOutput);
			mmlUInt32 header = 0xDEADBEAF;
			output->Write(sizeof(header), &header);
			mmlUInt8 type = PONG;
			output->Write(sizeof(type), &type);
			//size
			header = 0;
			output->Write(sizeof(header), &header);
			return output->Flush();
		}
		else if (*type == REQUEST && body != mmlNULL)
		{
			mmlUUID id;
			if (body->Read(sizeof(id), &id) != sizeof(id))
			{
				return mmlFalse;
			}
			mmlAutoPtr < mmlIVariantString > cmd;
			if (mDeserializer->Read(body, mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(cmd)) == mmlFalse)
			{
				return mmlFalse;
			}
			if (cmd == mmlNULL)
			{
				return mmlFalse;
			}
			mmlAutoPtr < mmlIVariant > request;
			if (mDeserializer->Read(body, request.getterAddRef()) == mmlFalse)
			{
				return mmlFalse;
			}
			mmlIRPCServerFunction * function = mHandler->Get(cmd->Get()->Get());
			mmlAutoPtr < mml_rpc_cmd > cmd_instance = new mml_rpc_cmd(function, id, mConnectionId, request, mOutput);
			if (function == mmlNULL)
			{
				cmd_instance->exception(mmlNewVariantString(mmlNewCString("function not found")));
				return mmlTrue;
			}
			else
			{
				return mRequestQueue->Post(cmd_instance);
			}
		}
		return mmlFalse;
	}
	mmlBool Close(void * data, const mmlInt32 data_size)
	{
		return mmlTrue;
	}
};

MML_OBJECT_IMPL1(mmlRCPServerBufferedQueueReader, mmlIInputMemoryBufferedQueueReader)

mmlRPCServerStreamHandler::mmlRPCServerStreamHandler()
{
	mRequestQueue = mmlNewObject(MML_OBJECT_UUID(mmlIObjectQueue));
	mWorkers.push_back(new mml_rpc_cmd_worker(mRequestQueue));
}

mmlBool mmlRPCServerStreamHandler::Construct(mmlIRPCServerAuthenticator * authenticator)
{
	mAuthenticator = authenticator;
	return mmlTrue;
}

void mmlRPCServerStreamHandler::Add(const mmlChar * name, mmlIRPCServerFunction * function)
{
	mFunctions[name] = function;
}

mmlIRPCServerFunction * mmlRPCServerStreamHandler::Get(const mmlChar * name)
{
	std::map < std::string, mmlAutoPtr < mmlIRPCServerFunction > > ::iterator func = mFunctions.find(name);
	if (func == mFunctions.end())
	{
		return mmlNULL;
	}
	return func->second;
}

void  mmlRPCServerStreamHandler::SetWorkers(const mmlInt32 count)
{
	if (mWorkers.size() < count)
	{
		for (mmlInt32 cnt = mWorkers.size(); cnt < count; cnt++)
		{
			mWorkers.push_back(new mml_rpc_cmd_worker(mRequestQueue));
		}
	}
	else
	{
		for (mmlInt32 cnt = mWorkers.size(); cnt < count; cnt++)
		{
			mWorkers.pop_back();
		}
	}
}

mmlBool mmlRPCServerStreamHandler::OnConnect(mmlICString * iface, mmlICString * source, mmlInt32 * chunk_size, mmlIOutputStream ** input, mmlIInputStream ** output)
{
	*chunk_size = 4096;
	log_rpc("rpc incoming connection\n");
	mmlSharedPtr < mmlIOutputMemoryBufferedQueue> _output = mmlNewObject(MML_OBJECT_UUID(mmlIOutputMemoryBufferedQueue));
	if (_output == mmlNULL)
	{
		return mmlFalse;
	}
	//_output->setDump("-> ", log_rpc);
	mmlSharedPtr < mmlIInputMemoryBufferedQueue> _input = mmlNewObject(MML_OBJECT_UUID(mmlIInputMemoryBufferedQueue),
		new mmlRCPServerBufferedQueueReader(this, source, iface, mAuthenticator, mRequestQueue, _output));
	//_input->setDump("<- ", log_rpc);
	if (_input == mmlNULL)
	{
		return mmlFalse;
	}
	*output = _output;
	MML_ADDREF(*output);
	*input = _input;
	MML_ADDREF(*input);
	return mmlTrue;
}

MML_OBJECT_IMPL1(mmlRPCServerStreamHandler, mmlIRPCServerStreamHandler)