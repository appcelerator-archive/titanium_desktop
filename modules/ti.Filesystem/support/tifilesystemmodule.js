(function()
{
    function patchFile(file)
    {
        /**
            Append the read, readLine, and write methods for
            backward compatibility.
         */
        file.read = function()
        {
            var fs = this.open(Titanium.Filesystem.MODE_READ);
            var content = fs.read();
            fs.close();
            return content;
        }
        file.readLine = function()
        {
            if (this._lineReader == undefined)
            {
                this._lineReader = this.open(Titanium.Filesystem.MODE_READ);
            }
            var content = this._lineReader.readLine();
            if (content == null)
            {
                this._lineReader.close();
                this._lineReader = undefined;
            }
            return content;
        }
        file.write = function(data)
        {
            var fs = this.open(Titanium.Filesystem.MODE_WRITE);
            var result = fs.write(data);
            fs.close();
            return result;
        }

		return file;
    }

    var getFile = Titanium.Filesystem.getFile; 
    Titanium.Filesystem.getFile = function()
    {
        var file = getFile.apply(this, arguments);
        return patchFile(file);
    }

    var createTempFile = Titanium.Filesystem.createTempFile;
    Titanium.Filesystem.createTempFile = function()
    {
        var file = createTempFile.apply(this, arguments);
        return patchFile(file);
    }
})();
