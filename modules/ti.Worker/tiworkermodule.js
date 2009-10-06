(function()
{
	var createWorker = Titanium.Worker.createWorker;
	
	Titanium.API.addEventListener(Titanium.PAGE_INITIALIZED, function(event)
	{
		if (!event.hasTitaniumObject)
			return;

		var window = event.scope;
		// hook into the createWorker and make sure we pass in the window
		// object as the first parameter

		window.Titanium.Worker.createWorker = function(value)
		{
			return createWorker(String(value),typeof(value)=='function');
		};
	});
	
})();


