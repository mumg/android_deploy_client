#include "android_utils.h"
#include "mml_components.h"
#include "mmlIMemoryStream.h"
#include "mmlICStringStream.h"
#include "mmlIFileSystem.h"
#include "mmlICString.h"
#include "mmlIVariant.h"
#include "mmlIProcess.h"
#include "mmlIFileSystem.h"
#include "mmlIStreamHash.h"
#include "mml_md5_uuids.h"
#include "mml_services.h"
#include "mmlIVariantSerializer.h"
#include "mmlIVariantDeserializer.h"
#include "mmlIRegexp.h"
#include "android_http_fetch.h"
#include "android_updater_database.h"
#include "mmlIThread.h"
#include "mmlIMemoryBufferedQueue.h"

#define MML_JSON_VARIANT_SERIALIZER_UUID { 0x407065FF, 0x6C70, 0x1014, 0xBDA0 , { 0xF9, 0xC2, 0xF4, 0x90, 0x1F, 0xFA } }

#define MML_JSON_VARIANT_DESERIALIZER_UUID { 0x4383190F, 0x6C70, 0x1014, 0x9A99 , { 0x9C, 0xA2, 0x29, 0x60, 0xE2, 0xE9 } }

void log_main( const mmlChar * formatter , ... );

mmlBool android_args_parse( const mmlChar * arg_str, std::list< mmlAutoPtr < mmlICString > > & args_list )
{
	mmlSharedPtr < mmlICString > str = mmlNewObject(MML_OBJECT_UUID(mmlICString));
	mmlSharedPtr < mmlICStringOutputStream > arg_stream = mmlNewObject(MML_OBJECT_UUID(mmlICStringOutputStream), mmlObjectPtr < mmlIMemoryOutputStream > (str)); 
	mmlBool is_string = mmlFalse;
	while (*arg_str != 0 )
	{
		switch ( *arg_str )
		{
		case '\\':
			{
				arg_str ++;
				if ( *arg_str == 0)
				{
					return mmlFalse;
				}
				arg_stream->WriteChar(*arg_str);
				break;
			}
		case '"':
			{
				if ( is_string == mmlTrue )
				{
					is_string = mmlFalse;
				}
				else
				{
					is_string = mmlTrue;
					break;
				}
				arg_stream->Close();
				if ( str->Length() > 0 )
				{
					args_list.push_back((mmlICString*)str);
				}
				str = mmlNewObject(MML_OBJECT_UUID(mmlICString));
				arg_stream = mmlNewObject(MML_OBJECT_UUID(mmlICStringOutputStream), mmlObjectPtr < mmlIMemoryOutputStream > (str)); 
				break;
			}
		case ' ':
			{
				if ( is_string == mmlTrue )
				{
					arg_stream->WriteChar(' ');
				}
				else
				{
					arg_stream->Close();
					if ( str->Length() > 0 )
					{
						args_list.push_back((mmlICString*)str);
					}
					str = mmlNewObject(MML_OBJECT_UUID(mmlICString));
					arg_stream = mmlNewObject(MML_OBJECT_UUID(mmlICStringOutputStream), mmlObjectPtr < mmlIMemoryOutputStream > (str)); 
				}
				break;
			}
		default:
			{
				arg_stream->WriteChar(*arg_str);
			}
		}
		arg_str++;

	}
	arg_stream->Close();
	if ( str->Length() > 0 )
	{
		args_list.push_back((mmlICString*)str);
	}
	return mmlTrue;
}

#ifdef MML_ANDROID
#define POPEN popen
#define PCLOSE pclose
#else
#define POPEN _popen
#define PCLOSE _pclose
#endif


mmlBool android_run_process(const mmlChar * args, mmlIOutputStream * output, mmlInt32 * exit_code)
{
	FILE * proc_output = POPEN(args, "r");

	if (proc_output == mmlNULL)
	{
		log_main("could not open\n");
		return mmlFalse;
	}

	mmlUInt8 buffer[1024];
	while (!feof(proc_output))
	{
		size_t sz = fread(buffer, 1, sizeof(buffer), proc_output);
		if (sz <= 0)
		{
			break;
		}
		output->Write(sz, buffer);
	}
	if (exit_code != mmlNULL)
	{
		*exit_code = (PCLOSE(proc_output)) >> 8;
	}
	else
	{
		PCLOSE(proc_output);
	}

	return mmlTrue;
}

mmlBool android_run_process ( const mmlChar * args , mmlIInputStream ** res, mmlInt32 * exit_code)
{
	mmlSharedPtr < mmlIInputStream > output;
	mmlSharedPtr < mmlIMemoryOutputStream > stdout_stream = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));

	if ( android_run_process(args, stdout_stream, exit_code) == mmlFalse )
	{
		return mmlFalse;
	}

	stdout_stream->Close();
	mmlAutoPtr < mmlIBuffer > data;
	stdout_stream->GetData(data.getterAddRef());
	if (data == mmlNULL ||
		data->Size() == 0)
	{
		return mmlFalse;
	}
	output = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), data);
	if (output == mmlNULL)
	{
		return mmlFalse;
	}
	*res = output;
	MML_ADDREF(*res);
	return mmlTrue;
}

mmlBool android_run_process(const mmlChar * args, mmlICString ** result, mmlInt32 * exit_code)
{
	mmlSharedPtr < mmlIMemoryOutputStream > output = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));
	if (android_run_process(args, output, exit_code) == mmlFalse)
	{
		return mmlFalse;
	}
	output->Close();
	mmlAutoPtr < mmlIBuffer > buffer;
	output->GetData(buffer.getterAddRef());
	*result = mmlNewCStringFromBuffer(buffer);
	MML_ADDREF(*result);
	return mmlTrue;
}

mmlBool android_run_process_v ( const mmlChar * executable , std::list < mmlAutoPtr < mmlICString > > & args_list , mmlIOutputStream * output, mmlInt32 * result_code)
{
	*result_code = -1;
	mmlSharedPtr < mmlIProcess > proc = mmlNewObject(MML_OBJECT_UUID(mmlIProcess));
	if ( proc == mmlNULL )
	{
		return mmlFalse;
	}
	const mmlChar ** argv = mmlNULL;
	mmlInt32 index = 0;
	std::list < mmlAutoPtr < mmlICString > >::iterator arg = args_list.begin();
	if ( args_list.size() > 0 )
	{
		argv = (const mmlChar**)mmlAlloc(sizeof(mmlChar*) * (args_list.size()));
		for ( ; arg != args_list.end(); arg ++ )
		{
			argv[index] = (*arg)->Get();
			index ++;
		}
	}
	if ( proc->Exec(mmlCStaticString(executable), mmlNULL, index , argv, mmlNULL, output, output ) == mmlFalse )
	{
		if ( argv != mmlNULL) mmlFree(argv);
		return mmlFalse;
	}
	mmlBool finished = mmlFalse;
	mmlInt32 result = -1;
	proc->Wait(PROCESS_WAIT_INFINITE, &finished, result_code);
	if ( argv != mmlNULL) mmlFree(argv);
	return mmlTrue;
}


mmlBool android_get_packages ( std::list < mmlAutoPtr < mmlICString > > & packages )
{
	mmlAutoPtr < mmlIInputStream > result;
#ifdef MML_WIN
	const mmlChar * cmd = "pm list packages";
#else
	const mmlChar * cmd = "/system/bin/sh /system/bin/pm list packages";
#endif
	android_run_process(cmd, result.getterAddRef());
	if ( result == mmlNULL )
	{
		return mmlFalse;
	}
	//log_main("reading packages\n");
	mmlSharedPtr < mmlICStringInputStream > string_stream = mmlNewObject(MML_OBJECT_UUID(mmlICStringInputStream), result);
	for (;;)
	{
		mmlAutoPtr < mmlICString > line;
		if ( string_stream->ReadUntil("\n\r", mmlNULL, mmlNULL, mmlNULL, line.getterAddRef()) == mmlFalse )
		{
			break;
		}
		string_stream->Skip("\r\n", mmlNULL);
		if ( line != mmlNULL && line->Length() > 0)
		{
			mmlSharedPtr < mmlICStringInputStream > line_parser = mmlNewObject(MML_OBJECT_UUID(mmlICStringInputStream), mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), line));
			mmlAutoPtr < mmlICString > token;
			if ( line_parser->ReadToken(":", mmlTrue, token.getterAddRef()) == mmlTrue )
			{
				mmlBool eq;
				mmlInt32 c;
				if ( token->CompareStr(MML_EQUAL, "package", eq, MML_CASE_INSENSITIVE) == mmlTrue &&
					 eq == mmlTrue )
				{
					if ( line_parser->Skip(":", &c) == mmlTrue && c == 1)
					{
						line_parser->ReadToken(mmlNULL, mmlTrue, token.getterAddRef());
						if ( token != mmlNULL && token->Length() > 0)
						{
							packages.push_back(token);
						}
					}
				}
			}
		}
	}
	//log_main("read %d packages\n", packages.size());
	return mmlTrue;
}


void log_main_v( const mmlChar * formatter , va_list vp );

void log_variant_log_func( const mmlInt32 level, void * arg, const mmlChar * formatter , ... )
{
	mmlChar _formatter[64];
	mmlMemorySet(_formatter, ' ', level * 2);
	mmlMemoryCopy(_formatter + level * 2, formatter, mmlStrLength(formatter) + 1 );
	va_list va;
	va_start(va, formatter);
	log_main_v(_formatter, va);
	va_end(va);
}

mmlBool android_file_get_md5(mmlICString * path, mmlICString ** md5)
{
	mmlSharedPtr < mmlIFileSystem > fs = mmlGetService(MML_OBJECT_UUID(mmlIFileSystem));
	mmlAutoPtr < mmlIInputStream > file_body;
	if ( fs->OpenFile(path->Get(), file_body.getterAddRef()) == mmlTrue )
	{
		mmlSharedPtr < mmlIOutputStreamHash > md5_file_body = mmlNewObject(MML_MD5_OUTPUT_STREAM_UUID());
		if ( md5_file_body == mmlNULL )
		{
			return mmlFalse;
		}
		mmlSharedPtr < mmlIStreamCopy > copy = mmlNewObject(MML_OBJECT_UUID(mmlIStreamCopy));
		if ( copy->Copy(file_body, md5_file_body) == mmlTrue )
		{
			md5_file_body->Close();
			mmlAutoPtr < mmlIVariantString > file_md5;
			md5_file_body->GetHash(mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant>(file_md5));
			if ( file_md5 != mmlNULL )
			{
				*md5 = file_md5->Get();
				MML_ADDREF(*md5);
				return mmlTrue;
			}
		}
	}
	return mmlFalse;
}

class escape_map
{
public:
	escape_map(const mmlChar * escapeCharacters)
	{
		mmlMemorySet(&mEscapeMap, 0, sizeof(mEscapeMap));
		for (; *escapeCharacters != 0; escapeCharacters++)
		{
			mEscapeMap[(mmlUInt8)*escapeCharacters] = mmlTrue;
		}
	}

	void write(mmlICStringOutputStream * stream, mmlChar ch)
	{
		if (mEscapeMap[(mmlUInt8)ch] == mmlTrue)
		{
			stream->WriteFormatted("\\%c", ch);
		}
		else
		{
			stream->WriteChar(ch);
		}
	}

private:
	mmlBool mEscapeMap[256];
};

void android_cmd_escape_string(mmlICStringOutputStream* stream, mmlICString* string)
{
	static escape_map map("'\" *?[-]{}~$!&;()<>|#@");
	const mmlChar * str_ptr = string->Get();
	while (*str_ptr != 0)
	{
		map.write(stream, *str_ptr);
		str_ptr++;
	}
}


mmlBool android_file_check_md5(mmlICString * path, mmlICString * md5)
{
	mmlAutoPtr < mmlICString > file_md5;
	if ( android_file_get_md5(path, file_md5.getterAddRef()) == mmlTrue )
	{
		mmlBool eq;
		if ( file_md5->CompareStr(MML_EQUAL, md5->Get(), eq, MML_CASE_INSENSITIVE) == mmlTrue &&
			eq == mmlTrue )
		{
			return mmlTrue;
		}
	}
	return mmlFalse;
}

mmlBool android_json_serialize (mmlIVariant * object , mmlIBuffer ** data)
{
	mmlSharedPtr < mmlIMemoryOutputStream > request_output = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));
	static mmlUUID json_serializer_uuid = MML_JSON_VARIANT_SERIALIZER_UUID;
	mmlSharedPtr < mmlIVariantSerializer > json_serializer = mmlNewObject(json_serializer_uuid);
	if ( json_serializer == mmlNULL )
	{
		log_main("ERROR: Could not init json serializer\n");
		return mmlFalse;
	}
	if ( json_serializer->Write(request_output, object) == mmlFalse )
	{
		log_main("ERROR: Could not serialize object to JSON\n");
		return mmlFalse;
	}
	if ( request_output->Close() == mmlFalse )
	{
		return mmlFalse;
	}
	return request_output->GetData(data);
}

mmlBool android_query_server (mmlICString * url,  mmlIVariantStruct * config, mmlIVariantStruct * args , mmlIVariant ** result )
{
	mmlSharedPtr < mmlIFileSystem > fs = mmlGetService(MML_OBJECT_UUID(mmlIFileSystem));
	mmlSharedPtr < mmlIInputStream > request_content;
	{
		mmlAutoPtr < mmlIBuffer > data;
		if ( android_json_serialize(args, data.getterAddRef()) == mmlFalse )
		{
			return mmlFalse;
		}
		request_content = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), data);
	}

	mmlSharedPtr < mmlIOutputStream > response_output = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));

	mmlAutoPtr < mmlICString > response_content_type;
	if ( android_http_fetch(0, config, url, mmlCStaticString("application/json"),
		request_content,
		response_content_type.getterAddRef(),
		response_output) != HTTP_SUCCESS )
	{
		log_main("ERROR: Could not fetch from remote server\n");
		return mmlFalse;
	}
	if ( result != mmlNULL )
	{
		//deserialize response
		{
			response_output->Close();
			mmlSharedPtr < mmlIInputStream > response_input = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), response_output);

			static mmlUUID json_deserializer_uuid = MML_JSON_VARIANT_DESERIALIZER_UUID;
			mmlSharedPtr < mmlIVariantDeserializer > json_deserializer = mmlNewObject(json_deserializer_uuid);
			if ( json_deserializer == mmlNULL )
			{
				log_main("ERROR: Could not init json deserializer\n");
				return mmlFalse;
			}

			if ( json_deserializer->Read(response_input, result) == mmlFalse )
			{
				log_main("ERROR: Could not serialize object to JSON\n");
				return mmlFalse;
			}
		}
		if ( *result == mmlNULL )
		{
			return mmlFalse;
		}
	}
	return mmlTrue;
}

mmlBool android_json_deserialize(mmlIBuffer * data, mmlIVariant ** object)
{
	mmlSharedPtr < mmlIInputStream > response_input = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), data);

	static mmlUUID json_deserializer_uuid = MML_JSON_VARIANT_DESERIALIZER_UUID;
	mmlSharedPtr < mmlIVariantDeserializer > json_deserializer = mmlNewObject(json_deserializer_uuid);
	if (json_deserializer == mmlNULL)
	{
		log_main("ERROR: Could not init json deserializer\n");
		return mmlFalse;
	}

	return json_deserializer->Read(response_input, object);
}

#define WAKE_LOCK "/sys/power/wake_lock"

#define WAKE_UNLOCK "/sys/power/wake_unlock"

void android_echo(const mmlChar * file, const mmlChar * value)
{
	FILE * echo = fopen(file, "w");
	if (echo == mmlNULL)
	{
		return;
	}
	fprintf(echo, "%s\n", value);
	fclose(echo);
}

void android_wake_lock()
{
	android_echo(WAKE_LOCK, "UpdateServiceCore");
}

void android_wake_unlock()
{
	android_echo(WAKE_UNLOCK, "UpdateServiceCore");
}

std::list < START_STOP_MODULE_T * > modules;

void android_updater_register_init ( START_STOP_MODULE_T * module )
{
	modules.push_back(module);
}
mmlBool android_updater_init()
{
	for ( std::list < START_STOP_MODULE_T * >::iterator init = modules.begin(); init != modules.end(); init ++ )
	{
		if ( (*init)->start != mmlNULL )
		{
			if ( (*init)->start() == mmlFalse )
			{
				return mmlFalse;
			}
		}
	}
	return mmlTrue;
}
void android_updater_destroy()
{
	for ( std::list < START_STOP_MODULE_T * >::iterator init = modules.begin(); init != modules.end(); init ++ )
	{
		if ( (*init)->stop != mmlNULL )
		{
			(*init)->stop();
		}
	}
}

std::list < START_STOP_MODULE_T * > startstop;

void android_updater_register_startstop ( START_STOP_MODULE_T * module )
{
	startstop.push_back(module);
}
mmlBool android_updater_start()
{
	android_wake_lock();
	for ( std::list < START_STOP_MODULE_T * >::iterator init = startstop.begin(); init != startstop.end(); init ++ )
	{
		if ( (*init)->start != mmlNULL )
		{
			if ( (*init)->start() == mmlFalse )
			{
				return mmlFalse;
			}
		}
	}
	return mmlTrue;
}
void android_updater_stop()
{
	for ( std::list < START_STOP_MODULE_T * >::iterator init = startstop.begin(); init != startstop.end(); init ++ )
	{
		if ( (*init)->stop != mmlNULL )
		{
			(*init)->stop();
		}
	}
	android_wake_unlock();
}

class android_log_output_reader : public mmlIInputMemoryBufferedQueueReader
{
	MML_OBJECT_DECL
private:
	mmlChar mLogBuffer[256];
public:
	mmlBool Read(void * data, const mmlInt32 data_size, mmlInt32 * processed)
	{
		const mmlChar * ptr = (mmlChar*)data;
		for ( mmlInt32 idx = 0 ; idx < data_size; idx ++ )
		{
			if ( ptr[idx] == 0x0A || ptr[idx] == 0x0D )
			{
				mmlInt32 sz = idx > sizeof(mLogBuffer) - 2 ? sizeof(mLogBuffer) - 2 : idx;
				mmlMemoryCopy(mLogBuffer, data, sz);
				mLogBuffer[sz] = '\n';
				mLogBuffer[sz + 1] = 0;
				log_main(mLogBuffer);
				if ( idx + 1 < data_size && (ptr[idx+1] == 0x0A || ptr[idx + 1] == 0x0D))
				{
					*processed = idx + 2;
				}else
				{
					*processed = idx + 1;
				}
				break;
			}
		}
		return mmlTrue;
	}
	mmlBool Close(void * data, const mmlInt32 data_size)
	{
		if ( data_size > 0 )
		{
			mmlInt32 sz = data_size > sizeof(mLogBuffer) - 2 ? sizeof(mLogBuffer) - 2 : data_size;
			mmlMemoryCopy(mLogBuffer, data, sz);
			mLogBuffer[sz] = '\n';
			mLogBuffer[sz + 1] = 0;
			log_main(mLogBuffer);
		}
		return mmlTrue;
	}
};

MML_OBJECT_IMPL1(android_log_output_reader, mmlIInputMemoryBufferedQueueReader)

static mmlUUID log_output_uuid = { 0x09E048F9, 0x6C16, 0x1014, 0x8FF0 ,{ 0xF6, 0x04, 0xDB, 0x78, 0xFD, 0x32 } };

mmlIOutputStream * android_log_output()
{
	mmlIInputMemoryBufferedQueue * log_output = (mmlIInputMemoryBufferedQueue*)mmlGetService(log_output_uuid);
	if ( log_output == mmlNULL )
	{
		log_output = (mmlIInputMemoryBufferedQueue*)mmlNewObject(MML_OBJECT_UUID(mmlIInputMemoryBufferedQueue), new android_log_output_reader());
		if ( log_output != mmlNULL )
		{
			mmlSetService(log_output_uuid, log_output);
		}
	}
	return log_output;
}

mmlBool android_file_download(mmlICString * url, mmlICString * filename, mmlICString * md5, mmlICString ** fullpath)
{
	mmlSharedPtr < mmlIVariantStruct > config = mmlGetService(MML_GLOBAL_CONFIG_UUID());
	if (config == mmlNULL)
	{
		return mmlFalse;
	}
	mmlAutoPtr < mmlIVariantString > temp;
	config->Get("temp", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(temp));
	if (temp == mmlNULL)
	{
		return mmlFalse;
	}



	log_main("Downloading %s\n", filename->Get());
	MML_CREATE_CSTRING_MEMORY_OUTPUT_STREAM(path_stream);
	path_stream->WriteFormatted("%s/%s", temp->Get()->Get(), filename->Get());
	MML_CLOSE_CSTRING_MEMORY_OUTPUT_STREAM(path_stream);

	mmlSharedPtr < mmlIFileSystem > fs = mmlGetService(MML_OBJECT_UUID(mmlIFileSystem));

	if (fs->IsFileExists(path_stream_str->Get()) == mmlTrue)
	{
		if (android_file_check_md5(path_stream_str, md5) == mmlTrue)
		{
			log_main("File already downloaded\n");
			*fullpath = path_stream_str;
			MML_ADDREF(*fullpath);
			return mmlTrue;
		}
	}


	mmlAutoPtr < mmlIOutputStream > file_body;

	if (fs->OpenFile(path_stream_str->Get(), mmlTrue, file_body.getterAddRef()) == mmlFalse)
	{
		log_main("Could not open file for writing\n");
		return mmlFalse;
	}

	mmlInt32 offset = file_body->Size();
	mmlAutoPtr < mmlIVariantStruct > file_config;
	if (config->Get("file", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant>(file_config)) == mmlFalse)
	{
		return mmlFalse;
	}
	if (file_config == mmlNULL)
	{
		return mmlFalse;
	}
	HTTP_PROCESS_RESULT_T res;
	if ((res = android_http_fetch(offset, file_config, url, mmlNULL, mmlNULL, mmlNULL, file_body)) != HTTP_SUCCESS)
	{
		log_main("ERROR: Download failed\n");
		return mmlFalse;
	}
	log_main("Download complete\n");
	if (file_body->Close() == mmlFalse)
	{
		return mmlFalse;
	}
	if (android_file_check_md5(path_stream_str, md5) != mmlTrue)
	{
		return mmlFalse;
	}
	*fullpath = path_stream_str;
	MML_ADDREF(*fullpath);
	return mmlTrue;
}


mmlBool android_run_process_output(const mmlChar * args, mmlICString ** output)
{
	mmlSharedPtr < mmlIMemoryOutputStream > stream_output = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));
	mmlInt32 error_code = 0;
	if (android_run_process(
		args , stream_output, &error_code) == mmlFalse)
	{
		return mmlFalse;
	}
	if (error_code != 0)
	{
		return mmlFalse;
	}
	if (stream_output->Close() == mmlFalse)
	{
		return mmlFalse;
	}
	mmlAutoPtr<mmlIBuffer> output_data;
	stream_output->GetData(output_data.getterAddRef());
	*output = mmlNewCStringFromBuffer(output_data);
	MML_ADDREF(*output);
	return mmlTrue;
}

mmlBool android_run_process_match(const mmlChar * args, const mmlChar * regexp, const mmlBool full)
{
	mmlAutoPtr < mmlICString > output;
	if (android_run_process_output(args, output.getterAddRef()) == mmlFalse)
	{
		return mmlFalse;
	}
	mmlSharedPtr < mmlIRegexp > regexp_impl = mmlNewObject(MML_OBJECT_UUID(mmlIRegexp));
	if (regexp_impl == mmlNULL)
	{
		return mmlFalse;
	}
	return regexp_impl->Match(regexp, output, full);
}

mmlBool android_run_process_find(const mmlChar * args, const mmlChar * regexp, mmlICString ** substring)
{
	mmlAutoPtr < mmlICString > output;
	if (android_run_process_output(args, output.getterAddRef()) == mmlFalse)
	{
		return mmlFalse;
	}
	mmlSharedPtr < mmlIRegexp > regexp_impl = mmlNewObject(MML_OBJECT_UUID(mmlIRegexp));
	if (regexp_impl == mmlNULL)
	{
		return mmlFalse;
	}
	return regexp_impl->Find(regexp, output, substring);
}

mmlBool android_run_process_find(const mmlChar * args, const mmlChar * regexp, mmlIList ** sublist)
{
	mmlAutoPtr < mmlICString > output;
	if (android_run_process_output(args, output.getterAddRef()) == mmlFalse)
	{
		return mmlFalse;
	}
	mmlSharedPtr < mmlIRegexp > regexp_impl = mmlNewObject(MML_OBJECT_UUID(mmlIRegexp));
	if (regexp_impl == mmlNULL)
	{
		return mmlFalse;
	}
	return regexp_impl->Find(regexp, output, sublist);
}


mmlChar uuid_str[64] = { 0 };

void android_get_device_id(mmlICString ** id)
{
	if (mmlStrLength(uuid_str) != 0)
	{
		*id = mmlNewStaticCString(uuid_str);
		MML_ADDREF(*id);
		return;
	}
	mmlSharedPtr < mmlIVariantStruct > config = mmlGetService(MML_GLOBAL_CONFIG_UUID());
	mmlAutoPtr < mmlIVariantString > id_file;
	config->Get("uuid", mmlRelativePtrAddRef<mmlIVariantString, mmlIVariant>(id_file));



	mmlUUID uuid;
	mmlBool uuid_generate = mmlTrue;
	mmlSharedPtr < mmlIFileSystem> fs = mmlGetService(MML_OBJECT_UUID(mmlIFileSystem));
	if (fs->IsFileExists(id_file->Get()->Get()))
	{
		log_main("id file exists, reading...\n");
		mmlAutoPtr < mmlIInputStream > sin;
		if (fs->OpenFile(id_file->Get()->Get(), sin.getterAddRef()) == mmlTrue)
		{
			if (sin->Read(sizeof(uuid), &uuid) == sizeof(uuid))
			{
				log_main("id file read success\n");
				uuid_generate = mmlFalse;
			}
		}
	}
	if (uuid_generate == mmlTrue)
	{
		log_main("Generate new id\n");
		mmlUUIDGenerate(uuid);
		mmlAutoPtr < mmlIOutputStream > sout;
		if (fs->OpenFile(id_file->Get()->Get(), mmlFalse, sout.getterAddRef()) == mmlTrue)
		{
			sout->Write(sizeof(uuid), &uuid);
			sout->Close();
		}
	}
	mmlUUIDToStr(uuid, uuid_str, sizeof(uuid_str));
	log_main("id %s\n", uuid_str);
	*id = mmlNewStaticCString(uuid_str);
	MML_ADDREF(*id);
}