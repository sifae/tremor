# -*- coding: utf8 -*-
from http.server import BaseHTTPRequestHandler,HTTPServer
from socketserver import ThreadingMixIn
import os
import base64
import sqlite3
import json
import time
from threading import Thread


addr = '192.168.43.150'
port_1 = 80
port_2 = 4444

global answer
global file
file = 'data.db'

def get_parent_dir(directory):
    return os.path.dirname(directory)

parent = get_parent_dir(os.getcwd())
os.chdir("{0}/data".format(parent))

#open('data.txt', 'wb').close()                                               #Удаление данных из data

#Create custom HTTPRequestHandler class
class HTTPHandler_esp(BaseHTTPRequestHandler):
    #handle GET command
    def do_GET(self):
        answer = "OK"
        try:
            request = base64.b64decode(self.path[1:].encode()).decode()
        except BaseException:
            request = self.path[1:]
            
        if 'get_esp' in request:
            size = int(request.split(' ')[1])
            self.send_esp_data(size)
            #print(request)
            
        else:    
            self.store_esp_data(request)
            #print(request)
            
            #send code 200 response
            self.send_response(200)
    
            #send header first
            self.send_header('Content-type','text-html')
            self.end_headers()
    
            #send file content to client
            self.wfile.write(answer.encode())
        return
    
    def store_esp_data(self,data):
        conn = sqlite3.connect(file)
        cursor = conn.cursor()
        time1 = time.time()
        ax,ay,az,gx,gy,gz = data.split(' ')
        data1 = [(time1,ax,ay,az,gx,gy,gz)]
        cursor.executemany("INSERT INTO esp(time,ax,ay,az,gx,gy,gz) VALUES (?,?,?,?,?,?,?)", data1)
        conn.commit()
        conn.close()
        
    def send_esp_data(self,size):
        conn = sqlite3.connect(file)
        cursor = conn.cursor()
        
        sql = "SELECT * FROM esp WHERE id < ?"
        cursor.execute(sql,[(size)])
        l = cursor.fetchall()
        
        arg = ('id','time','ax','ay','az','gx','gy','gz')
        dic = [dict(zip(arg,i)) for i in l]
        jsonarray = json.dumps(dic, ensure_ascii=False)
        
        #send code 200 response
        self.send_response(200)
    
        #send header first
        self.send_header('Content-type','application/json')
        self.end_headers()
    
        #send file content to client
        self.wfile.write(jsonarray.encode())
        
        conn.close()
        
class HTTPHandler_opencv(BaseHTTPRequestHandler): 
    def do_GET(self):
        try:
            request = base64.b64decode(self.path[1:].encode()).decode()
        except BaseException:
            request = self.path[1:]
            
        if 'get_opencv' in request:
            size = int(request.split(' ')[1])
            self.send_opencv_data(size)
            #print(request)
            
        else:    
            self.store_opencv_data(request)
            #print(request)
            #send code 200 response
            self.send_response(200)

    
    def store_opencv_data(self,data):
        conn = sqlite3.connect(file)
        cursor = conn.cursor()
        time1 = time.time()
        x,y = data.split(' ')
        data1 = [(time1,x,y)]
        cursor.executemany("INSERT INTO opencv(time,x,y) VALUES (?,?,?)", data1)
        conn.commit()
        conn.close()     
        
    def send_opencv_data(self,size):
        conn = sqlite3.connect(file)
        cursor = conn.cursor()
        
        sql = "SELECT * FROM opencv WHERE id < ?"
        cursor.execute(sql,[(size)])
        l = cursor.fetchall()
        
        arg = ('id','time','x','y')
        dic = [dict(zip(arg,i)) for i in l]
        jsonarray = json.dumps(dic, ensure_ascii=False)
        
        #send code 200 response
        self.send_response(200)
        
        #send header first
        self.send_header('Content-type','application/json')
        self.end_headers()

        #send file content to client
        self.wfile.write(jsonarray.encode())
        conn.close()    
        

class ThreadingHTTPServer(ThreadingMixIn, HTTPServer):
    pass

def serve_on_port_1(port):
    server = ThreadingHTTPServer((addr,port), HTTPHandler_esp)
    server.serve_forever()
    
def serve_on_port_2(port):
    server = ThreadingHTTPServer((addr,port),HTTPHandler_opencv)
    server.serve_forever()

def run():
    print('http server is starting...')

    #ip and port of servr
    #by default http server port is 80
    server_address_1 = (addr, port_1)
    httpd = HTTPServer(server_address_1, HTTPRequestHandler)
    print('http server is running...')
    httpd.serve_forever()
    
if __name__ == '__main__':
    Thread(target=serve_on_port_2, args=[port_2]).start()
    Thread(target=serve_on_port_1, args=[port_1]).start()
    