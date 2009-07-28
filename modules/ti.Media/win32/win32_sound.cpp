/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "win32_sound.h"
#include <comutil.h>
#include <stdlib.h>
#include <math.h>

#include<sstream>

namespace ti
{
	UINT Win32Sound::graphNotifyMessage = ::RegisterWindowMessage(PRODUCT_NAME "GraphNotify");

	void Win32Sound::InitGraphBuilder ()
	{
		HRESULT hr;

		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
			IID_IGraphBuilder, (void **) &graphBuilder);
		if (FAILED(hr)) throw ValueException::FromString("Failed creating Graph Builder for sound");
		
		hr = graphBuilder->QueryInterface(IID_IBasicAudio, (void**)&basicAudio);
		if (FAILED(hr)) throw ValueException::FromString("Failed querying IBasicAudio for sound");
		
		hr = graphBuilder->QueryInterface(IID_IMediaControl, (void **)&mediaControl);
		if (FAILED(hr)) throw ValueException::FromString("Failed querying IMediaControl for sound");
		
		hr = graphBuilder->QueryInterface(IID_IMediaEventEx, (void **)&mediaEventEx);
		if (FAILED(hr)) throw ValueException::FromString("Failed querying IMediaEventEx for sound");
		
		hr = graphBuilder->QueryInterface(IID_IMediaSeeking, (void **)&mediaSeeking);
		if (FAILED(hr)) throw ValueException::FromString("Failed querying IMediaSeeking for sound");
	}

	Win32Sound::Win32Sound(std::string &url) :
		Sound(url),
		callback(0),
		graphBuilder(NULL),
		mediaControl(NULL),
		mediaEventEx(NULL),
		mediaSeeking(NULL)
	{
		InitGraphBuilder();

		std::string path = URLUtils::AppURLToPath(url);
		BSTR pathBstr = _bstr_t(path.c_str());

		// why does a sound event need an HWND?? oh windows..
		SharedValue value = Host::GetInstance()->GetGlobalObject()->GetNS("UI.mainWindow.windowHandle");
		if (value->IsVoidPtr())
		{
			HWND hwnd = (HWND) value->ToVoidPtr();
			mediaEventEx->SetNotifyWindow((OAHWND)hwnd, graphNotifyMessage, 0);
			
			MethodCallback* messageCallback =
				NewCallback<Win32Sound, const ValueList&, SharedValue>(this, &Win32Sound::GraphCallback);
			SharedKMethod method = new StaticBoundMethod(messageCallback);
			Host::GetInstance()->GetGlobalObject()->CallNS(
				"UI.mainWindow.addMessageHandler", Value::NewInt(graphNotifyMessage), Value::NewMethod(method));
		}

		graphBuilder->RenderFile(pathBstr, NULL);
	}

	Win32Sound::~Win32Sound()
	{
		if (graphBuilder)
			graphBuilder->Release();
		if (mediaControl)
			mediaControl->Release();
		if (mediaEventEx)
			mediaEventEx->Release();
		if (mediaSeeking)
			mediaSeeking->Release();

		if (callback)
		{
			delete callback;
		}
	}
	
	void Win32Sound::Play()
	{
		mediaControl->Run();
	}
	
	void Win32Sound::Pause()
	{
		HRESULT hr = mediaControl->Pause();
		if (hr == S_FALSE) {
			// wait for the state change
			FILTER_STATE fs;
			HRESULT hr = mediaControl->GetState(500, (OAFilterState*)&fs);
		}
	}
	
	void Win32Sound::Stop()
	{
		mediaControl->Stop();
		//REFERENCE_TIME rt;
		//mediaSeeking->SetPositions(&rt, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
	}
	
	void Win32Sound::Reload()
	{
	}
	
	void Win32Sound::SetVolume(double volume)
	{
		if (volume > 1.0) {
			volume = 1.0;
		}
		long newVolume = -1 * floor((1.0-volume)*4000);
		basicAudio->put_Volume(newVolume);
	}
	
	double Win32Sound::GetVolume()
	{
		long volume;
		HRESULT hr = basicAudio->get_Volume(&volume);

		if ( SUCCEEDED(hr) )
		{
			std::stringstream s;
			s << volume;

			std::string buf;

			buf = "current volume from IBasicAudio (" + s.str() + ")";
			PRINTD(buf.c_str());

			if (volume < -4000.0) {
				volume = -4000.0;
			}
		}
		else 
		{
			throw ValueException::FromString("IBasicAudio for current volume");		
		}
		return 1.0 - (abs(volume) / 4000.0);
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
		HRESULT hr = mediaControl->GetState(500, (OAFilterState*)&fs);
		return (fs == State_Running);
	}
	
	bool Win32Sound::IsPaused()
	{
		FILTER_STATE fs;
		HRESULT hr = mediaControl->GetState(500, (OAFilterState*)&fs);
		return (fs == State_Paused);
	}
	
	void Win32Sound::Completed()
	{
		ValueList args;
		SharedValue arg = Value::NewBool(true);
		args.push_back(arg);
		
		// GraphCallback already executes from the main thread (WndProc event)
		(*this->callback)->Call(args);
	}
	
	void Win32Sound::GraphCallback(const ValueList& args, SharedValue result)
	{
		if (!this->callback) return;
		
		long code, param1, param2;
		HRESULT hr;
		while (hr = mediaEventEx->GetEvent(&code, &param1, &param2, 0), SUCCEEDED(hr))
		{ 
			hr = mediaEventEx->FreeEventParams(code, param1, param2);
			if ((EC_COMPLETE == code) || (EC_USERABORT == code))
			{
				mediaControl->Stop();
				this->Completed();
				break;
			} 
		}
	}
	
	void Win32Sound::OnComplete(SharedKMethod callback)
	{
		if (this->callback)
		{
			delete this->callback;
		}
		
		this->callback = new SharedKMethod(callback);
	}
}
