/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <sstream>

#include "app_config.h"
#include "window_config.h"
#include "config_utils.h"

static size_t windowCount = 0;

namespace ti
{
static bool CoerceBool(KObjectRef props, const char* name, bool defaultValue)
{
	KValueRef v(props->Get(name));
	if (v->IsString())
	{
		std::string value(v->ToString());
		if (value=="yes" || value=="1" || value=="true" || value=="True")
			return true;
		else
			return false;
	}
	else if (v->IsInt())
		return v->ToInt();

	else if (v->IsBool())
		return v->ToBool();

	return defaultValue;
}

static void EnforceTransparentBackgroundSettings(WindowConfig* config)
{
	if (!config->HasTransparentBackground())
		return;

	config->SetCloseable(false);
	config->SetResizable(false);
	config->SetMinimizable(false);
	config->SetMaximizable(false);
	config->SetUsingChrome(false);
}

static void EnforceMaxMinConstraints(WindowConfig* config)
{
	if (config->GetMinWidth() <= 0)
	{
		config->SetMinWidth(-1);
	}
	else if (config->GetWidth() < config->GetMinWidth())
	{
		config->SetWidth(config->GetMinWidth());
	}

	if (config->GetMaxWidth() <= 0)
	{
		config->SetMaxWidth(-1);
	}
	else if (config->GetWidth() > config->GetMaxWidth())
	{
		config->SetWidth(config->GetMaxWidth());
	}

	if (config->GetMinHeight() <= 0)
	{
		config->SetMinHeight(-1);
	}
	else if (config->GetHeight() < config->GetMinHeight())
	{
		config->SetHeight(config->GetMinHeight());
	}

	if (config->GetMaxHeight() <= 0)
	{
		config->SetMaxHeight(-1);
	}
	else if (config->GetHeight() > config->GetMaxHeight())
	{
		config->SetHeight(config->GetMaxHeight());
	}
}

WindowConfig::WindowConfig()
{
	std::ostringstream winid;
	winid << "win_" << ++windowCount;
	this->winid = winid.str();

	this->maximizable = true;
	this->minimizable = true;
	this->closeable = true;
	this->resizable = true;

	this->usingChrome = true;
	this->toolWindow = false;
	this->usingScrollbars = true;
	this->fullscreen = false;
	this->maximized = false;
	this->minimized = false;
	this->visible = true;
	this->topMost = false;

#ifdef OS_OSX
	this->texturedBackground = true;
#endif

	this->transparency = 1.0;
	this->transparentBackground = false;
	this->width = 800;
	this->height = 600;
	this->x = DEFAULT_POSITION;
	this->y = DEFAULT_POSITION;

	// -1 in this case signifies no constraints
	this->minWidth = -1;
	this->minHeight = -1;
	this->maxWidth = -1;
	this->maxHeight = -1;

	this->url = URLUtils::BlankPageURL();
	this->title = Host::GetInstance()->GetApplication()->name;
}

/*static*/
AutoPtr<WindowConfig> WindowConfig::FromProperties(KObjectRef properties)
{
	WindowConfig* c = new WindowConfig();
	c->SetID(properties->GetString("id", c->GetID()));
	c->SetURL(properties->GetString("url", c->GetURL()));
	c->SetURLRegex(properties->GetString("urlRegex", c->GetURLRegex()));
	c->SetTitle(properties->GetString("title", c->GetTitle()));
	c->SetX(properties->GetInt("x", c->GetX()));
	c->SetY(properties->GetInt("y", c->GetY()));
	c->SetWidth(properties->GetInt("width", c->GetWidth()));
	c->SetMinWidth(properties->GetInt("minWidth", c->GetMinWidth()));
	c->SetMaxWidth(properties->GetInt("maxWidth", c->GetMaxWidth()));
	c->SetHeight(properties->GetInt("height", c->GetHeight()));
	c->SetMinHeight(properties->GetInt("minHeight", c->GetMinHeight()));
	c->SetMaxHeight(properties->GetInt("maxHeight", c->GetMaxHeight()));
	c->SetMaximizable(CoerceBool(properties, "maximizable", c->IsMaximizable()));
	c->SetMinimizable(CoerceBool(properties, "minimizable", c->IsMinimizable()));
	c->SetCloseable(CoerceBool(properties, "closeable", c->IsCloseable()));
	c->SetResizable(CoerceBool(properties, "resizable", c->IsResizable()));
	c->SetFullscreen(CoerceBool(properties, "fullscreen", c->IsFullscreen()));
	c->SetMaximized(CoerceBool(properties, "maximized", c->IsMaximized()));
	c->SetMinimized(CoerceBool(properties, "minimized", c->IsMinimized()));
	c->SetUsingChrome(CoerceBool(properties, "usingChrome", c->IsUsingChrome()));
	c->SetToolWindow(CoerceBool(properties, "toolWindow", c->IsToolWindow()));
	c->SetTopMost(CoerceBool(properties, "topMost", c->IsTopMost()));
	c->SetVisible(CoerceBool(properties, "visible", c->IsVisible()));
	c->SetTransparentBackground(CoerceBool(properties,
		"transparentBackground", c->HasTransparentBackground()));
	c->SetTransparency(properties->GetDouble("transparency", c->GetTransparency()));

#ifdef OS_OSX
	c->SetTexturedBackground(properties->GetDouble("texturedBackground", c->HasTexturedBackground()));
#endif

	EnforceMaxMinConstraints(c);
	EnforceTransparentBackgroundSettings(c);
	return c;
}

/*static*/
AutoPtr<WindowConfig> WindowConfig::FromWindowConfig(AutoPtr<WindowConfig> config)
{
	WindowConfig* newConfig = new WindowConfig();

	// Just use defaults given a NULL instance.
	if (config.isNull())
		return newConfig;

	newConfig->SetURL(config->GetURL());
	newConfig->SetTitle(config->GetTitle());
	newConfig->SetX(config->GetX());
	newConfig->SetY(config->GetY());
	newConfig->SetWidth(config->GetWidth());
	newConfig->SetMinWidth(config->GetMinWidth());
	newConfig->SetMaxWidth(config->GetMaxWidth());
	newConfig->SetHeight(config->GetHeight());
	newConfig->SetMinHeight(config->GetMinHeight());
	newConfig->SetMaxHeight(config->GetMaxHeight());
	newConfig->SetVisible(config->IsVisible());
	newConfig->SetMaximizable(config->IsMaximizable());
	newConfig->SetMinimizable(config->IsMinimizable());
	newConfig->SetResizable(config->IsResizable());
	newConfig->SetFullscreen(config->IsFullscreen());
	newConfig->SetMaximized(config->IsMaximized());
	newConfig->SetMinimized(config->IsMinimized());
	newConfig->SetUsingChrome(config->IsUsingChrome());
	newConfig->SetUsingScrollbars(config->IsUsingScrollbars());
	newConfig->SetTopMost(config->IsTopMost());
	newConfig->SetTransparency(config->GetTransparency());
	newConfig->SetTransparentBackground(config->HasTransparentBackground());
#ifdef OS_OSX
	newConfig->SetTexturedBackground(config->HasTexturedBackground());
#endif

	return newConfig;
}

/*static*/
AutoPtr<WindowConfig> WindowConfig::FromXMLNode(xmlNodePtr element)
{
	WindowConfig* config = new WindowConfig();

	xmlNodePtr child = element->children;
	while (child)
	{
		if (child->type != XML_ELEMENT_NODE)
		{
			child = child->next;
			continue;
		}

		// This should always be a UTF-8, so we can just cast
		// the node name here to a char*
		std::string nodeName(reinterpret_cast<char*>(
			const_cast<xmlChar*>(child->name)));

		if (nodeName == "id")
		{
			config->SetID(ConfigUtils::GetNodeValue(child));
		}
		else if (nodeName == "title")
		{
			config->SetTitle(ConfigUtils::GetNodeValue(child));
		}
		else if (nodeName == "url")
		{
			config->SetURL(ConfigUtils::GetNodeValue(child));
		}
		else if (nodeName == "url-regex")
		{
			config->SetURLRegex(ConfigUtils::GetNodeValue(child));
		}
		else if (nodeName == "maximizable")
		{
			config->SetMaximizable(ConfigUtils::GetNodeValueAsBool(child));
		}
		else if (nodeName == "minimizable")
		{
			config->SetMinimizable(ConfigUtils::GetNodeValueAsBool(child));
		}
		else if (nodeName == "closeable")
		{
			config->SetCloseable(ConfigUtils::GetNodeValueAsBool(child));
		}
		else if (nodeName == "resizable")
		{
			config->SetResizable(ConfigUtils::GetNodeValueAsBool(child));
		}
		else if (nodeName == "fullscreen")
		{
			config->SetFullscreen(ConfigUtils::GetNodeValueAsBool(child));
		}
		else if (nodeName == "maximized")
		{
			config->SetMaximized(ConfigUtils::GetNodeValueAsBool(child));
		}
		else if (nodeName == "minimized")
		{
			config->SetMinimized(ConfigUtils::GetNodeValueAsBool(child));
		}
		else if (nodeName == "chrome")
		{
			config->SetUsingChrome(ConfigUtils::GetNodeValueAsBool(child));
			std::string scrollbars(ConfigUtils::GetPropertyValue(child, "scrollbars"));
			if (!scrollbars.empty())
			{
				config->SetUsingScrollbars(ConfigUtils::StringToBool(scrollbars));
			}
		}
		else if (nodeName == "tool-window")
		{
			config->SetToolWindow(ConfigUtils::GetNodeValueAsBool(child));
		}
		else if (nodeName == "transparency")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			config->SetTransparency((float) atof(value.c_str()));
		}
		else if (nodeName == "transparent-background")
		{
			config->SetTransparentBackground(
				ConfigUtils::GetNodeValueAsBool(child));
		}
		else if (nodeName == "x")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			config->SetX(atoi(value.c_str()));
		}
		else if (nodeName == "y")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			config->SetY(atoi(value.c_str()));
		}
		else if (nodeName == "width")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			config->SetWidth(atoi(value.c_str()));
		}
		else if (nodeName == "height")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			config->SetHeight(atoi(value.c_str()));
		}
		else if (nodeName == "visible")
		{
			config->SetVisible(ConfigUtils::GetNodeValueAsBool(child));
		}
		else if (nodeName == "min-width")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			config->SetMinWidth(atoi(value.c_str()));
		}
		else if (nodeName == "max-width")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			config->SetMaxWidth(atoi(value.c_str()));
		}
		else if (nodeName == "min-height")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			config->SetMinHeight(atoi(value.c_str()));
		}
		else if (nodeName == "max-height")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			config->SetMaxHeight(atoi(value.c_str()));
		}
		else if (nodeName == "top-most")
		{
			config->SetTopMost(ConfigUtils::GetNodeValueAsBool(child));
		}
#ifdef OS_OSX
		else if (nodeName == "texturedBackground")
		{
			config->SetTexturedBackground(ConfigUtils::GetNodeValueAsBool(child));
		}
#endif
		child = child->next;
	}

	EnforceMaxMinConstraints(config);
	EnforceTransparentBackgroundSettings(config);
	return config;
}

} // namespace ti
