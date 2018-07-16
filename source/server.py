from http.server import BaseHTTPRequestHandler,HTTPServer
import os
import base64

addr = '192.168.0.106'
port = 80

#Create custom HTTPRequestHandler class
class HTTPRequestHandler(BaseHTTPRequestHandler):
    
    #handle GET command
    def do_GET(self):
        request = base64.b64decode(self.path[1:].encode()).decode()
        
        print(request)
        
        answer = "!!!!!";
        #send code 200 response
        self.send_response(200)

        #send header first
        self.send_header('Content-type','text-html')
        self.end_headers()

        #send file content to client
        self.wfile.write(answer.encode())
        return
    
def run():
    print('http server is starting...')

    #ip and port of servr
    #by default http server port is 80
    server_address = (addr, port)
    httpd = HTTPServer(server_address, HTTPRequestHandler)
    print('http server is running...')
    httpd.serve_forever()
    
if __name__ == '__main__':
    run()