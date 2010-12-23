/*
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
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

#include "SoundWin.h"

#include <comutil.h>

#include <stdlib.h>
#include <math.h>

namespace Titanium {

UINT SoundWin::graphNotifyMessage = 
	::RegisterWindowMessage(L"GraphNotifyMediaModule");

SoundWin::SoundWin(std::string &url) :
	Sound(url),
	widePath(::UTF8ToWide(this->path)),
	graphBuilder(0),
	mediaControl(0),
	mediaEventEx(0),
	mediaSeeking(0)
{
	HRESULT hr;

	hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER,
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

	HWND hwnd = Host::GetInstance()->AddMessageHandler(
		&SoundWin::StaticGraphCallback);
	mediaEventEx->SetNotifyWindow((OAHWND) hwnd, graphNotifyMessage,
		reinterpret_cast<LONG_PTR>(this));

	this->Load();
}

SoundWin::~SoundWin()
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

void SoundWin::LoadImpl()
{
	if (!graphBuilder)
		return;

	BSTR pathBstr = SysAllocString(this->widePath.c_str());
	graphBuilder->RenderFile(pathBstr, 0);
}

void SoundWin::UnloadImpl()
{
}

void SoundWin::PlayImpl()
{
	if (!mediaControl)
		return;

	mediaControl->Run();
}

void SoundWin::PauseImpl()
{
	if (!mediaControl)
		return;

	mediaControl->Stop();
}

void SoundWin::StopImpl()
{
	if (!mediaControl)
		return;

	if (state == PLAYING)
		mediaControl->Stop();

	// Set the stream location to start at 0 (absolute beginning)
	// and have it end at NoPosition i.e. keep playing.
	LONGLONG newStartPosition = 0;
	mediaSeeking->SetPositions(
		&newStartPosition, AM_SEEKING_AbsolutePositioning, 
		0, AM_SEEKING_NoPositioning);
}

void SoundWin::SetVolumeImpl(double volume)
{
	if (!basicAudio)
		return;

	long newVolume = -1 * floor((1.0-volume)*4000);
	basicAudio->put_Volume(newVolume);
}

double SoundWin::GetVolumeImpl()
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

bool SoundWin::GraphCallback(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	// Get the next event on the queue and wait 0 milliseconds
	// for other events if the queue is empty (3rd parameter).
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
bool SoundWin::StaticGraphCallback(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	if (message != graphNotifyMessage || !lParam)
		return false;

	SoundWin* sound = reinterpret_cast<SoundWin*>(lParam);
	return sound->GraphCallback(hwnd, message, wParam, lParam);
}

} // namespace Titanium
