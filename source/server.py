from http.server import BaseHTTPRequestHandler,HTTPServer
import os
import base64
import pickle

addr = '192.168.0.106'
port = 80
global answer

def get_parent_dir(directory):
    import os
    return os.path.dirname(directory)

parent = get_parent_dir(os.getcwd())
os.chdir("{0}/data".format(parent))

#Create custom HTTPRequestHandler class
class HTTPRequestHandler(BaseHTTPRequestHandler):
    #handle GET command
    def do_GET(self):
        answer = "OK"
        try:
            request = base64.b64decode(self.path[1:].encode()).decode()
        except BaseException:
            request = self.path[1:]
            
        if request == 'get_data':
            self.send_data()
            
        else:    
            self.store_data(request)
            print(request)
            
            #send code 200 response
            self.send_response(200)
    
            #send header first
            self.send_header('Content-type','text-html')
            self.end_headers()
    
            #send file content to client
            self.wfile.write(answer.encode())
        return
    
    def store_data(self,data):
        file = open('data.txt','a')
        file.write(data)
        file.close()  
        
    def send_data(self):
        file = open('data.txt','rb')
        data = file.readline()
        self.send_response(200)
        self.send_header('Content-type','text-html')
        self.end_headers()
        #send file content to client
        self.wfile.write(data)
        
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