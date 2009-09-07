import BaseHTTPServer

class MyHandler(BaseHTTPServer.BaseHTTPRequestHandler):
	def do_GET(s):
		text = "here is some text for you!"
		s.send_response(200)
		s.send_header("Content-type", "text/plain")
		s.send_header("Content-Length", len(text))
		s.end_headers()
		s.wfile.write(text)

if __name__ == '__main__':
	print 'starting.......'
	server_class = BaseHTTPServer.HTTPServer
	httpd = server_class(('127.0.0.1', 8888), MyHandler)
	httpd.serve_forever()


