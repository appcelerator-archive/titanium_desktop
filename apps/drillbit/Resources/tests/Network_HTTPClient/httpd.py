import BaseHTTPServer
from Cookie import SimpleCookie
import base64
import time

reply = "I got it!"
error = "bad data!"

class MyHandler(BaseHTTPServer.BaseHTTPRequestHandler):
	def send_text(self, text='here is some text for you!'):
		self.send_response(200)
		self.send_header("Content-type", "text/plain")
		self.send_header("Content-Length", len(text))
		self.end_headers()
		self.wfile.write(text)

	def do_GET(self):
		if self.path == "/longrequest":
			print "Starting long request..."
			time.sleep(5.0)
			print "Done with long request sending response..."
			self.send_text()
		elif self.path == "/301redirect":
			print "Redirecting..."
			self.send_response(301)
			self.send_header("Location", "http://127.0.0.1:8888/")
			self.end_headers()
		elif self.path == "/sendcookie":
			print "Receiving a cookie..."
			try:
				cookies = SimpleCookie(self.headers['Cookie'])
				value = cookies['peanutbutter'].value
				if value == 'yummy':
					self.send_text('got the cookie!')
				else:
					raise Exception('Bad cookie! value = %s' % value)
			except Exception, e:
				print 'Error! %s' % e
				self.send_response(400)
		elif self.path == "/recvcookie":
			print "Sending a cookie..."
			self.send_response(200)
			self.send_header('Set-Cookie', 'chocolatechip=tasty')
			self.end_headers()
		elif self.path == "/basicauth":
			print "Basic auth..."
			basic_auth = self.headers['Authorization']
			if basic_auth.startswith('Basic') is False:
				raise Exception('Not basic auth!')
			up = base64.decodestring(basic_auth.replace('Basic', '').strip())
			username, password = up.split(':')
			if username == 'test' and password == 'password':
				print 'Valid: username=%s password=%s' % (username, password)
				self.send_text('authorized')
			else:
				raise Exception('Invalid username/password: %s:%s' % (username, password))
		else:
			print "Sending text..."	
			self.send_text()

	def do_POST(self):
		if self.headers.has_key('content-length'):
			length= int( self.headers['content-length'] )
			data = self.rfile.read(length)
			print "Got data: %s" % data
			if data == text:
				self.send_response(200)
				self.send_header("Content-type", "text/plain")
				self.send_header("Content-Length", len(reply))
				self.end_headers()
				self.wfile.write(reply)
				return
		self.send_response(400)
		self.send_header("Content-type", "text/plain")
		self.send_header("Content-Length", len(error))
		self.end_headers()
		self.wfile.write(error)

if __name__ == '__main__':
	print 'starting.......'
	server_class = BaseHTTPServer.HTTPServer
	httpd = server_class(('127.0.0.1', 8888), MyHandler)
	httpd.serve_forever()

