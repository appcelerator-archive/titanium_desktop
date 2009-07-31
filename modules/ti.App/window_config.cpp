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

#define SET_STRING(name, prop) \
{ \
	SharedValue v = properties->Get(#name); \
	if (v->IsString()) \
	{ \
		std::string value(v->ToString()); \
		this->prop = value; \
	} \
}

#define SET_BOOL(name, prop) \
{ \
	SharedValue v = properties->Get(#name); \
	if (v->IsString()) \
	{ \
		std::string value(v->ToString()); \
		if (value=="yes" || value=="1" || value=="true" || value=="True") \
		{\
			this->prop = true; \
		} \
		else \
		{ \
			this->prop = false; \
		} \
	} \
	else if (v->IsInt()) \
	{ \
		this->prop = v->ToInt() == 1; \
	} \
	else if (v->IsBool()) \
	{ \
		this->prop = v->ToBool(); \
	} \
}

#define SET_INT(name, prop) \
{ \
	SharedValue v = properties->Get(#name); \
	if (v->IsNumber()) \
	{ \
		this->prop = v->ToInt(); \
	} \
}

#define SET_DOUBLE(name, prop) \
{ \
	SharedValue v = properties->Get(#name); \
	if (v->IsNumber()) \
	{ \
		this->prop = v->ToDouble(); \
	} \
}

using namespace ti;

int WindowConfig::DEFAULT_POSITION = -404404404;
int WindowConfig::windowCount = 0;
std::string WindowConfig::blankPageURL("about:blank");

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
	this->usingScrollbars = true;
	this->fullscreen = false;
	this->maximized = false;
	this->minimized = false;
	this->visible = true;
	this->topMost = false;

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

	this->url = WindowConfig::blankPageURL;
	this->title = Host::GetInstance()->GetApplication()->name;
}

void WindowConfig::UseProperties(SharedKObject properties)
{
	this->SetDefaults();

	SET_STRING(id, winid)
	SET_STRING(url, url);
	SET_STRING(urlRegex, urlRegex);
	SET_STRING(title, title);
	SET_INT(x, x);
	SET_INT(y, y);
	SET_INT(width, width);
	SET_INT(minWidth, minWidth);
	SET_INT(maxWidth, maxWidth);
	SET_INT(height, height);
	SET_INT(minHeight, minHeight);
	SET_INT(maxHeight, maxHeight);
	SET_BOOL(visible, visible);
	SET_BOOL(maximizable, maximizable);
	SET_BOOL(minimizable, minimizable);
	SET_BOOL(closeable, closeable);
	SET_BOOL(resizable, resizable);
	SET_BOOL(fullscreen, fullscreen);
	SET_BOOL(maximized, maximized);
	SET_BOOL(minimized, minimized);
	SET_BOOL(usingChrome, usingChrome);
	SET_BOOL(usingScrollbars, usingScrollbars);
	SET_BOOL(topMost, topMost);
	SET_DOUBLE(transparency, transparency);
}

WindowConfig::WindowConfig(WindowConfig *config, std::string& url)
{
	this->SetDefaults();
	this->url = url;

	if (config == NULL) // Just use defaults if not found
	{
		return;
	}

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

}

WindowConfig::WindowConfig(void* data)
{
	xmlElementPtr element = (xmlElementPtr) data;
	SetDefaults();

	xmlNodePtr child = element->children;
	while (child != NULL)
	{
		if (nodeNameEquals(child, "id"))
		{
			winid = ConfigUtils::GetNodeValue(child);
		}
		else if (nodeNameEquals(child, "title"))
		{
			title = ConfigUtils::GetNodeValue(child);
		}
		else if (nodeNameEquals(child, "url"))
		{
			url = ConfigUtils::GetNodeValue(child);
			url = AppConfig::Instance()->InsertAppIDIntoURL(url);
		}
		else if (nodeNameEquals(child, "url-regex"))
		{
			urlRegex = ConfigUtils::GetNodeValue(child);
		}
		else if (nodeNameEquals(child, "maximizable"))
		{
			maximizable = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeNameEquals(child, "minimizable"))
		{
			minimizable = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeNameEquals(child, "closeable"))
		{
			closeable = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeNameEquals(child, "resizable"))
		{
			resizable = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeNameEquals(child, "fullscreen"))
		{
			fullscreen = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeNameEquals(child, "maximized"))
		{
			maximized = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeNameEquals(child, "minimized"))
		{
			minimized = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeNameEquals(child, "chrome"))
		{
			usingChrome = ConfigUtils::GetNodeValueAsBool(child);
			std::string scrollbars = ConfigUtils::GetPropertyValue(child, "scrollbars");
			if (!scrollbars.empty())
			{
				usingScrollbars = ConfigUtils::StringToBool(scrollbars);
			}
		}
		else if (nodeNameEquals(child, "transparency"))
		{
			std::string value = ConfigUtils::GetNodeValue(child);
			transparency = (float) atof(value.c_str());
		}
		else if (nodeNameEquals(child, "x"))
		{
			std::string value = ConfigUtils::GetNodeValue(child);
			x = atoi(value.c_str());
		}
		else if (nodeNameEquals(child, "y"))
		{
			std::string value = ConfigUtils::GetNodeValue(child);
			y = atoi(value.c_str());
		}
		else if (nodeNameEquals(child, "width"))
		{
			std::string value = ConfigUtils::GetNodeValue(child);
			width = atoi(value.c_str());
		}
		else if (nodeNameEquals(child, "height"))
		{
			std::string value = ConfigUtils::GetNodeValue(child);
			height = atoi(value.c_str());
		}
		else if (nodeNameEquals(child, "visible"))
		{
			visible = ConfigUtils::GetNodeValueAsBool(child);
		}
		else if (nodeNameEquals(child, "min-width"))
		{
			std::string value = ConfigUtils::GetNodeValue(child);
			minWidth = atoi(value.c_str());
		}
		else if (nodeNameEquals(child, "max-width"))
		{
			std::string value = ConfigUtils::GetNodeValue(child);
			maxWidth = atoi(value.c_str());
		}
		else if (nodeNameEquals(child, "min-height"))
		{
			std::string value = ConfigUtils::GetNodeValue(child);
			minHeight = atoi(value.c_str());
		}
		else if (nodeNameEquals(child, "max-height"))
		{
			std::string value = ConfigUtils::GetNodeValue(child);
			maxHeight = atoi(value.c_str());
		}
		else if (nodeNameEquals(child, "top-most"))
		{
			topMost = ConfigUtils::GetNodeValueAsBool(child);
		}
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
