App.UI.registerUIComponent('control','jquery_datepicker',
{
	create: function()
	{
		this.options = null;
		this.id = null;
		this.defaultValues = {
      altField: '',
      altFormat: '',
      appendText: '',
      buttonImage: '',
      buttonImageOnly: false,
      buttonText: '...',
      calculateWeek: jQuery.datepicker.iso8601Week,
      changeFirstDay: true,
      changeMonth: true,
      changeYear: true,
      closeText: 'Close',
      constrainInput: true,
      currentText: 'Today',
      dateFormat: 'mm/dd/yy',
      dayNames: ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'],
      dayNamesMin: ['Su', 'Mo', 'Tu', 'We', 'Th', 'Fr', 'Sa'],
      dayNamesShort: ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'],
      defaultDate: null,
      duration: 'normal',
      firstDay: 0,
      gotoCurrent: false,
      hideIfNoPrevNext: false,
      isRTL: false,
      maxDate: null,
      minDate: null,
      monthNames: ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'],
      monthNamesShort: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'],
      navigationAsDateFormat: false,
      nextText: 'Next>',
      numberOfMonths: 1,
      prevText: '<Prev',
      shortYearCutoff: '+10',
      showAnim: 'show',
      showButtonPanel: false,
      showOn: 'focus',
      showOptions: {},
      showOtherMonths: false,
      stepMonths: 1,
      yearRange: '-10:+10'
		};

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
			return [{
        name: 'altField',
        defaultValue: '',
        description: "The jQuery selector for another field that is to be updated with the selected date from the datepicker. Use the altFormat setting below to change the format of the date within this field. Leave as blank for no alternate field.",
        optional: true
      },
      {
        name: 'altFormat',
        defaultValue: '',
        description: "The dateFormat to be used for the altField option. This allows one date format to be shown to the user for selection purposes, while a different format is actually sent behind the scenes.",
        optional: true
      },
      {
        name: 'appendText',
        defaultValue: '',
        descrption: "The text to display after each date field, e.g. to show the required format.",
        optional: true
      },
      {
        name: 'buttonImage',
        defaultValue: '',
        description: "The URL for the popup button image. If set, button text becomes the alt value and is not directly displayed.",
        optional: true
      },
      {
        name: 'buttonImageOnly',
        defaultValue: false,
        description: "Set to true to place an image after the field to use as the trigger without it appearing on a button.",
        optional: true
      },
      {
        name: 'buttonText',
        defaultValue: '...',
        description: "The text to display on the trigger button. Use in conjunction with showOn equal to 'button' or 'both'.",
        optional: true
      },
      {
        name: 'calculateWeek',
        defaultValue: jQuery.datepicker.iso8601Week,
        description: "Perform the week of the year calculation. This function accepts a Date as a parameter and returns the number of the corresponding week of the year. The default implementation uses the ISO 8601 definition of a week: weeks start on a Monday and the first week of the year contains January 4. This means that up to three days from the previous year may be included in the first week of the current year, and that up to three days from the current year may be included in the last week of the previous year.",
        optional: true
      },
      {
        name: 'changeFirstDay',
        defaultValue: true,
        description: "Allows you to click on the day names to have the week start on that day. You can disable this feature by setting the attribute to false.",
        optional: true
      },
      {
        name: 'changeMonth',
        defaultValue: true,
        description: "Allows you to change the month by selecting from a drop-down list. You can disable this feature by setting the attribute to false.",
        optional: true
      },
      {
        name: 'changeYear',
        defaultValue: true,
        description: "Allows you to change the year by selecting from a drop-down list. You can disable this feature by setting the attribute to false.",
        optional: true
      },
      {
        name: 'closeText',
        defaultValue: 'Close',
        description: "The text to display for the close link. This attribute is one of the regionalisation attributes.",
        optional: true
      },
      {
        name: 'constrainInput',
        defaultValue: true,
        description: "True if the input field is constrained to the current date format.",
        optional: true
      },
      {
        name: 'currentText',
        defaultValue: 'Today',
        description: "The text to display for the current day link. This attribute is one of the regionalisation attributes.",
        optional: true
      },
      {
        name: 'dateFormat',
        defaultValue: 'mm/dd/yy',
        description: "The format for parsed and displayed dates. This attribute is one of the regionalisation attributes. For a full list of the possible formats see the formatDate function.",
        optional: true
      },
      {
        name: 'dayNames',
        defaultValue: ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'],
        description: "The list of long day names, starting from Sunday, for use as requested via the dateFormat setting. They also appear as popup hints when hovering over the corresponding column headings. This attribute is one of the regionalisation attributes.",
        optional: true
      },
      {
        name: 'dayNamesMin',
        defaultValue: ['Su', 'Mo', 'Tu', 'We', 'Th', 'Fr', 'Sa'],
        description: "The list of minimised day names, starting from Sunday, for use as column headers within the datepicker. This attribute is one of the regionalisation attributes.",
        optional: true
      },
      {
        name: 'dayNamesShort',
        defaultValue: ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'],
        description: "The list of abbreviated day names, starting from Sunday, for use as requested via the dateFormat setting. This attribute is one of the regionalisation attributes.",
        optional: true
      },
      {
        name: 'defaultDate',
        defaultValue: null,
        description: "Set the date to display on first opening if the field is blank. Specify either an actual date via a Date object, or relative to today with a number (e.g. +7) or a string of values and periods ('y' for years, 'm' for months, 'w' for weeks, 'd' for days, e.g. '+1m +7d'), or null for today.",
        optional: true
      },
      {
        name: 'duration',
        defaultValue: 'normal',
        description: "Control the speed at which the datepicker appears, it may be a time in milliseconds, a string representing one of the three predefined speeds (\"slow\", \"normal\", \"fast\"), or '' for immediately.",
        optional: true
      },
      {
        name: 'firstDay',
        defaultValue: 0,
        description: "Set the first day of the week: Sunday is 0, Monday is 1, ... This attribute is one of the regionalisation attributes.",
        optional: true
      },
      {
        name: 'gotoCurrent',
        defaultValue: false,
        description: "If true, the current day link moves to the currently selected date instead of today.",
        optional: true
      },
      {
        name: 'hideIfNoPrevNext',
        defaultValue: false,
        description: "Normally the previous and next links are disabled when not applicable (see minDate/maxDate). You can hide them altogether by setting this attribute to true.",
        optional: true
      },
      {
        name: 'isRTL',
        defaultValue: false,
        description: "True if the current language is drawn from right to left. This attribute is one of the regionalisation attributes.",
        optional: true
      },
      {
        name: 'maxDate',
        defaultValue: null,
        description: "Set a maximum selectable date via a Date object, or relative to today with a number (e.g. +7) or a string of values and periods ('y' for years, 'm' for months, 'w' for weeks, 'd' for days, e.g. '+1m +1w'), or null for no limit.",
        optional: true
      },
      {
        name: 'minDate',
        defaultValue: null,
        description: "Set a minimum selectable date via a Date object, or relative to today with a number (e.g. +7) or a string of values and periods ('y' for years, 'm' for months, 'w' for weeks, 'd' for days, e.g. '-1y -1m'), or null for no limit.",
        optional: true
      },
      {
        name: 'monthNames',
        defaultValue: ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'],
        description: "The list of full month names, as used in the month header on each datepicker and as requested via the dateFormat setting. This attribute is one of the regionalisation attributes.",
        optional: true
      },
      {
        name: 'monthNamesShort',
        defaultValue: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'],
        description: "The list of abbreviated month names, for use as requested via the dateFormat setting. This attribute is one of the regionalisation attributes.",
        optional: true
      },
      {
        name: 'navigationAsDateFormat',
        defaultValue: false,
        description: "When true the formatDate function is applied to the prevText, nextText, and currentText values before display, allowing them to display the target month names for example.",
        optional: true
      },
      {
        name: 'nextText',
        defaultValue: 'Next>',
        description: "The text to display for the next month link. This attribute is one of the regionalisation attributes.",
        optional: true
      },
      {
        name: 'numberOfMonths',
        defaultValue: 1,
        description: "Set how many months to show at once. The value can be a straight integer, or can be a two-element array to define the number of rows and columns to display.",
        optional: true
      },
      {
        name: 'prevText',
        defaultValue: '<Prev',
        description: "The text to display for the previous month link. This attribute is one of the regionalisation attributes.",
        optional: true
      },
      {
        name: 'shortYearCutoff',
        defaultValue: '+10',
        description: "Set the cutoff year for determining the century for a date (used in conjunction with dateFormat 'y'). If a numeric value (0-99) is provided then this value is used directly. If a string value is provided then it is converted to a number and added to the current year. Once the cutoff year is calculated, any dates entered with a year value less than or equal to it are considered to be in the current century, while those greater than it are deemed to be in the previous century.",
        optional: true
      },
      {
        name: 'showAnim',
        defaultValue: 'show',
        description: "Set the name of the animation used to show/hide the datepicker. Use 'show' (the default), 'slideDown', 'fadeIn', or any of the show/hide jQuery UI effects.",
        optional: true
      },
      {
        name: 'showButtonPanel',
        defaultValue: false,
        description: "Whether to show the button panel.",
        optional: true
      },
      {
        name: 'showOn',
        defaultValue: 'focus',
        description: "Have the datepicker appear automatically when the field receives focus ('focus'), appear only when a button is clicked ('button'), or appear when either event takes place ('both').",
        optional: true
      },
      {
        name: 'showOptions',
        defaultValue: {},
        description: "If using one of the jQuery UI effects for showAnim, you can provide additional settings for that animation via this option.",
        optional: true
      },
      {
        name: 'showOtherMonths',
        defaultValue: false,
        description: "Display dates in other months (non-selectable) at the start or end of the current month.",
        optional: true
      },
      {
        name: 'stepMonths',
        defaultValue: 1,
        description: "Set how many months to move when clicking the Previous/Next links.",
        optional: true
      },
      {
        name: 'yearRange',
        defaultValue: '-10:+10',
        description: "Control the range of years displayed in the year drop-down: either relative to current year (-nn:+nn) or absolute (nnnn:nnnn).",
        optional: true
      }];
		}


		this.enable = function(value)
		{
			jQuery("#" + this.id).datepicker("enable");
		}
		
		this.disable = function(value)
		{
			jQuery("#" + this.id).datepicker("disable");
		}
		
		this.isDisabled = function(value)
		{
			return jQuery("#" + this.id).datepicker("isDisabled");
		}
		
		
		this.hide = function(value)
		{
			if(App.getActionValue(value,'speed'))
			{
				jQuery("#" + this.id).datepicker("hide", App.getActionValue(value,'speed'));
			}
			else
			{
				jQuery("#" + this.id).datepicker("hide");
			}
		}
				
		this.show = function(value)
		{
			jQuery("#" + this.id).datepicker("show");
		}
		
		this.getDate = function(value)
		{
			return jQuery("#" + this.id).datepicker("getDate");
		}
		
		this.setDate = function(value)
		{
			var date = typeof(App.getActionValue(value,'date')) === "string" ? new Date(Date.parse(App.getActionValue(value,'date'))) : App.getActionValue(value,'date');
			var endDate = typeof(App.getActionValue(value,'endDate')) === "string" ? new Date(Date.parse(App.getActionValue(value,'endDate'))) : App.getActionValue(value,'endDate');

			if(endDate)
			{
				jQuery("#" + this.id).datepicker("setDate", date, endDate);
			}
			else
			{
				
				jQuery("#" + this.id).datepicker("setDate", date);
			}
		}

		this.getActions = function()
		{
			return ['enable','disable','isDisabled','hide','show','setDate','getDate'];
		}

		this.build = function(element,options)
		{
			this.options = options;
			this.id = element.id;

			for (v in options)
			{
  			if (this.defaultValues[v] === options[v])
  			{
  				delete options[v];
  			}
      }
	    jQuery("#" + element.id).datepicker(options);			
		}
		
		this.getControlCSS = function() {
		  return ['../../common/css/jquery-themes/ui.all.css']
		};
	}
});
