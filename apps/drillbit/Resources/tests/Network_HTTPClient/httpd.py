import BaseHTTPServer
import time

text = "here is some text for you!"
reply = "I got it!"
error = "bad data!"

class MyHandler(BaseHTTPServer.BaseHTTPRequestHandler):
	def send_text(self):
		self.send_response(200)
		self.send_header("Content-type", "text/plain")
		self.send_header("Content-Length", len(text))
		self.end_headers()
		self.wfile.write(text)

	def do_GET(self):
		if self.path == "/longrequest":
			print "Starting long request..."
			time.sleep(20)
			print "Done with long request sending response..."
			self.send_text()
		else:	
			self.send_text()

	def do_POST(self):
		if self.headers.has_key('content-length'):
			length= int( self.headers['content-length'] )
			data = self.rfile.read(length)
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

