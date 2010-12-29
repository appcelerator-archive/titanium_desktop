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

#include "SoundGStreamer.h"

namespace Titanium {

static gboolean GSTBusCallback(GstBus* bus, GstMessage* message, gpointer data);

SoundGStreamer::SoundGStreamer(std::string& path) :
    Sound(path),
    pipeline(0)
{
    // Convert the path back into a file:// URL. We don't use the
    // original URL here because it may be an app:// or ti:// URL.
    this->fileURL = URLUtils::PathToFileURL(this->path);
    this->Load();
}

void SoundGStreamer::LoadImpl()
{
    // The superclass will be responsible for unloading before calling load.
    this->pipeline = gst_element_factory_make("playbin", 0);
    g_object_set(G_OBJECT(pipeline), "uri", url.c_str(), NULL);

    // Add a callback to listen for GST bus messages
    this->duplicate();
    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    this->watchEventSourceId = gst_bus_add_watch(bus, GSTBusCallback, this);
    gst_object_unref(bus);
}

void SoundGStreamer::UnloadImpl()
{
    if (!this->pipeline)
        return;

    g_source_remove(this->watchEventSourceId);
    this->watchEventSourceId = -1;
    this->release();

    gst_element_set_state(this->pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(this->pipeline));
    this->pipeline = 0;
}

void SoundGStreamer::PlayImpl()
{
    if (!this->pipeline)
        return;

    gst_element_set_state(this->pipeline, GST_STATE_PLAYING);
}

void SoundGStreamer::PauseImpl()
{
    if (!this->pipeline)
        return;

    gst_element_set_state(this->pipeline, GST_STATE_PAUSED);
}

void SoundGStreamer::StopImpl()
{
    if (!this->pipeline)
        return;

    gst_element_set_state(this->pipeline, GST_STATE_NULL);
    gst_element_seek_simple( this->pipeline, GST_FORMAT_TIME,
        (GstSeekFlags) (GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT), 0);
}

void SoundGStreamer::SetVolumeImpl(double volume)
{
    if (!this->pipeline)
        return;

    volume = volume * 10;
    g_object_set(G_OBJECT(this->pipeline), "volume", volume, NULL);
}

double SoundGStreamer::GetVolumeImpl()
{
    if (!this->pipeline)
        return 0.0;

    gdouble gvolume;
    g_object_get(this->pipeline, "volume", &gvolume, NULL);
    return gvolume / 10;
}

static gboolean GSTBusCallback(GstBus* bus, GstMessage* message, gpointer data)
{
    SoundGStreamer* sound = (SoundGStreamer*) data;
    if (message->type == GST_MESSAGE_ERROR)
    {
        sound->Stop();

        GError* err;
        gst_message_parse_error(message, &err, NULL);
        Logger::Get("Media.Sound")->Error("Error while playing sound: %s", err->message);
        g_error_free(err);
    }
    else if (message->type == GST_MESSAGE_EOS)
    {
        sound->SoundCompletedIteration();
    }

    return TRUE;
}

} // namespace Titanium
