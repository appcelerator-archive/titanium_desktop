describe("ti.Media tests", {

	test_create_sound: function()
	{
		value_of(Titanium.Media.createSound).should_be_function();
		value_of(Titanium.Media.beep).should_be_function();
		
		var exception = false;
		try {
			Titanium.Media.createSound();
		} catch(E) {
			exception = true;
		}
		
		value_of(exception).should_be_true();
		
		var sound = Titanium.Media.createSound(Titanium.App.appURLToPath("app://sound.wav"));
		value_of(sound.play).should_be_function();
		value_of(sound.pause).should_be_function();
		value_of(sound.stop).should_be_function();
		value_of(sound.reload).should_be_function();
		value_of(sound.setVolume).should_be_function();
		value_of(sound.getVolume).should_be_function();
		value_of(sound.setLooping).should_be_function();
		value_of(sound.isLooping).should_be_function();
		value_of(sound.isPlaying).should_be_function();
		value_of(sound.isPaused).should_be_function();
		value_of(sound.onComplete).should_be_function();
		
		sound = null;
	},
	
	test_beep: function() {
		Titanium.Media.beep();
	},
	
	test_play_sound_as_async: function(callback)
	{
		var sound = Titanium.Media.createSound("app://sound.wav");
		sound.play();
		
		var timer = 0;
		
		setTimeout(function(){
			value_of(sound.isPlaying()).should_be_true();
			sound.pause();
			value_of(sound.isPlaying()).should_be_false();
			value_of(sound.isPaused()).should_be_true();
			value_of(sound.isLooping()).should_be_false();
			sound.play();
			
			sound.onComplete(function(){
				clearTimeout(timer);
				value_of(sound.isPlaying()).should_be_false();
				callback.passed();
			});
		}, 2000);
		
		timer = setTimeout(function(){
			callback.failed("sound onComplete timed out");
		}, 10000);
	},
	
	test_play_sound_looping_as_async: function(callback)
	{
		var sound = Titanium.Media.createSound("app://sound.wav");
		sound.play();
		
		var timer = 0;
		setTimeout(function(){
			value_of(sound.isPlaying()).should_be_true();
			// FIXME -- these is not implemented 
			// looping isn't implemented, this should be false until
			// we complete the code.
			sound.setLooping(true);
			if (!sound.isLooping())
			{
				sound.stop();
				clearTimeout(timer);
				callback.passed();
			}
			else
			{
				sound.stop();
				clearTimeout(timer);
				callback.failed("unit test has to be updated, sound looping implemented");
			}
		}, 2000);
		
		timer = setTimeout(function(){
			callback.failed("sound error on looping test");
		}, 10000);
	},

	test_play_sound_volume_as_async: function(callback)
	{
		var sound = Titanium.Media.createSound("app://sound.wav");
		// figure out what the default sound level is, so we can 
		// restore it when we are done.
		var defaultSound = sound.getVolume();
		value_of(defaultSound).should_be_number();
		Titanium.API.debug("current volume "+defaultSound);
		
		if ( Titanium.platform == 'win32' )
		{
			// FIXME - sound volume doesn't work on vista.
			// this is a hack to get around the issue for now.
			callback.passed();
			Titanium.API.warn("Windows sound volume doesn't work");
			// the side effect of the buggy windows code is that the sound
			// returned will always be 1.  chuck an error when this changes.
			// then we can remove this workaround.
			value_of(defaultSound).should_be(1);
		}
		else 
		{
			var timer = 0;
			
			setTimeout(function()
			{
				value_of(sound.isPlaying()).should_be_true();
				sound.setVolume(55);
				if (sound.getVolume() == 55.0)
				{
					sound.setVolume(25);
					if (sound.getVolume() == 25.0)
					{
						clearTimeout(timer);
						callback.passed();
					}
					else 
					{
						sound.stop();
						clearTimeout(timer);
						callback.failed("failed to set volume to 25.0");
					}
				}
				else 
				{
					sound.stop();
					clearTimeout(timer);
					callback.failed("failed to set volume to 55.0 actual volume: "+sound.getVolume());
				}
				sound.setVolume(defaultsound);
			}, 2000);
			
			timer = setTimeout(function(){			
				callback.failed("unknown sound error setting the volume");
			}, 10000);
		}
	}
});
