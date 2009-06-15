/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "win32_sound.h"
#include <comutil.h>
namespace ti
{
	void Win32Sound::InitGraphBuilder ()
	{
		HRESULT hr;

		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
			IID_IGraphBuilder, (void **) &graph_builder);

		hr = graph_builder->QueryInterface(IID_IMediaControl, (void **) &media_control);
		hr = graph_builder->QueryInterface(IID_IMediaEventEx, (void **)&media_event_ex);
		hr = graph_builder->QueryInterface(IID_IMediaSeeking, (void **)&media_seeking);
	}

	Win32Sound::Win32Sound(std::string &url) : Sound(url), callback(0), graph_builder(NULL), media_control(NULL), media_event_ex(NULL), media_seeking(NULL)
	{
		InitGraphBuilder();

		std::string path = Host::GetInstance()->
			GetGlobalObject()->CallNS("App.appURLToPath", Value::NewString(url))->ToString();
		BSTR pathBstr = _bstr_t(path.c_str());

		graph_builder->RenderFile(pathBstr, NULL);
	}

	Win32Sound::~Win32Sound()
	{
		if (graph_builder)
			graph_builder->Release();
		if (media_control)
			media_control->Release();
		if (media_event_ex)
			media_event_ex->Release();
		if (media_seeking)
			media_seeking->Release();

		if (callback)
		{
			delete callback;
		}
	}
	void Win32Sound::Play()
	{
		media_control->Run();
	}
	void Win32Sound::Pause()
	{
		HRESULT hr = media_control->Pause();
		if (hr == S_FALSE) {
			// wait for the state change
			FILTER_STATE fs;
			HRESULT hr = media_control->GetState(500, (OAFilterState*)&fs);
		}
	}
	void Win32Sound::Stop()
	{
		media_control->Stop();
		//REFERENCE_TIME rt;
		//media_seeking->SetPositions(&rt, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
	}
	void Win32Sound::Reload()
	{
	}
	void Win32Sound::SetVolume(double volume)
	{
	}
	double Win32Sound::GetVolume()
	{
		return 0.0;
	}
	void Win32Sound::SetLooping(bool loop)
	{
	}
	bool Win32Sound::IsLooping()
	{
		return false;
	}
	bool Win32Sound::IsPlaying()
	{
		FILTER_STATE fs;
		HRESULT hr = media_control->GetState(500, (OAFilterState*)&fs);
		return (fs == State_Running);
	}
	bool Win32Sound::IsPaused()
	{
		FILTER_STATE fs;
		HRESULT hr = media_control->GetState(500, (OAFilterState*)&fs);
		return (fs == State_Paused);
	}
	void Win32Sound::OnComplete(SharedKMethod callback)
	{
		// TODO: implement me
	}
}
