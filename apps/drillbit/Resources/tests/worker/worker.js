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
	test_worker_as_queue_before_start_as_async: function(result)
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
	test_worker_attach_onmessage_after_start_as_async: function(result)
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
	test_worker_test_multiple_imports_as_async: function(result)
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
	test_worker_test_titanium_as_async: function(result)
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
	test_worker_data_types_as_async: function(result)
	{
		var worker = Titanium.Worker.createWorker('test5.js');

		worker.start();

		var timer = setTimeout(function()
		{
			result.failed("timed out");
			worker.terminate();
		},2000);

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
	},
	test_worker_multiple_queued_items_as_async: function(result)
	{
		var worker = Titanium.Worker.createWorker('test5.js');

		var timer = setTimeout(function()
		{
			result.failed("timed out");
			worker.terminate();
		},2000);

		worker.start();

		worker.postMessage([1,2,3]);
		worker.postMessage([1,2,3]);
		worker.postMessage([1,2,3]);
		worker.postMessage([1,2,3]);
		worker.postMessage([1,2,3]);
		worker.postMessage([1,2,3]);
		worker.postMessage([1,2,3]);
		worker.postMessage([1,2,3]);
		worker.postMessage([1,2,3]);
		worker.postMessage([1,2,3]);

		var count = 0;
		
		worker.onmessage = function(v)
		{
			if (++count == 10)
			{
				clearTimeout(timer);
				worker.terminate();
				result.passed();
			}
		};
	},
	test_worker_multiple_sleep_as_async: function(result)
	{
		var worker = Titanium.Worker.createWorker('test6.js');

		var timer = setTimeout(function()
		{
			result.failed("timed out");
			worker.terminate();
		},4000);
		
		var started = new Date().getTime();

		worker.start();

		worker.onmessage = function(v)
		{
			clearTimeout(timer);
			worker.terminate();
			
			var finished = new Date().getTime();
			if (finished-started >= 2000)
			{
				result.passed();
			}
			else
			{
				result.failed("didn't sleep long enough");
			}
		};
	},
	//test_worker_multiple_sleep_interrupted_as_async: function(result)
	//{
	//	var worker = Titanium.Worker.createWorker('test6.js');
	//	worker.start();
	//	worker.onmessage = function(v)
	//	{
	//		if (v.message == 0)
	//		{
	//			result.failed("terminate didn't cause interrupted exception");
	//		}
	//		else
	//		{
	//			result.passed();
	//		}
	//	};

	//	// give it a bit, then kill it
	//	setTimeout(function()
	//	{
	//		worker.terminate();
	//	}, 500);

	//	setTimeout(function()
	//	{
	//		result.failed("Test timed out.");
	//	}, 2000);
	//}
});
