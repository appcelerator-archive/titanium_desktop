/*
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
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

#include "../Notification.h"

#include <libnotify/notify.h>

namespace Titanium {

/*static*/
bool Notification::InitializeImpl()
{
	std::string& appName = Host::GetInstance()->GetApplication()->name;
	return notify_init(appName.c_str());
}

/*static*/
void Notification::ShutdownImpl()
{
	notify_uninit();
}

void Notification::CreateImpl()
{
	this->notification = 0;
}

void Notification::DestroyImpl()
{
	if (this->notification)
		g_object_unref(G_OBJECT(this->notification));
}

bool Notification::ShowImpl()
{
	// Make sure title is NOT empty. Libnotify will not
	// display if the length is zero.
	if (this->title.empty())
		this->title = " ";

	// Convert icon URL to path
	std::string iconPath = URLUtils::URLToPath(this->iconURL);

	// Delete old notification if present
	if (this->notification)
		g_object_unref(G_OBJECT(this->notification));

	this->notification = notify_notification_new(
		this->title.c_str(),
		this->message.c_str(),
		iconPath.c_str(),
		NULL);

	int timeout = (this->timeout > 0) ? this->timeout * 1000 : this->timeout;
	notify_notification_set_timeout(this->notification, timeout);

	return notify_notification_show(this->notification, NULL);
}

bool Notification::HideImpl()
{
	if (!this->notification)
		return true;

		return notify_notification_close(this->notification, NULL);
}

} // namespace Titanium

