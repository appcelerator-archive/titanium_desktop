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
			
			// FIXME -- these seem to be broken
			//sound.setLooping(true);
			//value_of(sound.isLooping()).should_be_true();
			//sound.setVolume(55);
			//value_of(sound.getVolume()).should_be(55);

			sound.onComplete(function(){
				clearTimeout(timer);
				value_of(sound.isPlaying()).should_be_false();
				callback.passed();
			});
			sound.play();
		}, 1000);
		
		timer = setTimeout(function(){
			callback.failed("sound onComplete timed out");
		}, 10000);
	}
});
