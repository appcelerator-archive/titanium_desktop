/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _CODEC_BINDING_H_
#define _CODEC_BINDING_H_

#include <kroll/kroll.h>

#include <Poco/Zip/Zip.h>
#include <Poco/Zip/Compress.h>
#include <Poco/Path.h>

namespace ti
{
	class CodecBinding : public StaticBoundObject
	{
	public:
		CodecBinding(KObjectRef);
	protected:
		virtual ~CodecBinding();
	private:
		KObjectRef global;

		void EncodeBase64(const ValueList& args, KValueRef result);
		void DecodeBase64(const ValueList& args, KValueRef result);
		void DigestToHex(const ValueList& args, KValueRef result);
		void DigestHMACToHex(const ValueList& args, KValueRef result);
		void EncodeHexBinary(const ValueList& args, KValueRef result);
		void DecodeHexBinary(const ValueList& args, KValueRef result);
		void Checksum(const ValueList& args, KValueRef result);
		void CreateZip(const ValueList& args, KValueRef result);
		
		static KValueRef CreateZipAsync(const ValueList& args);
	};
}

#endif
