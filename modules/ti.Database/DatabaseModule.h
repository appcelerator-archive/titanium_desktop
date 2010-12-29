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

#ifndef DatabaseModule_h
#define DatabaseModule_h

#include <kroll/kroll.h>

namespace Titanium {

/**
 * Database module which exposes a synchronous API for Database
 * access and interoperates with the WebKit Database seamlessly.
 *
 * You can interchangeably use the WebKit HTML 5 Database API or
 * this API on the same database.  Additionally, if you create 
 * databases in this API, they will be exposed in the Web
 * Inspector.
 *
 */
class EXPORT DatabaseModule : public kroll::Module, public StaticBoundObject {
    KROLL_MODULE_CLASS(DatabaseModule)

private:
    kroll::KObjectRef binding;
    void Open(const ValueList& args, KValueRef result);
    void OpenFile(const ValueList& args, KValueRef result);
};

} // namespace Titanium

#endif
