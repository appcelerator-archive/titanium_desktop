describe("ti.Platform tests", {
	test_interfaces: function() {
		var interfaces = Titanium.Platform.interfaces;
		value_of(Titanium.Platform.interfaces).should_be_array();
		value_of(Titanium.Platform.interfaces.length > 0).should_be_true();
		
		for (var i = 0; i < Titanium.Platform.interfaces.length; i++) {
			var iface = Titanium.Platform.interfaces[i];
			value_of(iface.name).should_be_string();
			value_of(iface.address).should_be_string();
			value_of(iface.displayName).should_be_string();
		}
	},
	
	test_platform_properties: function() {
		value_of(Titanium.Platform.name).should_be_string();
		value_of(Titanium.Platform.ostype).should_be_string();
		value_of(Titanium.Platform.version).should_be_string();
		value_of(Titanium.Platform.architecture).should_be_string();
		value_of(Titanium.Platform.address).should_be_string();
		value_of(Titanium.Platform.id).should_be_string();
		value_of(Titanium.Platform.macaddress).should_be_string();
		value_of(Titanium.Platform.processorCount).should_be_number();
		value_of(Titanium.Platform.username).should_be_string();
		value_of(Titanium.Platform.createUUID).should_be_function();
	},
	
	test_unique_uuid: function() {
		var uuids = [];
		for (var i = 0; i < 100; i++) {
			var uuid = Titanium.Platform.createUUID();
			value_of(uuid in uuids).should_be_false();
			uuids.push(uuid);
		}
	}
});