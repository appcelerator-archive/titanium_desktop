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

#include "MediaGStreamer.h"

#include <gdk/gdk.h>

namespace Titanium {

MediaGStreamer::MediaGStreamer(KObjectRef global)
    : Media(global)
{
    char **argv;
    int argc = 0;
    gst_init(&argc, &argv);
}

MediaGStreamer::~MediaGStreamer()
{
}

void MediaGStreamer::Beep()
{
    gdk_beep();
}

KObjectRef MediaGStreamer::CreateSound(std::string& url)
{
    //This is a path so, turn it into a file:// URL
    std::string myurl = url;
    std::string path = URLUtils::URLToPath(url);
    if (path.find("://") == std::string::npos)
    {
        myurl = URLUtils::PathToFileURL(path);
    }
    return new SoundGStreamer(myurl);
}

} // namespace Titanium
