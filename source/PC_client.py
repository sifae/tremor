import http.client
import base64

addr = '192.168.0.106'
port = 80

request_data = "get_data"

conn = http.client.HTTPConnection("{0}:{1}".format(addr,port))

conn.request("GET", "/{0}".format(request_data))
r1 = conn.getresponse()
data1 = r1.read()
print(data1)

conn.close()