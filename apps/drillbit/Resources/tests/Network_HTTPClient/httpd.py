import BaseHTTPServer
from Cookie import SimpleCookie
import base64
import time
import cgi

reply = "I got it!"
error = "bad data!"

class MyHandler(BaseHTTPServer.BaseHTTPRequestHandler):

	def error(self, message, status=400):
		self.send_response(status)
		self.wfile.write(message)
		raise Exception('Error %s: %s' % (status, message))

	# Send some text as the response
	def send_text(self, text='here is some text for you!'):
		self.send_response(200)
		self.send_header("Content-type", "text/plain")
		self.send_header("Content-Length", len(text))
		self.end_headers()
		self.wfile.write(text)

	# Long request -- delay the response
	def long_request(self):
		print "Starting long request..."
		time.sleep(5.0)
		print "Done with long request sending response..."
		self.send_text()

	# Issue a 301 redirect
	def redirect_301(self):
		print "Redirecting..."
		self.send_response(301)
		self.send_header("Location", "http://127.0.0.1:8888/")
		self.end_headers()

	# Send a cookie
	def send_cookie(self):
		print "Receiving a cookie..."
		try:
			cookies = SimpleCookie(self.headers['Cookie'])
			value = cookies['peanutbutter'].value
			if value == 'yummy':
				self.send_text('got the cookie!')
			else:
				self.error('Invalid cookie: value=%s' % value)
		except Exception, e:
			self.error('Error while parsing cookie header: %s' % e)

	# Receive a cookie from client
	def recv_cookie(self):
		print "Sending a cookie..."
		self.send_response(200)
		self.send_header('Set-Cookie', 'chocolatechip=tasty')
		self.end_headers()

	# Receive headers from client
	def recv_headers(self):
		print 'Receiving headers...'
		if self.headers.get("Foo") != "Bar" or \
				self.headers.get("Hello") != "World" or \
				self.headers.get("Head") != "Tail":
			self.error('Invalid headers: %s' % self.headers)
		self.send_text('Got the headers!')

	# Send headers to client
	def send_headers(self):
		print 'Sending headers...'
		self.send_response(200)
		self.send_header('Foo', 'Bar')
		self.send_header('Head', 'Tail')
		self.end_headers()

	def send_continue_headers(self):
		print 'Sending headers...'
		# Work-around for http://bugs.python.org/issue1491
		# Just send the raw header directly.
		self.wfile.write("HTTP/1.1 100 Continue\r\n\r\n")
		self.send_response(200)
		self.send_header('Foo', 'Bar')
		self.send_header('Head', 'Tail')
		self.end_headers()

	# Verify basic auth credentials
	def basic_auth(self):
		print "Basic auth..."
		basic_auth = self.headers['Authorization']
		if basic_auth.startswith('Basic') is False:
			self.error('Not basic auth header')
		up = base64.decodestring(basic_auth.replace('Basic', '').strip())
		username, password = up.split(':')
		if username == 'test' and password == 'password':
			print 'Valid: username=%s password=%s' % (username, password)
			self.send_text('authorized')
		else:
			self.error('Invalid username/password: %s:%s' % (username, password))

	def recv_post_data(self):
		print 'Receiving post data...'
		if self.command != 'POST':
			self.error('Not a POST request')
		if self.headers.has_key('content-length') is False:
			self.error('Missing content length')
		length = int( self.headers['content-length'] )
		data = self.rfile.read(length)
		expected_data = 'here is some text for you!'
		print "Got data: '%s' length=%i expected: '%s' length=%i " %  \
			(data, len(data), expected_data, len(expected_data))
		if data == expected_data:
			self.send_text('I got it!')
		else:
			self.error('Invalid data: \'%s\'' % data)

	def recv_post_parameters(self):
		print 'Receiving post data...'
		if self.command != 'POST':
			self.error('Not a POST request')
		if self.headers.has_key('content-length') is False:
			self.error('Missing content length')
		length = int( self.headers['content-length'] )

		ctype, pdict = cgi.parse_header(self.headers.getheader('content-type'))
		query = cgi.parse_multipart(self.rfile, pdict)

		if query.get('one')[0] != 'flippityflop':
			self.error('Invalid data for "one": \'%s\'' % query.get['one'])
		elif query.get('two')[0] != 'bloopityblop':
			self.error('Invalid data for "two": \'%s\'' % query.get['two'])
		elif query.get('three')[0] != '':
			self.error('Invalid data for "three": \'%s\'' % query.get['three'])

		self.send_text('I got it!');

	def recv_file(self):
		correct_text = """Just some test text that will be sent
to the http server to verify file sending works
with the http client.
"""

		if self.headers.has_key('content-length') is False:
			self.error('Missing content length')
		length = int( self.headers['content-length'] )
		file_data = self.rfile.read(length)
		if file_data == correct_text:
			self.send_text('Got the file!')
		else:
			self.error('File text corrupted!: %s' % file_data)

	def do_GET(self):
		self.urls[self.path](self)

	def do_POST(self):
		self.urls[self.path](self)

	urls = {
		'/': send_text,
		'/longrequest': long_request,
		'/301redirect': redirect_301,
		'/sendcookie': send_cookie,
		'/recvcookie': recv_cookie,
		'/basicauth': basic_auth,
		'/recvpostdata': recv_post_data,
		'/recvpostparams': recv_post_parameters,
		'/recvfile': recv_file,
		'/requestheaders': recv_headers,
		'/responseheaders': send_headers,
		'/continue': send_continue_headers,
	}

if __name__ == '__main__':
	print 'starting.......'
	server_class = BaseHTTPServer.HTTPServer
	httpd = server_class(('127.0.0.1', 8888), MyHandler)
	httpd.serve_forever()

