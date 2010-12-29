/*
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
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

#include "Host.h"

#include "Poco/Net/DNS.h"
#include "Poco/Net/NetException.h"

#include "IPAddress.h"

using namespace Poco::Net;

namespace Titanium {

Host::Host(const Poco::Net::IPAddress& address)
    : StaticBoundObject("Network.Host")
    , name(address.toString())
{
    this->Init();
    try
    {
        this->host = DNS::hostByAddress(address);
    }
    catch (HostNotFoundException&)
    {
        this->invalid = true;
        //TODO: improve this exception so we can properly raise
    }
    catch (NoAddressFoundException&)
    {
        this->invalid = true;
        //TODO: improve this exception so we can properly raise
    }
}

Host::Host(const std::string& name)
    : StaticBoundObject("Host")
    , name(name)
{
    this->Init();
    try
    {
        this->host = DNS::hostByName(name.c_str());
    }
    catch (HostNotFoundException&)
    {
        this->invalid = true;
        //TODO: improve this exception so we can properly raise
    }
    catch (NoAddressFoundException&)
    {
        this->invalid = true;
        //TODO: improve this exception so we can properly raise
    }
}

Host::~Host()
{
    KR_DUMP_LOCATION
}

void Host::Init() 
{
    this->invalid = false;
    /**
     * @tiapi(method=True,name=Network.Host.toString,since=0.2) Returns a string representation of a Host object
     * @tiresult(for=Network.Host.toString,type=String) the string representation of the Host object
     */
    this->SetMethod("toString",&Host::ToString);
    /**
     * @tiapi(method=True,name=Network.Host.isInvalid,since=0.2) Checks whether the Host object is invalid
     * @tiresult(for=Network.Host.isInvalid,type=Boolean) true if the Host object is invalid, false if otherwise
     */
    this->SetMethod("isInvalid",&Host::IsInvalid);
    /**
     * @tiapi(method=True,name=Network.Host.getName,since=0.2) Return the hostname of a Host object
     * @tiresult(for=Network.Host.getName,type=String) the hostname of the Host object
     */
    this->SetMethod("getName",&Host::GetName);
    /**
     * @tiapi(method=True,name=Network.Host.getAliases,since=0.2) Returns the list of aliases for a Host object
     * @tiresult(for=Network.Host.getAliases,type=Array<String>) a list of aliases for the Host object
     */
    this->SetMethod("getAliases",&Host::GetAliases);
    /**
     * @tiapi(method=True,name=Network.Host.getAddresses,since=0.2) Returns the list of address for a Host object
     * @tiresult(for=Network.Host.getAddresses,type=Array<Network.IPAddress>) a list of aliases for the Host object
     */
    this->SetMethod("getAddresses",&Host::GetAddresses);
}

void Host::ToString(const ValueList& args, KValueRef result)
{
    std::string s("[IPAddress ");
    s+=name;
    s+="]";
    result->SetString(s.c_str());
}

void Host::IsInvalid(const ValueList& args, KValueRef result)
{
    result->SetBool(this->invalid);
}

void Host::GetName(const ValueList& args, KValueRef result)
{
    result->SetString(this->host.name().c_str());
}

void Host::GetAliases(const ValueList& args, KValueRef result)
{
    KListRef list = new StaticBoundList();
    std::vector<std::string> aliases = this->host.aliases();
    std::vector<std::string>::iterator iter = aliases.begin();
    while (iter!=aliases.end())
    {
        std::string alias = (*iter++);
        list->Append(Value::NewString(alias));
    }
    result->SetList(list);
}

void Host::GetAddresses(const ValueList& args, KValueRef result)
{
    KListRef list = new StaticBoundList();
    std::vector<Poco::Net::IPAddress> addresses = this->host.addresses();
    std::vector<Poco::Net::IPAddress>::iterator iter = addresses.begin();
    while (iter!=addresses.end())
    {
        Poco::Net::IPAddress address = (*iter++);
        KObjectRef obj = new IPAddress(address);
        KValueRef addr = Value::NewObject(obj);
        list->Append(addr);
    }
    result->SetList(list);
}

} // namespace Titanium
