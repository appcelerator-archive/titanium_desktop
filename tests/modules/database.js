describe("Open an existing database from file", function () {
    var database;

    beforeEach(function () {
        database = Titanium.Database.openFile(Drillbit.testDirectory.nativePath() + '/db.sqlite');
    });

    afterEach(function () {
        database.close();
    });

    it("successfully opened the database", function () {
        expect(database).toBeDefined();
    });

    describe("execute a query to select rows from a table", function () {
        var resultSet;

        beforeEach(function () {
            resultSet = database.execute("SELECT * FROM People WHERE gender=?", 'm');
        });

        it("returns the correct number of rows", function () {
            expect(resultSet.rowCount()).toEqual(4); 
        });

        it("has a first row which is valid", function () {
            expect(resultSet.isValidRow()).toBe(true);
        });

        it("should have a row with the correct field count", function () {
            expect(resultSet.fieldCount()).toEqual(4);
        });

        it("properly iterates through the rows", function () {
            var count = 0;
            while (resultSet.isValidRow()) {
                count++;
                resultSet.next();
            }
            expect(count).toEqual(4);
        });
    });
});
