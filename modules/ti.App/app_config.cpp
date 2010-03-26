/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "app_config.h"
#include "window_config.h"
#include "config_utils.h"
#include "properties_binding.h"

#include <Poco/RegularExpression.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <sstream>

using Poco::Util::TitaniumPropertyFileConfiguration;

namespace ti
{

static void ParsePropertyNode(xmlNodePtr node,
	AutoPtr<TitaniumPropertyFileConfiguration> config)
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
			this->windows.push_back(WindowConfig::FromXMLNode(node));
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

AutoPtr<WindowConfig> AppConfig::GetWindowByURL(const std::string& url)
{
	AutoPtr<WindowConfig> config(0);

	// First try matching the URL exactly.
	for (size_t i = 0; i < windows.size(); i++)
	{
		if (windows[i]->GetURL() == url)
		{
			config = windows[i];
			break;
		}
	}

	// If we didn't find a matching window URL, try matching
	// against the url-regex parameter of the windows configs.
	if (config.isNull())
	{
		for (size_t i = 0; i < windows.size(); i++)
		{
			if (windows[i]->GetURLRegex().empty())
				continue;

			std::string urlRegex(windows[i]->GetURLRegex());
			Poco::RegularExpression::Match match;
			Poco::RegularExpression regex(windows[i]->GetURLRegex());
			regex.match(url, match);
			if (match.length != 0)
			{
				config = windows[i];
				break;
			}
		}
	}

	// Return NULL here, because callers need to know whether
	// or not a configuration was matched.
	if (config.isNull())
		return config;

	// Return a copy here, so that the original configuration
	// is preserved when this is mutated
	config = WindowConfig::FromWindowConfig(config);
	config->SetURL(url);
	return config;
}

AutoPtr<WindowConfig> AppConfig::GetMainWindow()
{
	// WindowConfig::FromWindowConfig should just return
	// the default configuration given a NULL config.
	if (windows.empty())
		return WindowConfig::FromWindowConfig(0);

	// Return a copy here, so that the original configuration
	// is preserved when this is mutated
	return WindowConfig::FromWindowConfig(windows[0]);
}

} // namespace ti
