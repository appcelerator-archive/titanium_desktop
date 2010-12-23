/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef Database_h
#define Database_h

#include <kroll/kroll.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>

namespace Titanium {

class Database : public KAccessorObject {
public:
	Database(std::string& name, bool isWebKitDatabase);
	virtual ~Database();

protected:
	void Open(const ValueList& args, KValueRef result);
	void Execute(const ValueList& args, KValueRef result);
	void Close(const ValueList& args, KValueRef result);
	void Remove(const ValueList& args, KValueRef result);
	void GetPath(const ValueList& args, KValueRef result);
	void Close();

	Poco::Data::Session *session;
	std::string name;
	std::string path;
	bool isWebKitDatabase;
};

} // namespace Titanium

#endif
