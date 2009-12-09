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

namespace ti
{
int WindowConfig::DEFAULT_POSITION = -404404404;
int WindowConfig::windowCount = 0;

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

void WindowConfig::SetDefaults ()
{
	WindowConfig::windowCount++;
	std::ostringstream winid;
	winid << "win_" << WindowConfig::windowCount;
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
	this->width = 800;
	this->height = 600;
	this->x = WindowConfig::DEFAULT_POSITION;
	this->y = WindowConfig::DEFAULT_POSITION;

	// -1 in this case signifies no constraints
	this->minWidth = -1;
	this->minHeight = -1;
	this->maxWidth = -1;
	this->maxHeight = -1;

	this->url = URLUtils::BlankPageURL();
	this->title = Host::GetInstance()->GetApplication()->name;
}

void WindowConfig::UseProperties(KObjectRef properties)
{
	this->SetDefaults();

	winid = properties->GetString("id", winid);
	url = properties->GetString("url", url);
	urlRegex = properties->GetString("urlRegex", urlRegex);
	title = properties->GetString("title", title);
	x = properties->GetInt("x", x);
	y = properties->GetInt("y", y);
	width = properties->GetInt("width", width);
	minWidth = properties->GetInt("minWidth", minWidth);
	maxWidth = properties->GetInt("maxWidth", maxWidth);
	height = properties->GetInt("height", height);
	minHeight = properties->GetInt("minHeight", minHeight);
	maxHeight = properties->GetInt("maxHeight", maxHeight);
	maximizable = CoerceBool(properties, "maximizable", maximizable);
	minimizable = CoerceBool(properties, "minimizable", minimizable);
	closeable = CoerceBool(properties, "closeable", closeable);
	resizable = CoerceBool(properties, "resizable", resizable);
	fullscreen = CoerceBool(properties, "fullscreen", fullscreen);
	maximized = CoerceBool(properties, "maximized", maximized);
	minimized = CoerceBool(properties, "minimized", minimized);
	usingChrome = CoerceBool(properties, "usingChrome", usingChrome);
	toolWindow = CoerceBool(properties, "toolWindow", toolWindow);
	topMost = CoerceBool(properties, "topMost", topMost);
	visible = CoerceBool(properties, "visible", visible);
	transparency = properties->GetDouble("transparency", transparency);

#ifdef OS_OSX
	texturedBackground = properties->GetDouble("texturedBackground",
		texturedBackground);
#endif
}

WindowConfig::WindowConfig(WindowConfig *config, std::string& url)
{
	this->SetDefaults();
	this->url = url;

	if (config == NULL) // Just use defaults if not found
		return;

	this->title = config->GetTitle();
	this->x = config->GetX();
	this->y = config->GetY();
	this->width = config->GetWidth();
	this->minWidth = config->GetMinWidth();
	this->maxWidth = config->GetMaxWidth();
	this->height = config->GetHeight();
	this->minHeight = config->GetMinHeight();
	this->maxHeight = config->GetMaxHeight();
	this->visible = config->IsVisible();
	this->maximizable = config->IsMaximizable();
	this->minimizable = config->IsMinimizable();
	this->resizable = config->IsResizable();
	this->fullscreen = config->IsFullscreen();
	this->maximized = config->IsMaximized();
	this->minimized = config->IsMinimized();
	this->usingChrome = config->IsUsingChrome();
	this->usingScrollbars = config->IsUsingScrollbars();
	this->topMost = config->IsTopMost();
	this->transparency = config->GetTransparency();

#ifdef OS_OSX
	this->texturedBackground = config->IsTexturedBackground();
#endif
}

WindowConfig::WindowConfig(void* data)
{
	xmlElementPtr element = (xmlElementPtr) data;
	SetDefaults();

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
			winid = ConfigUtils::GetNodeValue(child);
		}
		else if (nodeName == "title")
		{
			title = ConfigUtils::GetNodeValue(child);
		}
		else if (nodeName == "url")
		{
			url = ConfigUtils::GetNodeValue(child);
		}
		else if (nodeName == "url-regex")
		{
			urlRegex = ConfigUtils::GetNodeValue(child);
		}
		else if (nodeName == "maximizable")
		{
			maximizable = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeName == "minimizable")
		{
			minimizable = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeName == "closeable")
		{
			closeable = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeName == "resizable")
		{
			resizable = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeName == "fullscreen")
		{
			fullscreen = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeName == "maximized")
		{
			maximized = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeName == "minimized")
		{
			minimized = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeName == "chrome")
		{
			usingChrome = ConfigUtils::GetNodeValueAsBool(child);
			std::string scrollbars = ConfigUtils::GetPropertyValue(child, "scrollbars");
			if (!scrollbars.empty())
			{
				usingScrollbars = ConfigUtils::StringToBool(scrollbars);
			}
		}
		else if (nodeName == "tool-window")
		{
			toolWindow = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeName == "transparency")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			transparency = (float) atof(value.c_str());
		}
		else if (nodeName == "x")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			x = atoi(value.c_str());
		}
		else if (nodeName == "y")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			y = atoi(value.c_str());
		}
		else if (nodeName == "width")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			width = atoi(value.c_str());
		}
		else if (nodeName == "height")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			height = atoi(value.c_str());
		}
		else if (nodeName == "visible")
		{
			visible = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeName == "min-width")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			minWidth = atoi(value.c_str());
		}
		else if (nodeName == "max-width")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			maxWidth = atoi(value.c_str());
		}
		else if (nodeName == "min-height")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			minHeight = atoi(value.c_str());
		}
		else if (nodeName == "max-height")
		{
			std::string value(ConfigUtils::GetNodeValue(child));
			maxHeight = atoi(value.c_str());
		}
		else if (nodeName == "top-most")
		{
			topMost = ConfigUtils::GetNodeValueAsBool(child);
		}
#ifdef OS_OSX
		else if (nodeName == "texturedBackground")
		{
			texturedBackground = ConfigUtils::GetNodeValueAsBool(child);
		}
#endif
		child = child->next;
	}

	if (minWidth <= 0)
	{
		minWidth = -1;
	}
	else if (width < minWidth)
	{
		width = minWidth;
	}

	if (maxWidth <= 0)
	{
		maxWidth = -1;
	}
	else if (width > maxWidth)
	{
		width = maxWidth;
	}

	if (minHeight <= 0)
	{
		minHeight = -1;
	}
	else if (height < minHeight)
	{
		height = minHeight;
	}

	if (maxHeight <= 0)
	{
		maxHeight = -1;
	}
	else if (height > maxHeight)
	{
		height = maxHeight;
	}
}

std::string WindowConfig::ToString()
{
	std::ostringstream stream;

	stream << "[WindowConfig id=" << winid
		<< ", x=" << x << ", y=" << y
		<< ", width=" << width << ", height=" << height
		<< ", url=" << url
		<< "]";

	return stream.str();
}

} // namespace ti
