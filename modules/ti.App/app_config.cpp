/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "app_config.h"
#include "window_config.h"
#include "config_utils.h"
#include "Poco/RegularExpression.h"
#include "Properties/properties_binding.h"

using namespace ti;
AppConfig *AppConfig::instance_ = NULL;

AppConfig::AppConfig(std::string& xmlfile)
{
	systemProperties = new PropertiesBinding();
	instance_ = this;
	error = NULL;
	xmlParserCtxtPtr context = xmlNewParserCtxt();

	xmlDocPtr document = xmlCtxtReadFile(context, xmlfile.c_str(), NULL, 0);
	if (document != NULL)
	{
		xmlNodePtr root = xmlDocGetRootElement(document);
		xmlNodePtr node = root->children;
		while (node != NULL)
		{
			if (node->type == XML_ELEMENT_NODE)
			{
				if (nodeNameEquals(node, "name"))
				{
					appName = ConfigUtils::GetNodeValue(node);
				}
				else if (nodeNameEquals(node, "id"))
				{
					appID = ConfigUtils::GetNodeValue(node);
				}
				else if (nodeNameEquals(node, "description"))
				{
					description = ConfigUtils::GetNodeValue(node);
				}
				else if (nodeNameEquals(node, "copyright"))	
				{
					copyright = ConfigUtils::GetNodeValue(node);
				}
				else if (nodeNameEquals(node, "url"))
				{
					url = ConfigUtils::GetNodeValue(node);
				}
				else if (nodeNameEquals(node, "version"))
				{
					version = ConfigUtils::GetNodeValue(node);
				}
				else if (nodeNameEquals(node, "publisher"))
				{
					publisher = ConfigUtils::GetNodeValue(node);
				}
				else if (nodeNameEquals(node, "window"))
				{
					this->windows.push_back(new WindowConfig((void *) node));
				}
				else if (nodeNameEquals(node, "icon"))
				{
					xmlNodePtr child = node->children;
					while (child != NULL)
					{
						if (child->type == XML_ELEMENT_NODE)
						{
							if (nodeNameEquals(child, "image16"))
							{
								icon16 = ConfigUtils::GetNodeValue(child);
							}
							else if (nodeNameEquals(child, "image32"))
							{
								icon32 = ConfigUtils::GetNodeValue(child);
							}
							else if (nodeNameEquals(child, "image48"))
							{
								icon48 = ConfigUtils::GetNodeValue(child);
							}
						}
						child = child->next;
					}
				}
				else if (nodeNameEquals(node, "property"))
				{
					std::string name = ConfigUtils::GetPropertyValue(node, "name");
					if (name.size() > 0) {
						std::string type = ConfigUtils::GetPropertyValue(node, "type");
						std::string value = ConfigUtils::GetNodeValue(node);
					
						PRINTD("system property " << name << " = " << value);
						
						if (type == "int") {
							systemProperties->GetConfig()->setInt(name, atoi(value.c_str()));
						}
						else if (type == "bool") {
							systemProperties->GetConfig()->setBool(name, ConfigUtils::StringToBool(value));
						}
						else if (type == "double")
						{
							systemProperties->GetConfig()->setDouble(name, atof(value.c_str()));
						}
						else {
							systemProperties->GetConfig()->setString(name, value);
						}
					}
				}
			}
			node = node->next;
		}

		xmlFreeDoc(document);
	}

	if (document == NULL)
	{
		std::string _error;

		if (context->lastError.code != XML_IO_LOAD_ERROR)
		{
			_error += context->lastError.file;
			_error += " [Line ";

			std::ostringstream o;
			o << context->lastError.line;

			_error += o.str();
			_error += "]";
			_error += " ";
		}
		_error += context->lastError.message;

		error = strdup(_error.c_str());
	}

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

		if(match.length != 0)
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
