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

#ifndef Sound_h
#define Sound_h

#include <kroll/kroll.h>

namespace Titanium {

class Sound : public StaticBoundObject {
public:
    Sound(std::string& url);
    virtual ~Sound();

    void Play(const ValueList& args, KValueRef result);
    void Pause(const ValueList& args, KValueRef result);
    void Stop(const ValueList& args, KValueRef result);
    void Reload(const ValueList& args, KValueRef result);
    void SetVolume(const ValueList& args, KValueRef result);
    void GetVolume(const ValueList& args, KValueRef result);
    void SetLooping(const ValueList& args, KValueRef result);
    void IsLooping(const ValueList& args, KValueRef result);
    void IsPlaying(const ValueList& args, KValueRef result);
    void IsPaused(const ValueList& args, KValueRef result);
    void SetOnComplete(const ValueList& args, KValueRef result);

    void Play();
    void Pause();
    void Stop();
    void Reload();
    void SetVolume(double newVolume);
    double GetVolume();
    void SetLooping(bool newLooping);
    bool IsLooping();
    bool IsPlaying();
    bool IsPaused();
    void SetOnComplete(KMethodRef newCallback);
    void Load();
    void Unload();
    void SoundCompletedIteration();

    virtual void LoadImpl() = 0;
    virtual void UnloadImpl() = 0;
    virtual void PlayImpl() = 0;
    virtual void PauseImpl() = 0;
    virtual void StopImpl() = 0;
    virtual void SetVolumeImpl(double volume) = 0;
    virtual double GetVolumeImpl() = 0;;

protected:
    enum SoundState { PLAYING, PAUSED, STOPPED, END_OF_ITERATION };
    SoundState state;
    std::string url;
    std::string path;
    KMethodRef callback;
    bool looping;
};

} // namespace Titanium

#endif
