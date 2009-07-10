describe("UI.Dialog",
{
	create_dialog_with_no_content_as_async:function(callback)
	{
		value_of(Titanium.UI.showDialog).should_be_function();
		var dialog = Titanium.UI.showDialog({
			'url':'app://dialog_no_content.html'
		});
		value_of(dialog).should_be_object();
		value_of(dialog.close).should_be_function();

		value_of(dialog.getDialogParameter('in')).should_be_undefined();
		value_of(dialog.getDialogParameter('in2')).should_be_undefined();
		value_of(dialog.getDialogParameter('in3')).should_be_undefined();
		value_of(dialog.getDialogParameter('in4')).should_be_undefined();

		value_of(dialog.getDialogParameter('in2', 42)).should_be(42);
		value_of(dialog.getDialogParameter('in3', 42)).should_be(42);
		value_of(dialog.getDialogParameter('in4', 42)).should_be(42);

		value_of(dialog.getDialogParameter('unknown')).should_be_undefined;
		value_of(dialog.getDialogParameter('unknown', 'alt')).should_be('alt');
		setTimeout(function()
		{
			try
			{
				value_of(dialog.getDialogResult()).should_be_null();
				callback.passed();
			}
			catch(e)
			{
				callback.failed(e);
			}
			dialog.close();
		},1000);
	},

	create_dialog_return_result_as_async:function(callback)
	{
		var timer = null;
		value_of(Titanium.UI.showDialog).should_be_function();
		var dialog = Titanium.UI.showDialog({
			'url':'app://dialog_return_result.html',
			'parameters':{
				'in':'out',
				'in2':42,
				'in3':-100,
				'in4': {'in4.1':'blarg', 'in4.2':42}
			},
			'onclose':function(result)
			{
				clearTimeout(timer);
				try
				{
					value_of(result).should_be(dialog.getDialogResult());
					value_of(result).should_not_be_null();
					value_of(result).should_be_object();
					value_of(result.a).should_be('b');
					value_of(result.out).should_be('out');
					callback.passed();
				}
				catch(e)
				{
					callback.failed(e);
				}
			}
		});
		value_of(dialog).should_be_object();
		value_of(dialog.close).should_be_function();

		value_of(dialog.getDialogParameter('in')).should_be('out');
		value_of(dialog.getDialogParameter('in2')).should_be(42);
		value_of(dialog.getDialogParameter('in3')).should_be(-100);
		value_of(dialog.getDialogParameter('in4')).should_be_object();
		value_of(dialog.getDialogParameter('in4')['in4.1']).should_be('blarg');
		value_of(dialog.getDialogParameter('in4')['in4.2']).should_be(42);

		value_of(dialog.getDialogParameter('in', 'alt')).should_be('out');
		value_of(dialog.getDialogParameter('in2', 'alt')).should_be(42);
		value_of(dialog.getDialogParameter('in3', 'alt')).should_be(-100);
		value_of(dialog.getDialogParameter('in4', 'alt')).should_be_object();
		value_of(dialog.getDialogParameter('in4', 'alt')['in4.1']).should_be('blarg');
		value_of(dialog.getDialogParameter('in4', 'alt')['in4.2']).should_be(42);

		value_of(dialog.getDialogParameter('unknown')).should_be_undefined;
		value_of(dialog.getDialogParameter('unknown', 'alt')).should_be('alt');

		timer = setTimeout(function()
		{
			callback.failed('dialog timed out, should have closed by now');
			dialog.close();
		},5000);
	}
});
