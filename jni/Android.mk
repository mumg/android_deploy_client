
LOCAL_PATH := $(call my-dir)
########################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/core/include \
                    jni/mml/core/sources \
                    jni/mml/core/sources/portable/posix
LOCAL_CFLAGS    := -DMML_ANDROID -DMML_POSIX

LOCAL_MODULE    := mml_core
LOCAL_ARM_MODE   := arm 
LOCAL_SRC_FILES := \
	mml/core/sources/mmlCString.cpp \
	mml/core/sources/mmlCStringStream.cpp \
	mml/core/sources/mmlCStringUtils.cpp \
	mml/core/sources/mmlList.cpp \
	mml/core/sources/mmlLogger.cpp \
	mml/core/sources/mml_components.cpp \
	mml/core/sources/mml_library.cpp \
	mml/core/sources/mml_memory.cpp \
	mml/core/sources/mml_services.cpp \
	mml/core/sources/mml_strutils.cpp \
	mml/core/sources/mmlVariant.cpp \
	mml/core/sources/mml_time_common.cpp \
    mml/core/sources/mmlMemoryStream.cpp \
	mml/core/sources/mmlBuffer.cpp \
	mml/core/sources/mmlCommandLine.cpp \
	mml/core/sources/mmlFileSystem.cpp \
	mml/core/sources/mmlStateMachine.cpp \
	mml/core/sources/mml_config.cpp \
	mml/core/sources/mmlBufferedStream.cpp \
	mml/core/sources/mmlStreamCopy.cpp \
	mml/core/sources/mmlMemoryBufferedQueue.cpp \
	mml/core/sources/mmlObjectQueue.cpp \
	mml/core/sources/portable/posix/mmlCondition.cpp \
	mml/core/sources/portable/posix/mmlMutex.cpp \
	mml/core/sources/portable/posix/mmlRandom.cpp \
	mml/core/sources/portable/posix/mmlSleep.cpp \
	mml/core/sources/portable/posix/mmlThread.cpp \
	mml/core/sources/portable/posix/mml_time.cpp \
	mml/core/sources/portable/posix/mmlTimer.cpp \
	mml/core/sources/portable/posix/mmlDirectory.cpp \
    mml/core/sources/portable/posix/mmlFileSystemOS.cpp \
	mml/core/sources/portable/posix/mmlPerformanceCounter.cpp \
	mml/core/sources/portable/posix/mml_atomic.cpp


include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/core/include jni/mml/serialization/include jni/mml/serialization/json 
LOCAL_CFLAGS    := -DMML_ANDROID
LOCAL_ARM_MODE   := arm 
LOCAL_MODULE    := mml_json
LOCAL_SRC_FILES := \
	mml/serialization/json/mml_json_serialization.cpp \
    mml/serialization/json/mmlJSONDeserializer.cpp \
	mml/serialization/json/mmlJSONSerializer.cpp
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/core/include jni/mml/serialization/include jni/mml/serialization/binary 
LOCAL_CFLAGS    := -DMML_ANDROID
LOCAL_ARM_MODE   := arm 
LOCAL_MODULE    := mml_binary
LOCAL_SRC_FILES := \
	mml/serialization/binary/mml_binary_serialization.cpp \
    mml/serialization/binary/mmlBinaryDeserializer.cpp \
	mml/serialization/binary/mmlBinarySerializer.cpp
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/thirdparty/mbedtls/include \
                    jni/mml/thirdparty/mbedtls/library
LOCAL_ARM_MODE   := arm 					
LOCAL_MODULE    := mbedcrypto
LOCAL_SRC_FILES := \
    mml/thirdparty/mbedtls/library/aes.c \
    mml/thirdparty/mbedtls/library/aesni.c \
    mml/thirdparty/mbedtls/library/arc4.c \
    mml/thirdparty/mbedtls/library/asn1parse.c \
    mml/thirdparty/mbedtls/library/asn1write.c \
    mml/thirdparty/mbedtls/library/base64.c \
    mml/thirdparty/mbedtls/library/bignum.c \
    mml/thirdparty/mbedtls/library/blowfish.c \
    mml/thirdparty/mbedtls/library/camellia.c \
    mml/thirdparty/mbedtls/library/ccm.c \
    mml/thirdparty/mbedtls/library/cipher.c \
    mml/thirdparty/mbedtls/library/cipher_wrap.c \
    mml/thirdparty/mbedtls/library/cmac.c \
    mml/thirdparty/mbedtls/library/ctr_drbg.c \
    mml/thirdparty/mbedtls/library/des.c \
    mml/thirdparty/mbedtls/library/dhm.c \
    mml/thirdparty/mbedtls/library/ecdh.c \
    mml/thirdparty/mbedtls/library/ecdsa.c \
    mml/thirdparty/mbedtls/library/ecjpake.c \
    mml/thirdparty/mbedtls/library/ecp.c \
    mml/thirdparty/mbedtls/library/ecp_curves.c \
    mml/thirdparty/mbedtls/library/entropy.c \
    mml/thirdparty/mbedtls/library/entropy_poll.c \
    mml/thirdparty/mbedtls/library/error.c \
    mml/thirdparty/mbedtls/library/gcm.c \
    mml/thirdparty/mbedtls/library/havege.c \
    mml/thirdparty/mbedtls/library/hmac_drbg.c \
    mml/thirdparty/mbedtls/library/md.c \
    mml/thirdparty/mbedtls/library/md2.c \
    mml/thirdparty/mbedtls/library/md4.c \
    mml/thirdparty/mbedtls/library/md5.c \
    mml/thirdparty/mbedtls/library/md_wrap.c \
    mml/thirdparty/mbedtls/library/memory_buffer_alloc.c \
    mml/thirdparty/mbedtls/library/oid.c \
    mml/thirdparty/mbedtls/library/padlock.c \
    mml/thirdparty/mbedtls/library/pem.c \
    mml/thirdparty/mbedtls/library/pk.c \
    mml/thirdparty/mbedtls/library/pk_wrap.c \
    mml/thirdparty/mbedtls/library/pkcs12.c \
    mml/thirdparty/mbedtls/library/pkcs5.c \
    mml/thirdparty/mbedtls/library/pkparse.c \
    mml/thirdparty/mbedtls/library/pkwrite.c \
    mml/thirdparty/mbedtls/library/platform.c \
    mml/thirdparty/mbedtls/library/ripemd160.c \
    mml/thirdparty/mbedtls/library/rsa.c \
    mml/thirdparty/mbedtls/library/sha1.c \
    mml/thirdparty/mbedtls/library/sha256.c \
    mml/thirdparty/mbedtls/library/sha512.c \
    mml/thirdparty/mbedtls/library/threading.c \
    mml/thirdparty/mbedtls/library/timing.c \
    mml/thirdparty/mbedtls/library/version.c \
    mml/thirdparty/mbedtls/library/version_features.c \
    mml/thirdparty/mbedtls/library/xtea.c

	
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/thirdparty/mbedtls/include \
                    jni/mml/thirdparty/mbedtls/library
LOCAL_ARM_MODE   := arm 					
LOCAL_MODULE    := mbedx509
LOCAL_SRC_FILES := \
    mml/thirdparty/mbedtls/library/certs.c \
    mml/thirdparty/mbedtls/library/pkcs11.c \
    mml/thirdparty/mbedtls/library/x509.c \
    mml/thirdparty/mbedtls/library/x509_create.c \
    mml/thirdparty/mbedtls/library/x509_crl.c \
    mml/thirdparty/mbedtls/library/x509_crt.c \
    mml/thirdparty/mbedtls/library/x509_csr.c \
    mml/thirdparty/mbedtls/library/x509write_crt.c \
    mml/thirdparty/mbedtls/library/x509write_csr.c

	
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/thirdparty/mbedtls/include \
                    jni/mml/thirdparty/mbedtls/library
LOCAL_ARM_MODE   := arm 					
LOCAL_MODULE    := mbedtls
LOCAL_SRC_FILES := \
    mml/thirdparty/mbedtls/library/debug.c \
    mml/thirdparty/mbedtls/library/net_sockets.c \
    mml/thirdparty/mbedtls/library/ssl_cache.c \
    mml/thirdparty/mbedtls/library/ssl_ciphersuites.c \
    mml/thirdparty/mbedtls/library/ssl_cli.c \
    mml/thirdparty/mbedtls/library/ssl_cookie.c \
    mml/thirdparty/mbedtls/library/ssl_srv.c \
    mml/thirdparty/mbedtls/library/ssl_ticket.c \
    mml/thirdparty/mbedtls/library/ssl_tls.c

	
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/core/include \
                    jni/mml/network/include \
					jni/mml/network/source \
					jni/mml/thirdparty/mbedtls/include
					
LOCAL_CFLAGS    := -DMML_ANDROID -DMML_SSL
LOCAL_ARM_MODE   := arm 
LOCAL_MODULE    := mml_network
LOCAL_SRC_FILES := \
	mml/network/source/mml_network.cpp \
	mml/network/source/mmlNetworkStreamServer.cpp \
	mml/network/source/mmlNetworkStreamClientSSL.cpp \
	mml/network/source/mmlNetworkStreamClient.cpp \
	mml/network/source/mmlNetworkCommon.cpp 
	
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/core/include \
					jni/mml/database/include \
                    jni/mml/thirdparty/sqlite3

					
LOCAL_CFLAGS    := -DMML_ANDROID
LOCAL_ARM_MODE   := arm 
LOCAL_MODULE    := mml_database
LOCAL_SRC_FILES := \
	mml/database/source/mml_database.cpp \
	mml/database/source/mmlDatabase.cpp

	
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/core/include \
                    jni/mml/scheduler/include
					
LOCAL_CFLAGS    := -DMML_ANDROID
LOCAL_ARM_MODE   := arm 
LOCAL_MODULE    := mml_scheduler
LOCAL_SRC_FILES := \
	mml/scheduler/source/mmlScheduler.cpp
	
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/core/include \
                    jni/mml/http/include \
					jni/mml/network/include \
					jni/mml/http/source
					
LOCAL_CFLAGS    := -DMML_ANDROID
LOCAL_ARM_MODE   := arm 
LOCAL_MODULE    := mml_http
LOCAL_SRC_FILES := \
	mml/http/source/mml_http.cpp \
	mml/http/source/mmlHTTPClient.cpp \
	mml/http/source/mmlHTTPRequest.cpp \
	mml/http/source/mmlHTTPResponse.cpp \
	mml/http/source/mmlHTTPServer.cpp
				
	
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/core/include \
                    jni/mml/base64/include \
					jni/mml/base64/source
					
LOCAL_CFLAGS    := -DMML_ANDROID
LOCAL_ARM_MODE   := arm 
LOCAL_MODULE    := mml_base64
LOCAL_SRC_FILES := \
	mml/base64/source/mml_base64.cpp \
    mml/base64/source/mmlBase64OutputStream.cpp \
	mml/base64/source/mmlBase64InputStream.cpp

	
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/core/include \
                    jni/mml/sha1/include \
					jni/mml/sha1/source \
					jni/mml/sha1/sha1
					
LOCAL_CFLAGS    := -DMML_ANDROID
LOCAL_ARM_MODE   := arm 
LOCAL_MODULE    := mml_sha1
LOCAL_SRC_FILES := \
	mml/sha1/source/mml_sha1.cpp \
    mml/sha1/source/mmlSHA1OutputStream.cpp \
	mml/sha1/source/mmlSHA1InputStream.cpp \
	mml/sha1/sha1/sha1.c
	
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/core/include \
                    jni/mml/md5/include \
					jni/mml/md5/source \
					jni/mml/md5/md5
					
LOCAL_CFLAGS    := -DMML_ANDROID
LOCAL_ARM_MODE   := arm 
LOCAL_MODULE    := mml_md5
LOCAL_SRC_FILES := \
	mml/md5/source/mml_md5.cpp \
    mml/md5/source/mmlMD5InputStream.cpp \
	mml/md5/source/mmlMD5OutputStream.cpp \
	mml/md5/md5/md5.c
	
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/core/include \
                    jni/mml/crc/include \
					jni/mml/crc/source
					
LOCAL_CFLAGS    := -DMML_ANDROID
LOCAL_ARM_MODE   := arm 
LOCAL_MODULE    := mml_crc
LOCAL_SRC_FILES := \
	mml/crc/source/mml_crc32.cpp \
    mml/crc/source/mmlCRC32InputStream.cpp \
	mml/crc/source/mmlCRC32OutputStream.cpp
	
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/core/include \
                    jni/mml/http/include \
					jni/mml/serialization/include \
					jni/mml/websocket/include  \
					jni/mml/base64/include \
					jni/mml/sha1/include \
					jni/mml/websocket/source 
					
LOCAL_CFLAGS    := -DMML_ANDROID
LOCAL_ARM_MODE   := arm 
LOCAL_MODULE    := mml_websocket
LOCAL_SRC_FILES := \
	mml/websocket/source/mml_websocket.cpp \
    mml/websocket/source/mmlWebSocketClient.cpp

	
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/core/include \
                    jni/mml/process/include
					
LOCAL_CFLAGS    := -DMML_ANDROID
LOCAL_ARM_MODE   := arm 
LOCAL_MODULE    := mml_process
LOCAL_SRC_FILES := \
	mml/process/source/mml_process.cpp \
    mml/process/source/mmlProcess.cpp

	
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/core/include \
                    jni/mml/regexp/include \
					jni/mml/regexp/pcre/
					
LOCAL_CFLAGS    := -DMML_ANDROID -DHAVE_CONFIG_H
LOCAL_ARM_MODE   := arm 
LOCAL_MODULE    := mml_regexp
LOCAL_SRC_FILES := \
	mml/regexp/pcre/pcre_compile.c \
	mml/regexp/pcre/pcre_config.c \
	mml/regexp/pcre/pcre_dfa_exec.c \
	mml/regexp/pcre/pcre_exec.c \
	mml/regexp/pcre/pcre_fullinfo.c \
	mml/regexp/pcre/pcre_get.c \
	mml/regexp/pcre/pcre_globals.c \
	mml/regexp/pcre/pcre_info.c \
	mml/regexp/pcre/pcre_newline.c \
	mml/regexp/pcre/pcre_maketables.c \
	mml/regexp/pcre/pcre_ord2utf8.c \
	mml/regexp/pcre/pcre_refcount.c \
	mml/regexp/pcre/pcre_study.c \
	mml/regexp/pcre/pcre_tables.c \
	mml/regexp/pcre/pcre_try_flipped.c \
	mml/regexp/pcre/pcre_ucd.c \
	mml/regexp/pcre/pcre_valid_utf8.c \
	mml/regexp/pcre/pcre_version.c \
	mml/regexp/pcre/pcre_xclass.c \
	mml/regexp/pcre/pcrecpp.cc \
    mml/regexp/pcre/pcre_scanner.cc \
    mml/regexp/pcre/pcre_stringpiece.cc \
	mml/regexp/pcre/pcre_chartables.c \
	mml/regexp/source/mml_regexp.cpp \
	mml/regexp/source/mmlRegexp.cpp

	
include $(BUILD_STATIC_LIBRARY)

############################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := jni/mml/thirdparty/sqlite3
					
LOCAL_CFLAGS    := -DMML_ANDROID -DHAVE_CONFIG_H
LOCAL_ARM_MODE   := arm 
LOCAL_MODULE    := sqlite3
LOCAL_SRC_FILES := \
	mml/thirdparty/sqlite3/sqlite3.c

	
include $(BUILD_STATIC_LIBRARY)

############################################
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := jni/mml/core/include \
                    jni/mml/serialization/include \
   				    jni/mml/serialization/json \
					jni/mml/http/include \
					jni/mml/regexp/include \
					jni/mml/network/include \
					jni/mml/websocket/include \
					jni/mml/md5/include \
					jni/mml/process/include \
					jni/mml/scheduler/include \
					jni/mml/database/include
					
GIT_VERSION := $(shell git log --pretty=format:%h -n 1 HEAD)
LOCAL_CFLAGS    := -DMML_ANDROID -DGIT_VERSION=\"$(GIT_VERSION)\"
LOCAL_STATIC_LIBRARIES := mml_regexp \
                          mml_binary \
						  mml_process \
						  mml_crc \
						  mml_websocket \
						  mml_md5 \
						  mml_sha1 \
						  mml_base64  \
                          mml_json \
						  mml_network \
						  mml_http \
						  mml_scheduler \
						  mml_core \
						  mml_database \
						  mbedtls \
						  mbedx509 \
						  mbedcrypto \
						  sqlite3
						  
LOCAL_ARM_MODE   := arm  
LOCAL_LDLIBS    := -llog

LOCAL_MODULE    := android_updater_module

LOCAL_SRC_FILES := \
	updater/android_cmd_dispatcher.cpp \
	updater/android_cmd_logs.cpp \
	updater/android_cmd_shell.cpp \
	updater/android_cmd_update.cpp \
	updater/android_updater_module.cpp \
	updater/android_config.cpp \
	updater/android_http_fetch.cpp \
	updater/android_properties.cpp \
	updater/android_utils.cpp \
	updater/android_updater_subsystem.cpp \
	updater/android_updater_database.cpp \
	updater/android_http_update.cpp \
	updater/android_updater_task_tracking.cpp \
	updater/android_updater_task_reboot.cpp \
	updater/android_updater_task_cleanup.cpp \
	updater/android_updater_task_check.cpp \
	updater/android_settings.cpp
	
include $(BUILD_SHARED_LIBRARY)


############################################
include $(CLEAR_VARS)
						  
LOCAL_ARM_MODE   := arm  
LOCAL_LDLIBS    := -llog

LOCAL_MODULE    := android_updater

LOCAL_SRC_FILES := \
	runner/android_updater.c
include $(BUILD_EXECUTABLE)

