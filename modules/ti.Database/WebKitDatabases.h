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

#ifndef WebKitDatabases_h
#define WebKitDatabases_h

#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>

namespace Titanium {

/**
 * Databases is a convenience class for handling WebKit compatible databases
 */
class WebKitDatabases {
public:
	WebKitDatabases();
	virtual ~WebKitDatabases();

	/**
	 * returns true if database for origin exists, false if not found
	 */
	bool Exists(std::string name);

	/**
	 * return the path to an existing database with origin and name
	 */
	std::string Path(std::string name);

	/**
	 * create a new database with name for origin
	 */
	std::string Create(std::string name);

	/**
	 * delete an existing database for name and origin
	 */
	void Delete(std::string name);

private:
	std::string origin;
	std::string originPath;
	Poco::Data::Session* session;
};

} // namespace Titanium

#endif
