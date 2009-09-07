import BaseHTTPServer

class MyHandler(BaseHTTPServer.BaseHTTPRequestHandler):
	def do_GET(s):
		"""Respond to a GET request."""
		s.send_response(200)
		s.send_header("Content-type", "text/plain")
		s.end_headers()
		s.wfile.write("here is some text for you!")

if __name__ == '__main__':
	print 'starting.......'
	server_class = BaseHTTPServer.HTTPServer
	httpd = server_class(('127.0.0.1', 8888), MyHandler)
	httpd.serve_forever()


