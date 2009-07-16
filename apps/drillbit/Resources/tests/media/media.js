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

	// We can re-enable this test when looping is implemented on  Win32.
	//test_play_sound_looping_as_async: function(callback)
	//{
	//	var sound = Titanium.Media.createSound("app://sound.wav");
	//	sound.play();
	//	
	//	var timer = 0;
	//	setTimeout(function(){
	//		value_of(sound.isPlaying()).should_be_true();
	//		// FIXME -- these is not implemented 
	//		// looping isn't implemented, this should be false until
	//		// we complete the code.
	//		sound.setLooping(true);
	//		if (!sound.isLooping())
	//		{
	//			sound.stop();
	//			clearTimeout(timer);
	//			callback.passed();
	//		}
	//		else
	//		{
	//			sound.stop();
	//			clearTimeout(timer);
	//			callback.failed("unit test has to be updated, sound looping implemented");
	//		}
	//	}, 2000);
	//	
	//	timer = setTimeout(function(){
	//		callback.failed("sound error on looping test");
	//	}, 10000);
	//},
	test_play_sound_volume_as_async: function(callback)
	{
		var sound = Titanium.Media.createSound("app://sound.wav");
		var timer = 0;
		sound.play();

		steps = [
			function() {
				if (!sound.isPlaying()) {
					callback.failed("Sound did not start");
				}
				sound.setVolume(0.5);
			},
			function() {
				if (sound.getVolume() != 0.5) {
					callback.failed("Could not set volume to 0.5");
				}
				sound.setVolume(0.25);
			},
			function() {
				if (sound.getVolume() != 0.25) {
					callback.failed("Could not set volume to 0.5");
				}
				sound.setVolume(-0.25);
			},
			function() {
				if (sound.getVolume() != 0) {
					callback.failed("Volume should be 0 was " + sound.getVolume());
				}
				sound.setVolume(100);
			},
			function() {
				if (sound.getVolume() != 1) {
					callback.failed("Volume should be 1 was " + sound.getVolume());
				}
				sound.stop();
				callback.passed();
			}];

		var run_next_test = function() {
			var test = steps.shift();
			test();
			setTimeout(run_next_test, 250);
		};
		setTimeout(run_next_test, 250);
	}
});
