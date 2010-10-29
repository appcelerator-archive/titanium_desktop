describe("appURLToPath", function () {
    var appURLPath;

    beforeEach(function () {
        appURLPath = Titanium.App.appURLToPath("app://index.html");
    });

    it("return a valid absolute file path for an app URL", function () {
        var file = Titanium.Filesystem.getFile(appURLPath);
        expect(file.exists()).toBe(true);
    });

    it("returns a valid path even if the URL has a host in it", function () {
        // See Lighthouse ticket #90 for more details.
        var appURLWithHost = Titanium.UI.getCurrentWindow().getURL(),
            appURLPathWithHost = Titanium.App.appURLToPath(appURLWithHost);
        expect(appURLPathWithHost).toEqual(appURLPath);
    });
});

// TODO: implement a test for the exit() function.
xdescribe("exit", function () {});

describe("getArguments", function () {
    it("should return two arguments", function () {
        expect(Titanium.App.getArguments().length).toBe(2);
    });
});

describe("getCopyright", function () {
    it("returns the copyright specified in the tiapp.xml file", function () {
        expect(Titanium.App.getCopyright()).toEqual("2010 by Appcelerator");
    });
});

describe("getDescription", function () {
    it("returns the description specified in the tiapp.xml file", function () {
        expect(Titanium.App.getDescription()).toEqual("A testing harness for Titanium");
    });
});