/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _CODEC_BINDING_H_
#define _CODEC_BINDING_H_

#include <kroll/kroll.h>

namespace ti
{
	class CodecBinding : public StaticBoundObject
	{
	public:
		CodecBinding(SharedKObject);
	protected:
		virtual ~CodecBinding();
	private:
		SharedKObject global;
		
		void Base64Encode(const ValueList& args, SharedValue result);
		void Base64Decode(const ValueList& args, SharedValue result);
		void DigestToHex(const ValueList& args, SharedValue result);
		void DigestHMACToHex(const ValueList& args, SharedValue result);
		void EncodeHexBinary(const ValueList& args, SharedValue result);
		void DecodeHexBinary(const ValueList& args, SharedValue result);
		void Checksum(const ValueList& args, SharedValue result);
	};
}

#endif
