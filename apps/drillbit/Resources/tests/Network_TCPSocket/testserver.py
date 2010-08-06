import threading
import SocketServer

class TestServerHandler(SocketServer.BaseRequestHandler):
    """A simple testing server for TCP drillbit tests"""

    def handle(self):
        print 'New connection started'

        while True:
            try:
                data = self.request.recv(1024)
                length = len(data)
                if length == 0: break
                print 'Recv %d bytes' % length

                # Echo back the text that was just sent to us
                self.request.send(data)
                print 'Sent echo response'
            except Exception, e:
                print 'ERROR: %s' % e
                break

        print 'Connection closed'

class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    pass

if __name__ == '__main__':

    server = ThreadedTCPServer(('127.0.0.1', 8080), TestServerHandler)
    server.serve_forever()
