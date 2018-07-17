import os
import sys
import http.client
import base64
from argparse import ArgumentParser

addr = '192.168.43.150'
port = 80
store = False
request_type = "get_esp"

python_file_name = os.path.basename(os.getcwd())

parser = ArgumentParser(add_help=True)
parser.add_argument("-f", "--file", dest="filename",
                    help="choose file to write", metavar="FILE")
parser.add_argument("-s", "--store",dest="size",
                    help="store data", metavar="SIZE")
parser.add_argument("-t","--type",dest="type",
                    help="type of data [opencv or esp]",metavar="TYPE")

args = parser.parse_args()

if args.size == None:
    print("Choose data size. \nType 'python {0}.py -h' or 'python {0}.py --help' to view help".format(python_file_name))
    
else:
    if args.filename != None:
        if args.filename in os.listdir():
            open(args.filename, 'wb').close()
        store = True
        
    elif args.type == "opencv":
        request_type = "get_opencv"
         
    request_data = base64.b64encode("{1} {0}".format(args.size,request_type).encode()).decode()
    conn = http.client.HTTPConnection("{0}:{1}".format(addr,port))

    conn.request("GET", "/{0}".format(request_data))
    r1 = conn.getresponse()
    data1 = r1.read().decode()
    
    real_size = len(data1.split('\n'))
    
    if store:
        file = open(args.filename,'a')
        file.write(data1)
        file.close()
        print("Store {0} data in {1}".format(int(args.size) if int(args.size) <= real_size else real_size,args.filename))
    else:
        print("Recieve {0} data: \n".format(int(args.size) if int(args.size) <= real_size else real_size))
        print(data1)
    
    conn.close()