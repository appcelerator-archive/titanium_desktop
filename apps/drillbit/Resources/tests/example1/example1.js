describe("this is a test example",{
	
	timeout: 10000,
	
	before:function()
	{
		this.a = 'a';
	},
	
	after:function()
	{
		delete this.a;
	},
	
	execute_as_async:function(callback)
	{
		value_of(this.a).should_be('a');
		value_of(this.b).should_be_undefined();
		setTimeout(function()
		{
			callback.passed();
		},2000);
	}
});