/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _UI_MODULE_GTK_H_
#define _UI_MODULE_GTK_H_

#include <kroll/kroll.h>

using kroll::Value;
using kroll::KObject;
using kroll::StaticBoundObject;
using kroll::KMethod;
using kroll::StaticBoundMethod;
using kroll::KList;
using kroll::StaticBoundList;

#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <webkit/webkit.h>
#include <webkit/webkittitanium.h>

// X11 defines this again for some reason
#undef DisplayString

#include "gtk_menu.h"
#include "gtk_menu_item.h"
#include "gtk_user_window.h"
#include "gtk_tray_item.h"
#include "gtk_ui_binding.h"

#endif
