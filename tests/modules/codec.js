var testString = "Only two things are infinite, the universe and human stupidity, and I'm not sure about the former. -- Albert Einstein";
    base64Encoded = "T25seSB0d28gdGhpbmdzIGFyZSBpbmZpbml0ZSwgdGhlIHVuaXZlcnNlIGFuZCBodW1hbiBzdHVwaWRpdHksIGFuZCBJJ20gbm90IHN1cmUgYWJvdXQgdGhlIGZvcm1lci4gLS0gQWxiZXJ0IEVpbnN0ZWlu";
    hexEncoded = "4f6e6c792074776f207468696e67732061726520696e66696e6974652c2074686520756e69766572736520616e642068756d616e207374757069646974792c20616e642049276d206e6f7420737572652061626f75742074686520666f726d65722e202d2d20416c626572742045696e737465696e";

describe("checksum", function () {
    it("returns the correct checksum using the default algorithm", function () {
        expect(Titanium.Codec.checksum(testString)).toEqual("B2B0664C");
    });

    it("returns the correct checksum using crc32 alogrithm", function () {
        expect(Titanium.Codec.checksum(testString, Titanium.Codec.CRC32)).toEqual(2997904972);
    });

    it("returns the correct checksum using ADLER32 algorithm", function () {
        expect(Titanium.Codec.checksum(testString, Titanium.Codec.ADLER32)).toEqual(3417123301);
    });
});

xdescribe("createZip", function () {
    // TODO
});

describe("decodeBase64", function () {
    // FIXME: appears our impl is putting newlines in the hash causing these tests to fail.
    it("returns the original string that was base 64 encoded", function () {
        expect(Titanium.Codec.decodeBase64(base64Encoded)).toEqual(testString);
    });
});

describe("encodeBase64", function () {
    it("returns the base 64 encoded string", function () {
        expect(Titanium.Codec.encodeBase64(testString)).toEqual(base64Encoded);
    });
});

describe("decodeHexBinary", function () {
    it("returns the original string that was hex encoded", function () {
        expect(Titanium.Codec.decodeHexBinary(hexEncoded)).toEqual(testString);
    });
});

describe("encodeHexBinary", function () {
    it("returns the hex encoded string", function () {
        expect(Titanium.Codec.encodeHexBinary(testString)).toEqual(hexEncoded);
    });
});
