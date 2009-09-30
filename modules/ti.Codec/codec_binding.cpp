/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */	
#include <kroll/kroll.h>
#include "codec_binding.h"

#include <sstream>
#include <Poco/Base64Encoder.h>
#include <Poco/Base64Decoder.h>
#include <Poco/DigestEngine.h>
#include <Poco/MD2Engine.h>
#include <Poco/MD4Engine.h>
#include <Poco/MD5Engine.h>
#include <Poco/SHA1Engine.h>
#include <Poco/HMACEngine.h>
#include <Poco/HexBinaryEncoder.h>
#include <Poco/HexBinaryDecoder.h>
#include <Poco/Checksum.h>

#define CODEC_MD2 		1
#define CODEC_MD4	 		2
#define CODEC_MD5 		3
#define CODEC_SHA1 		4
#define CODEC_CRC32		1
#define CODEC_ADLER32	2

namespace ti
{
	CodecBinding::CodecBinding(SharedKObject global) : global(global)
	{
		/**
		 * @tiapi(method=True,name=Codec.encodeBase64,since=0.7) encode a string into base64
		 * @tiarg(for=Codec.encodeBase64,name=data,type=String) data to encode
		 * @tiresult(for=Codec.encodeBase64,type=string) returns base64 encoded string
		 */
		this->SetMethod("encodeBase64",&CodecBinding::Base64Encode);

		/**
		 * @tiapi(method=True,name=Codec.decodeBase64,since=0.7) decode a string from base64
		 * @tiarg(for=Codec.decodeBase64,name=data,type=String) data to decode
		 * @tiresult(for=Codec.decodeBase64,type=string) returns base64 decoded string
		 */
		this->SetMethod("decodeBase64",&CodecBinding::Base64Decode);

		/**
		 * @tiapi(method=True,name=Codec.digestToHex,since=0.7) encode a string using a digest algorithm
		 * @tiarg(for=Codec.digestToHex,name=type,type=int) encoding type: currently supports MD2, MD4, MD5, SHA1
		 * @tiarg(for=Codec.digestToHex,name=data,type=String) data to encode
		 * @tiresult(for=Codec.digestToHex,type=string) returns encoded string
		 */
		this->SetMethod("digestToHex",&CodecBinding::DigestToHex);

		/**
		 * @tiapi(method=True,name=Codec.digestHMACToHex,since=0.7) digest a encoded string in HMAC
		 * @tiarg(for=Codec.digestHMACToHex,name=type,type=int) encoding type: currently supports MD2, MD4, MD5, SHA1
		 * @tiarg(for=Codec.digestHMACToHex,name=data,type=String) data to encode
		 * @tiarg(for=Codec.digestHMACToHex,name=data,type=String) key to us for HMAC
		 * @tiresult(for=Codec.digestHMACToHex,type=string) returns base64 decoded string
		 */
		this->SetMethod("digestHMACToHex",&CodecBinding::DigestHMACToHex);

		/**
		 * @tiapi(method=True,name=Codec.encodeHexBinary,since=0.7) encode a string into hex binary
		 * @tiarg(for=Codec.encodeHexBinary,name=data,type=String) data to encode
		 * @tiresult(for=Codec.encodeHexBinary,type=string) returns hex binary encoded string
		 */
		this->SetMethod("encodeHexBinary",&CodecBinding::EncodeHexBinary);

		/**
		 * @tiapi(method=True,name=Codec.decodeHexBinary,since=0.7) decode a string from hex binary
		 * @tiarg(for=Codec.decodeHexBinary,name=data,type=String) data to decode
		 * @tiresult(for=Codec.decodeHexBinary,type=string) returns unencoded hex binary string
		 */
		this->SetMethod("decodeHexBinary",&CodecBinding::DecodeHexBinary);

		/**
		 * @tiapi(method=True,name=Codec.checksum,since=0.7) compute checksum
		 * @tiarg(for=Codec.checksum,name=data,type=String) data to checksum (as string)
		 * @tiarg(for=Codec.checksum,name=type,type=int,optional=True) checksum type: currently supports CRC32 (default) and ADLER32
		 * @tiresult(for=Codec.checksum,type=int) return checksum value
		 */
		this->SetMethod("checksum",&CodecBinding::Checksum);
		
		/**
		 * @tiapi(property=True,name=Codec.MD2,since=0.7) MD2 property
		 */
		this->SetInt("MD2", CODEC_MD2);
		/**
		 * @tiapi(property=True,name=Codec.MD2,since=0.7) MD4 property
		 */
		this->SetInt("MD4", CODEC_MD4);
		/**
		 * @tiapi(property=True,name=Codec.MD5,since=0.7) MD5 property
		 */
		this->SetInt("MD5", CODEC_MD5);
		/**
		 * @tiapi(property=True,name=Codec.SHA1,since=0.7) SHA1 property
		 */
		this->SetInt("SHA1", CODEC_SHA1);
		/**
		 * @tiapi(property=True,name=Codec.CRC32,since=0.7) CRC32 property
		 */
		this->SetInt("CRC32", CODEC_CRC32);
		/**
		 * @tiapi(property=True,name=Codec.ADLER32,since=0.7) ADLER32 property
		 */
		this->SetInt("ADLER32", CODEC_ADLER32);
	}
	CodecBinding::~CodecBinding()
	{
	}
	void CodecBinding::Base64Encode(const ValueList& args, SharedValue result)
	{
		std::string unencoded = args.at(0)->ToString();
		std::ostringstream str;
		Poco::Base64Encoder encoder(str);
		encoder << unencoded;
		encoder.close();
		std::string encoded = str.str();
		result->SetString(encoded);
	}
	void CodecBinding::Base64Decode(const ValueList& args, SharedValue result)
	{
		std::string encoded = args.at(0)->ToString();
		std::stringstream str;
		str << encoded;
		Poco::Base64Decoder decoder(str);
		std::string decoded;
		decoder >> decoded;
		result->SetString(decoded);
	}
	void CodecBinding::DigestToHex(const ValueList& args, SharedValue result)
	{
		int type = args.at(0)->ToInt();
		std::string encoded = args.at(1)->ToString();
		
		Poco::DigestEngine *engine = NULL;
		
		switch(type)
		{
			case CODEC_MD2:
			{
				engine = new Poco::MD2Engine(); 
				break;
			}
			case CODEC_MD4:
			{
				engine = new Poco::MD4Engine(); 
				break;
			}
			case CODEC_MD5:
			{
				engine = new Poco::MD5Engine(); 
				break;
			}
			case CODEC_SHA1:
			{
				engine = new Poco::SHA1Engine(); 
				break;
			}
			default:
			{
				std::ostringstream msg("Unsupported encoding type: ");
				msg << type;
				throw ValueException::FromString(msg.str());
			}
		}

		engine->update(encoded); 
		std::string data = Poco::DigestEngine::digestToHex(engine->digest()); 
		result->SetString(data);
		delete engine;
	}
	
	void CodecBinding::DigestHMACToHex(const ValueList& args, SharedValue result)
	{
		int type = args.at(0)->ToInt();
		std::string encoded = args.at(1)->ToString();
		std::string key = args.at(2)->ToString();

		switch(type)
		{
			case CODEC_MD2:
			{
				Poco::HMACEngine<Poco::MD2Engine> engine(key); 
				engine.update(encoded); 
				std::string data = Poco::DigestEngine::digestToHex(engine.digest()); 
				result->SetString(data);
				break;
			}
			case CODEC_MD4:
			{
				Poco::HMACEngine<Poco::MD4Engine> engine(key); 
				engine.update(encoded); 
				std::string data = Poco::DigestEngine::digestToHex(engine.digest()); 
				result->SetString(data);
				break;
			}
			case CODEC_MD5:
			{
				Poco::HMACEngine<Poco::MD5Engine> engine(key); 
				engine.update(encoded); 
				std::string data = Poco::DigestEngine::digestToHex(engine.digest()); 
				result->SetString(data);
				break;
			}
			case CODEC_SHA1:
			{
				Poco::HMACEngine<Poco::SHA1Engine> engine(key); 
				engine.update(encoded); 
				std::string data = Poco::DigestEngine::digestToHex(engine.digest()); 
				result->SetString(data);
				break;
			}
			default:
			{
				std::ostringstream msg("Unsupported encoding type: ");
				msg << type;
				throw ValueException::FromString(msg.str());
			}
		}

	}
	void CodecBinding::EncodeHexBinary(const ValueList& args, SharedValue result)
	{
		std::string unencoded = args.at(0)->ToString();
		std::stringstream str;
		Poco::HexBinaryEncoder encoder(str);
		encoder << unencoded;
		encoder.close();
		std::string encoded = str.str();
		result->SetString(encoded);
	}
	void CodecBinding::DecodeHexBinary(const ValueList& args, SharedValue result)
	{
		std::string encoded = args.at(0)->ToString();
		std::stringstream str;
		str << encoded;
		Poco::HexBinaryDecoder decoder(str);
		std::string decoded;
		decoder >> decoded;
		result->SetString(decoded);
	}
	void CodecBinding::Checksum(const ValueList& args, SharedValue result)
	{
		int type = CODEC_CRC32;
		
		if (args.size() == 2)
		{
			type = args.at(1)->ToInt();
		}
		
		Poco::Checksum *checksum = NULL;
		
		switch(type)
		{
			case CODEC_CRC32:
			{
				checksum = new Poco::Checksum(Poco::Checksum::TYPE_CRC32);
				break;
			}
			case CODEC_ADLER32:
			{
				checksum = new Poco::Checksum(Poco::Checksum::TYPE_ADLER32);
				break;
			}
			default:
			{
				std::ostringstream msg("Unsupported type: ");
				msg << type;
				throw ValueException::FromString(msg.str());
			}
		}

		if (args.at(0)->IsString())
		{
			std::string encoded = args.at(0)->ToString();
			const char *data = encoded.c_str();
			checksum->update(data,encoded.size());
			result->SetInt(checksum->checksum());
		}
		else if (args.at(0)->IsObject())
		{
			AutoBlob blobData = args.at(0)->ToObject().cast<Blob>();
			if (blobData.isNull())
			{
				delete checksum;
				throw ValueException::FromString("unsupported data type passed as argument 1");
			}
			checksum->update(blobData->Get(),blobData->Length());
			result->SetInt(checksum->checksum());
		}
		else
		{
			delete checksum;
			throw ValueException::FromString("unsupported data type passed as argument 1");
		}
		
		delete checksum;
	}
}
