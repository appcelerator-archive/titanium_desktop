describe("Properties", function () {
    var properties;

    describe("given a properties object initialized with some values", function () {

        beforeEach(function () {
            properties = Titanium.App.createProperties({
                'double': 1.123456,
                'int': 1,
                'list': ["a", "b", "c"],
                'string': 'abcdef'
            });
        });

        describe("getDouble", function () {
            it("returns a double for the given property", function () {
                expect(properties.getDouble('double')).toEqual(1.123456);
            });
        });

        describe("getInt", function () {
            it("returns an integer for the given property", function () {
                expect(properties.getInt('int')).toEqual(1);
            });
        });

        describe("getList", function () {
            it("returns a list for the given property", function () {
                expect(properties.getList('list')).toEqual(["a", "b", "c"]);
            });
        });

        describe("getString", function () {
            it("returns a string for the given property", function () {
                expect(properties.getString('string')).toEqual('abcdef');
            });
        });

        describe("listProperties", function () {
            it("should return a list of all the property keys", function () {
                expect(properties.listProperties()).toEqual(['double', 'int', 'list', 'string']);
            });
        });

        describe("setDouble", function () {
            it("will set a double value of a property", function () {
                properties.setDouble("new_double", 1.123456);
                expect(properties.getDouble("new_double")).toEqual(1.123456);
            });
        });

        describe("setInt", function () {
            it("will set an integer value of a property", function () {
                properties.setInt("new_int", 1);
                expect(properties.getInt("new_int")).toEqual(1);
            });
        });

        describe("setList", function () {
            it("will set a list value of a property", function () {
                properties.setList("new_list", ["a", "b", "c"]);
                expect(properties.getList("new_list")).toEqual(["a", "b", "c"]);
            });
        });

        describe("setString", function () {
            it("will set a string value of a property", function () {
                properties.setString("new_string", "abcdef");
                expect(properties.getString("new_string")).toEqual("abcdef");
            });
        });
    });

    describe("loading properties from a file", function () {
        beforeEach(function () {
            var path = Drillbit.testDirectory.nativePath() + "/properties.txt";
            properties = Titanium.App.loadProperties(path);
        });

        it("should have a 'int' property with an integer value", function () {
            expect(properties.getInt("int")).toEqual(123);
        });

        it("should have a 'string' property with a string value", function () {
            expect(properties.getString("string")).toEqual("abc def");
        });

        it("should have a 'double' property with a double value", function () {
            expect(properties.getDouble("double")).toEqual(1.123456);
        });

        // TODO: should also test loading a list from file
    });
});
