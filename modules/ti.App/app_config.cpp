/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "app_config.h"
#include "window_config.h"
#include "config_utils.h"
#include "properties_binding.h"

#include "Poco/RegularExpression.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

using Poco::Util::PropertyFileConfiguration;

namespace ti
{

static void ParsePropertyNode(xmlNodePtr node, 
	AutoPtr<PropertyFileConfiguration> config)
{
	std::string name(ConfigUtils::GetPropertyValue(node, "name"));

	if (name.empty())
		return;

	std::string type(ConfigUtils::GetPropertyValue(node, "type"));
	std::string value(ConfigUtils::GetNodeValue(node));
	if (type == "int")
	{
		config->setInt(name, atoi(value.c_str()));
	}
	else if (type == "bool")
	{
		config->setBool(name, ConfigUtils::StringToBool(value));
	}
	else if (type == "double")
	{
		config->setDouble(name, atof(value.c_str()));
	}
	else
	{
		config->setString(name, value);
	}
}

static Logger* GetLogger()
{
	static Logger* logger = Logger::Get("App.AppConfig");
	return logger;
}

AppConfig* AppConfig::Instance()
{
	static AppConfig* instance = 0;
	if (!instance)
	{
		std::string configFilename(FileUtils::Join(
			Host::GetInstance()->GetApplication()->path.c_str(),
			CONFIG_FILENAME, 0));
		GetLogger()->Debug("Loading config file: %s", configFilename.c_str());
		if (!FileUtils::IsFile(configFilename))
		{
			GetLogger()->Critical("Cannot load config file: %s",
				configFilename.c_str());
			throw ValueException::FromFormat("Cannot load config file: %s",
				configFilename.c_str());
		}

		instance = new AppConfig(configFilename);
	}

	return instance;
}

AppConfig::AppConfig(std::string& xmlfile)
	: analyticsEnabled(true)
{
	systemProperties = new PropertiesBinding();
	xmlParserCtxtPtr context = xmlNewParserCtxt();
	xmlDocPtr document = xmlCtxtReadFile(context, xmlfile.c_str(), NULL, 0);

	if (!document)
	{
		std::ostringstream error;
		if (context->lastError.code != XML_IO_LOAD_ERROR)
		{
			error << context->lastError.file << "[Line ";
			error << context->lastError.line << "] ";
		}
		error << context->lastError.message;
		GetLogger()->Error(error.str());

		xmlFreeParserCtxt(context);
		xmlCleanupParser();
		return;
	}

	xmlNodePtr root = xmlDocGetRootElement(document);
	xmlNodePtr node = root->children;
	while (node)
	{
		if (node->type != XML_ELEMENT_NODE)
		{
			node = node->next;
			continue;
		}

		// This should always be a UTF-8, so we can just cast
		// the node name here to a char*
		std::string nodeName(reinterpret_cast<char*>(
			const_cast<xmlChar*>(node->name)));

		if (nodeName == "name")
		{
			appName = ConfigUtils::GetNodeValue(node);
		}
		else if (nodeName == "id")
		{
			appID = ConfigUtils::GetNodeValue(node);
		}
		else if (nodeName == "description")
		{
			description = ConfigUtils::GetNodeValue(node);
		}
		else if (nodeName == "copyright")
		{
			copyright = ConfigUtils::GetNodeValue(node);
		}
		else if (nodeName == "url")
		{
			url = ConfigUtils::GetNodeValue(node);
		}
		else if (nodeName == "version")
		{
			version = ConfigUtils::GetNodeValue(node);
		}
		else if (nodeName == "publisher")
		{
			publisher = ConfigUtils::GetNodeValue(node);
		}
		else if (nodeName == "window")
		{
			this->windows.push_back(new WindowConfig((void *) node));
		}
		else if (nodeName == "analytics")
		{
			std::string nodeValue(ConfigUtils::GetNodeValue(node));
			analyticsEnabled = ConfigUtils::StringToBool(nodeValue);
		}
		else if (nodeName == "icon")
		{
			icon = ConfigUtils::GetNodeValue(node);
		}
		else if (nodeName == "property")
		{
			ParsePropertyNode(node, systemProperties->GetConfig());
		}

		node = node->next;
	}

	xmlFreeDoc(document);
	xmlFreeParserCtxt(context);
	xmlCleanupParser();
}

AppConfig::~AppConfig()
{
}

WindowConfig* AppConfig::GetWindow(std::string& id)
{
	for (size_t i = 0; i < windows.size(); i++)
	{
		if (windows[i]->GetID() == id)
		{
			return windows[i];
		}
	}
	return NULL;
}

WindowConfig* AppConfig::GetWindowByURL(std::string url)
{
	for (size_t i = 0; i < windows.size(); i++)
	{
		std::string urlRegex(windows[i]->GetURLRegex());

		Poco::RegularExpression::Match match;
		Poco::RegularExpression regex(urlRegex);

		regex.match(url, match);

		if (match.length != 0)
		{
			return windows[i];
		}
	}
	return NULL;
}

WindowConfig* AppConfig::GetMainWindow()
{
	if (windows.size() > 0)
		return windows[0];
	else
		return NULL;
}

} // namespace ti
