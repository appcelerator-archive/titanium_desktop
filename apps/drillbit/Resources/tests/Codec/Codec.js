describe("Codec Tests",{
	
	test_base64_encode: function()
	{
		value_of(Titanium.Codec).should_be_object();
		value_of(Titanium.Codec.encodeBase64("abc")).should_be("YWJj");
	},

	test_base64_decode: function()
	{
		value_of(Titanium.Codec.decodeBase64("YWJj")).should_be("abc");
	},
	
	test_md2: function()
	{
		value_of(Titanium.Codec.digestToHex(Titanium.Codec.MD2,"abc")).should_be("da853b0d3f88d99b30283a69e6ded6bb");
	},

	test_md4: function()
	{
		value_of(Titanium.Codec.digestToHex(Titanium.Codec.MD4,"abc")).should_be("a448017aaf21d8525fc10ae87aa6729d");
	},

	test_md5: function()
	{
		value_of(Titanium.Codec.digestToHex(Titanium.Codec.MD5,"abc")).should_be("900150983cd24fb0d6963f7d28e17f72");
	},

	test_sha1: function()
	{
		value_of(Titanium.Codec.digestToHex(Titanium.Codec.SHA1,"abc")).should_be("a9993e364706816aba3e25717850c26c9cd0d89d");
	},
	
	test_hmac_md5: function()
	{
		value_of(Titanium.Codec.digestHMACToHex(Titanium.Codec.MD5,"abc","123")).should_be("ffb7c0fc166f7ca075dfa04d59aed232");
	},

	test_hmac_sha1: function()
	{
		value_of(Titanium.Codec.digestHMACToHex(Titanium.Codec.SHA1,"abc","123")).should_be("540b0c53d4925837bd92b3f71abe7a9d70b676c4");
	},

	test_encode_hex_binary: function()
	{
		value_of(Titanium.Codec.encodeHexBinary("ABCDEF")).should_be("414243444546");
	},

	test_decode_hex_binary: function()
	{
		value_of(Titanium.Codec.decodeHexBinary("414243444546")).should_be("ABCDEF");
	},
	
	test_checksum_crc32: function()
	{
		value_of(Titanium.Codec.checksum("abc")).should_be(891568578);
		value_of(Titanium.Codec.checksum("abc",Titanium.Codec.CRC32)).should_be(891568578);
		var blob = Titanium.API.createBlob("abc");
		value_of(Titanium.Codec.checksum(blob,Titanium.Codec.CRC32)).should_be(891568578);
	},
	
	test_checksum_adler32: function()
	{
		value_of(Titanium.Codec.checksum("abc",Titanium.Codec.ADLER32)).should_be(38600999);

		var blob = Titanium.API.createBlob("abc");
		value_of(Titanium.Codec.checksum(blob,Titanium.Codec.ADLER32)).should_be(38600999);
	}
	
});