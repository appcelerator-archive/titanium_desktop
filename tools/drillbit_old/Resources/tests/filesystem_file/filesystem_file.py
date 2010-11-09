import tempfile
import os
import random
import string

def make_large_file():
	filepath = os.path.join(tempfile.gettempdir(), 'drillbit-large-file.txt');
	ascii = string.ascii_letters

	out = ''
	for x in xrange(1, 2 * 1024 * 1024): # 2 megabytes or so
		out += random.choice(ascii)
	out += '\n'

	# Open in binary mode so that '\n' doesn't produce a
	# carriage return on Windows.
	f = open(filepath, 'wb')
	f.write(out)
	f.close()

	return (filepath, out)
