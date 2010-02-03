/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "win32_sound.h"
#include <comutil.h>
#include <stdlib.h>
#include <math.h>
#include <kroll/win32/host.h>

namespace ti
{
	UINT Win32Sound::graphNotifyMessage = 
		::RegisterWindowMessage(L"GraphNotifyMediaModule");

	Win32Sound::Win32Sound(std::string &url) :
		Sound(url),
		path(::UTF8ToWide(URLUtils::URLToPath(url))),
		graphBuilder(NULL),
		mediaControl(NULL),
		mediaEventEx(NULL),
		mediaSeeking(NULL)
	{
		HRESULT hr;

		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
			IID_IGraphBuilder, (void **) &graphBuilder);
		if (FAILED(hr))
			throw ValueException::FromString("Failed creating Graph Builder for sound");

		hr = graphBuilder->QueryInterface(IID_IBasicAudio, (void**) &basicAudio);
		if (FAILED(hr))
			throw ValueException::FromString("Failed querying IBasicAudio for sound");

		hr = graphBuilder->QueryInterface(IID_IMediaControl, (void **) &mediaControl);
		if (FAILED(hr))
			throw ValueException::FromString("Failed querying IMediaControl for sound");

		hr = graphBuilder->QueryInterface(IID_IMediaEventEx, (void **) &mediaEventEx);
		if (FAILED(hr))
			throw ValueException::FromString("Failed querying IMediaEventEx for sound");

		hr = graphBuilder->QueryInterface(IID_IMediaSeeking, (void **) &mediaSeeking);
		if (FAILED(hr))
			throw ValueException::FromString("Failed querying IMediaSeeking for sound");

		HWND hwnd = Win32Host::Win32Instance()->AddMessageHandler(
			&Win32Sound::StaticGraphCallback);
		mediaEventEx->SetNotifyWindow((OAHWND) hwnd, graphNotifyMessage,
			reinterpret_cast<LONG_PTR>(this));

		this->Load();
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
	}

	void Win32Sound::LoadImpl()
	{
		if (!graphBuilder)
			return;

		BSTR pathBstr = SysAllocString(this->path.c_str());
		graphBuilder->RenderFile(pathBstr, NULL);
	}

	void Win32Sound::UnloadImpl()
	{
	}

	void Win32Sound::PlayImpl()
	{
		if (!mediaControl)
			return;

		mediaControl->Run();
	}

	void Win32Sound::PauseImpl()
	{
		if (!mediaControl)
			return;

		mediaControl->Stop();
	}

	void Win32Sound::StopImpl()
	{
		if (!mediaControl)
			return;

		if (state == PLAYING)
			mediaControl->Stop();

		// Set the stream location to start at 0 (absolute beginning)
		// and have it end at NoPosition. I.E. keep playing.
		LONGLONG newStartPosition = 0;
		mediaSeeking->SetPositions(
			&newStartPosition, AM_SEEKING_AbsolutePositioning, 
			NULL, AM_SEEKING_NoPositioning);
	}

	void Win32Sound::SetVolumeImpl(double volume)
	{
		if (!basicAudio)
			return;

		long newVolume = -1 * floor((1.0-volume)*4000);
		basicAudio->put_Volume(newVolume);
	}

	double Win32Sound::GetVolumeImpl()
	{
		long wvolume = 0;
		HRESULT hr = basicAudio->get_Volume(&wvolume);
		if (SUCCEEDED(hr))
		{
			if (wvolume < -4000.0)
				wvolume = -4000.0;

			return 1.0 - (abs(wvolume) / 4000.0);
		}
		else
		{
			throw ValueException::FromString("Could not get volume.");
		}
	}

	bool Win32Sound::GraphCallback(HWND hwnd, UINT message,
		WPARAM wParam, LPARAM lParam)
	{
		// Get the next event on the queue and wait 
		// 0 milliseconds when the queue is empty.
		long code, param1, param2;
		HRESULT hr = mediaEventEx->GetEvent(&code, &param1, &param2, 0);
		while (hr == S_OK)
		{
			mediaEventEx->FreeEventParams(code, param1, param2);
			if ((EC_COMPLETE == code) || (EC_USERABORT == code))
			{
				this->SoundCompletedIteration();
			}

			hr = mediaEventEx->GetEvent(&code, &param1, &param2, 0);
		}

		return true;
	}

	/*static*/
	bool Win32Sound::StaticGraphCallback(HWND hwnd, UINT message,
		WPARAM wParam, LPARAM lParam)
	{
		if (message != graphNotifyMessage || !lParam)
			return false;

		Win32Sound* sound = reinterpret_cast<Win32Sound*>(lParam);
		return sound->GraphCallback(hwnd, message, wParam, lParam);
	}
}
