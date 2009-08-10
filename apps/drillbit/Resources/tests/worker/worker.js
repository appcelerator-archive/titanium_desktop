describe("async worker tests",
{
	test_worker_as_async: function(result)
	{
		var worker = Titanium.Worker.createWorker(function()
		{
			postMessage("123");
		});

		var timer = setTimeout(function()
		{
			result.failed("timed out");
			worker.terminate();
		},2000);
		
		worker.onmessage = function(v)
		{
			clearTimeout(timer);
			worker.terminate();
			try
			{
				value_of(v.message).should_be('123');
				result.passed();
			}
			catch(e)
			{
				result.failed(e);
			}
		};
		
		worker.start();
	},

	test_worker_with_titanium_as_async: function(result)
	{
		var version = Titanium.version;
		
		var worker = Titanium.Worker.createWorker(function()
		{
			postMessage(Titanium.version);
		});
		
		worker.onmessage = function(v)
		{
			clearTimeout(timer);
			worker.terminate();
			try
			{
				value_of(v.message).should_be(version);
				result.passed();
			}
			catch(e)
			{
				result.failed(e);
			}
		};
		
		worker.start();
		
		var timer = setTimeout(function()
		{
			result.failed("timed out");
			worker.terminate();
		},2000);
		
	},
	test_worker_as_external_file_as_async: function(result)
	{
		var worker = Titanium.Worker.createWorker('test.js');
		
		worker.onmessage = function(v)
		{
			clearTimeout(timer);
			worker.terminate();
			try
			{
				value_of(v.message).should_be('you said: 123');
				result.passed();
			}
			catch(e)
			{
				result.failed(e);
			}
		};
		
		worker.start();
		
		var timer = setTimeout(function()
		{
			result.failed("timed out");
			worker.terminate();
		},2000);
		
		worker.postMessage("123");
	},
	test_worker_as_external_file_with_import_as_async: function(result)
	{
		var worker = Titanium.Worker.createWorker('test2.js');

		var timer = setTimeout(function()
		{
			result.failed("timed out");
			worker.terminate();
		},2000);

		worker.onmessage = function(v)
		{
			clearTimeout(timer);
			worker.terminate();
			try
			{
				value_of(v).should_be_object();
				value_of(v.message).should_be('you said: 123');
				result.passed();
			}
			catch(e)
			{
				result.failed(e);
			}
		};
		
		worker.start();
		worker.postMessage("456");
	},
	test_worker_as_queue_before_start: function(result)
	{
		var worker = Titanium.Worker.createWorker('test2.js');

		var timer = setTimeout(function()
		{
			result.failed("timed out");
			worker.terminate();
		},2000);

		worker.onmessage = function(v)
		{
			clearTimeout(timer);
			worker.terminate();
			try
			{
				value_of(v).should_be_object();
				value_of(v.message).should_be('you said: 123');
				result.passed();
			}
			catch(e)
			{
				result.failed(e);
			}
		};
		
		// queue before start
		worker.postMessage("456");
		
		worker.start();
	},
	test_worker_attach_onmessage_after_start: function(result)
	{
		var worker = Titanium.Worker.createWorker('test2.js');

		var timer = setTimeout(function()
		{
			result.failed("timed out");
			worker.terminate();
		},2000);

		worker.start();

		worker.onmessage = function(v)
		{
			clearTimeout(timer);
			worker.terminate();
			try
			{
				value_of(v).should_be_object();
				value_of(v.message).should_be('you said: 123');
				result.passed();
			}
			catch(e)
			{
				result.failed(e);
			}
		};

		worker.postMessage("456");
	},
	test_worker_test_multiple_imports: function(result)
	{
		var worker = Titanium.Worker.createWorker('test3.js');

		var timer = setTimeout(function()
		{
			result.failed("timed out");
			worker.terminate();
		},2000);

		worker.start();

		worker.postMessage("456");

		worker.onmessage = function(v)
		{
			clearTimeout(timer);
			worker.terminate();
			try
			{
				value_of(v).should_be_object();
				value_of(v.message).should_be('you said: 123');
				result.passed();
			}
			catch(e)
			{
				result.failed(e);
			}
		};
	},
	test_worker_test_titanium: function(result)
	{
		var worker = Titanium.Worker.createWorker('test4.js');

		var timer = setTimeout(function()
		{
			result.failed("timed out");
			worker.terminate();
		},2000);

		worker.start();

		worker.postMessage("1");

		worker.onmessage = function(v)
		{
			clearTimeout(timer);
			worker.terminate();
			try
			{
				value_of(v).should_be_object();
				value_of(v.message).should_be(Titanium.version);
				result.passed();
			}
			catch(e)
			{
				result.failed(e);
			}
		};
	},
	test_worker_data_types: function(result)
	{
		var worker = Titanium.Worker.createWorker('test5.js');

		var timer = setTimeout(function()
		{
			result.failed("timed out");
			worker.terminate();
		},2000);

		worker.start();

		worker.postMessage([1,2,3]);

		worker.onmessage = function(v)
		{
			clearTimeout(timer);
			worker.terminate();
			try
			{
				value_of(v).should_be_object();
				value_of(v.message).should_be_object();
				value_of(v.message[0]).should_be('object');
				value_of(v.message[1]).should_be(2);
				result.passed();
			}
			catch(e)
			{
				result.failed(e);
			}
		};
	}
});
