describe("HTML 5 Databases",{
	
	before:function()
	{
		this.db = openDatabase('unittest', '1.0', "Unit test database", 1024*1024*5);
		
		// test the return object to make sure we got the correct object.
		value_of(this.db).should_be_object();
		
		// add methods to verify that we got the proper object.
	},
	
	after:function()
	{
		this.db.transaction(function(tx)
		{
			tx.executeSql('DROP TABLE TEST');
		});
		this.db = null;
	},
	
	validate_version:function()
	{
		value_of(this.db.version).should_not_be_undefined();
		value_of(this.db.version).should_be('1.0');
	},
	
	test_data_as_async:function(scope)
	{
		function run_test(tx)
		{
			// see if project table exists
			tx.executeSql("CREATE TABLE TEST (name TEXT)", [], function(tx,result) 
			{
				tx.executeSql('INSERT INTO TEST (name) VALUES (?)',['foo'],function(tx,result)
				{
					tx.executeSql('SELECT * FROM TEST',[],function(tx,result)
					{
						scope.passed();
					},
					function(tx,error)
					{
						scope.failed('select failed:'+error.message);
					});
				},
				function(tx, error)
				{
					scope.failed('insert failed:'+error.message);
				})
			}, 
			function(tx, error) 
			{
				scope.failed('create table failed:'+error.message);
			});
		}
		this.db.transaction(function(tx)
		{   
			tx.executeSql('DROP TABLE TEST',null,function(tx,result)
			{
				run_test(tx);
			},
			function(tx,error)
			{
				// this is OK, just means we didn't have the table
				run_test(tx);
			});
		});
	}
});
