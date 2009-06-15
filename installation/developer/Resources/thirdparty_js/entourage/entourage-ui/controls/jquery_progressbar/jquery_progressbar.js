App.UI.registerUIComponent('control','jquery_progressbar',
{
	create: function()
	{
		this.options = null;
		this.id = null;

		/**
		 * The version of the control. This will automatically be corrected when you
		 * publish the component.
		 */
		this.getVersion = function()
		{
			// leave this as-is and only configure from the build.yml file 
			// and this will automatically get replaced on build of your distro
			return '1.0';
		}
		
		/**
		 * The control spec version.  This is used to maintain backwards compatability as the
		 * Widget API needs to change.
		 */
		this.getSpecVersion = function()
		{
			return 1.0;
		}

		this.getAttributes = function()
		{
			return [{name: "value", 
						optional: true, 
						description: "Value of the progressbar.",
						defaultValue: 0
					},
					{name: "change", 
						optional: true, 
						description: "Callback for a change in the value"
					}];
		}
		
		this.enable = function(value)
		{
			jQuery("#" + this.id).progressbar("enable");
		}
		
		this.disable = function(value)
		{
			jQuery("#" + this.id).progressbar("disable");
		}
		
		this.value = function(value)
		{
			jQuery("#" + this.id).progressbar("value", parseInt(App.getActionValue(value, 'value')));
		}
				
		this.getActions = function()
		{
			return ['value','enable','disable'];
		}

		this.build = function(element,options)
		{
			this.options = options;
			this.id = element.id;
			
	        jQuery("#" + element.id).progressbar(options);
			
			if(options["value"])
			{
				jQuery("#" + element.id).progressbar("value", parseInt(options['value']));
			}
			
		}
		
		this.getControlCSS = function() {
		  return ['../../common/css/jquery-themes/ui.all.css']
		};
	}
});
