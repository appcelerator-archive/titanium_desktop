/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "gst_media_binding.h"

namespace ti
{
	static gboolean GSTBusCallback(GstBus* bus, GstMessage* message, gpointer data);
	GstSound::GstSound(std::string& path) :
		Sound(path),
		pipeline(0)
	{
		// Convert the path back into a file:// URL. We don't use the
		// original URL here because it may be an app:// or ti:// URL.
		this->fileURL = URLUtils::PathToFileURL(this->path);
		this->Load();
	}

	void GstSound::LoadImpl()
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

	void GstSound::UnloadImpl()
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

	void GstSound::PlayImpl()
	{
		if (!this->pipeline)
			return;

		gst_element_set_state(this->pipeline, GST_STATE_PLAYING);
	}

	void GstSound::PauseImpl()
	{
		if (!this->pipeline)
			return;

		gst_element_set_state(this->pipeline, GST_STATE_PAUSED);
	}

	void GstSound::StopImpl()
	{
		if (!this->pipeline)
			return;

		gst_element_set_state(this->pipeline, GST_STATE_NULL);
		gst_element_seek_simple( this->pipeline, GST_FORMAT_TIME,
			(GstSeekFlags) (GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT), 0);
	}

	void GstSound::SetVolumeImpl(double volume)
	{
		if (!this->pipeline)
			return;

		volume = volume * 10;
		g_object_set(G_OBJECT(this->pipeline), "volume", volume, NULL);
	}

	double GstSound::GetVolumeImpl()
	{
		if (!this->pipeline)
			return 0.0;

		gdouble gvolume;
		g_object_get(this->pipeline, "volume", &gvolume, NULL);
		return gvolume / 10;
	}

	static gboolean GSTBusCallback(GstBus* bus, GstMessage* message, gpointer data)
	{
		GstSound* sound = (GstSound*) data;
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
}
