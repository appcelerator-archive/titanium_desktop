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
	
	test_beep: function()
	{
		Titanium.Media.beep();
	},
	
	test_play_sound_as_async: function(callback)
	{
		var sound = Titanium.Media.createSound("app://sound.wav");
		sound.onComplete(function(){
			value_of(sound.isPlaying()).should_be_false();
			callback.passed();
		});

		sound.play();
		setTimeout(function(){
			try
			{
				value_of(sound.isPlaying()).should_be_true();
				sound.pause();
				value_of(sound.isPlaying()).should_be_false();
				value_of(sound.isPaused()).should_be_true();
				value_of(sound.isLooping()).should_be_false();
				sound.play();
			}
			catch (exception)
			{
				callback.failed(exception);
			}
		}, 1000);

		setTimeout(function(){
			callback.failed("sound onComplete timed out");
		}, 10000);
	},
	test_play_sound_via_path_as_async: function(callback)
	{
		var soundPath = Titanium.App.appURLToPath("app://sound.wav");
		var sound = Titanium.Media.createSound(soundPath);
		sound.onComplete(function(){
			value_of(sound.isPlaying()).should_be_false();
			callback.passed();
		});

		sound.play();
		setTimeout(function(){
			try
			{
				value_of(sound.isPlaying()).should_be_true();
				sound.pause();
				value_of(sound.isPlaying()).should_be_false();
				value_of(sound.isPaused()).should_be_true();
				value_of(sound.isLooping()).should_be_false();
				sound.play();
			}
			catch (exception)
			{
				callback.failed(exception);
			}
		}, 1000);

		setTimeout(function(){
			callback.failed("sound onComplete timed out");
		}, 10000);
	},
	test_stop_does_not_call_oncomplete_as_async: function(callback)
	{
		var sound = Titanium.Media.createSound("app://sound.wav");
		sound.onComplete(function()
		{
			value_of(sound.isPlaying()).should_be_false();
			callback.failed("On complete was called");
		});
		sound.play();

		setTimeout(function()
		{
			try
			{
				sound.stop();
			}
			catch (exception)
			{
				callback.failed(exception);
			}
		}, 1000);

		// If three seconds finished and we didn't get an onComplete call, pass.
		setTimeout(function(){ callback.passed(); }, 3000);
	},
	test_play_sound_looping_as_async: function(callback)
	{
		var timesPlayed = 0;
		var sound = Titanium.Media.createSound("app://short_sound.wav");

		sound.setLooping(true);
		sound.onComplete(function()
		{
			timesPlayed = timesPlayed + 1;
			if (timesPlayed > 1)
			{
				sound.stop();
				callback.passed();
			}
		});
		sound.play();
		
		setTimeout(function()
		{
			callback.failed("Timeout waiting for sound to loop");
		}, 4000);
	},
	test_play_sound_volume_as_async: function(callback)
	{
		var sound = Titanium.Media.createSound("app://sound.wav");
		sound.play();

		steps = [
			function() {
				if (!sound.isPlaying())
					throw "Sound did not start";
				sound.setVolume(0.5);
			},
			function() {
				var volume = sound.getVolume();
				if (volume != 0.5)
					throw "Volume should have been 0.5, but was " + volume;
				sound.setVolume(0.25);
			},
			function() {
				var volume = sound.getVolume();
				if (volume != 0.25)
					throw "Volume should have been 0.25, but was " + volume;
				sound.setVolume(-0.25);
			},
			function() {
				var volume = sound.getVolume();
				if (volume != 0)
					throw "Volume should have been 0, but was " + volume;
				sound.setVolume(100);
			},
			function() {
				var volume = sound.getVolume();
				if (volume != 1)
					throw "Volume should have been 1, but was " + volume;
				sound.stop();
				callback.passed();
			}];

		var run_next_test = function() {
			var test = steps.shift();
			try
			{
				test();
				setTimeout(run_next_test, 250);
			}
			catch (e)
			{
				callback.failed(e);
			}
		};
		setTimeout(run_next_test, 250);
	},
	test_sound_destruction_as_async : function(callback)
	{
		// This code at some point caused a crash on OS X because
		// the intro object would be garbage collected before the
		// sound was finished.
		var w = Titanium.UI.createWindow("app://destruction-test.html");
		w.open();
		setTimeout(function() { w.close(); callback.passed() }, 1000);
	},
	test_looping_stop_as_async: function(callback)
	{
		var timesPlayed = 0;
		var sound = Titanium.Media.createSound("app://short_sound.wav");

		sound.setLooping(true);
		sound.onComplete(function()
		{
			timesPlayed = timesPlayed + 1;
			if (timesPlayed > 1)
			{
				sound.stop();
				setTimeout(function() {
					if (sound.isPlaying())
					{
						callback.failed("Sound did not stop playing.");
					}
					else
					{
						callback.passed();
					}
				}, 1000);
			}
		});
		sound.play();

		setTimeout(function()
		{
			callback.failed("Timeout waiting for sound to loop");
		}, 4000);
	},
	issue35_open_sound_crashes_as_async: function(callback)
	{
		var sound = Titanium.Media.createSound('app://sound.wav');
		sound.play();
		
		var timer = setTimeout(function(){
			window.location.href = 'http://en.wikipedia.org/wiki/The_Evil_Dead';
			callback.passed();
		}, 3000);
	},
});
