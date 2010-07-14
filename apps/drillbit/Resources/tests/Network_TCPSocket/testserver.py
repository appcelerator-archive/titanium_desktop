import SocketServer

class TestServerHandler(SocketServer.BaseRequestHandler):
    """A simple testing server for TCP drillbit tests"""

    def handle(self):
        print 'New connection...'

        try:
            # Assume test case first writes a string
            text = self.request.recv(1024)
            print 'Got data: %s' % text

            # Echo back the text that was just sent to us
            self.request.send(text)
            print 'Sent response'
        except Exception, e:
            print 'ERROR: %s' % e

        print 'Done.'

if __name__ == '__main__':

    server = SocketServer.TCPServer(('127.0.0.1', 8080), TestServerHandler)
    server.serve_forever()
