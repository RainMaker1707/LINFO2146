import socket
import argparse
import sys
import time

def recv(sock):
    data = sock.recv(1)
    buf = b""
    while data.decode("utf-8") != "\n":
        buf += data
        data = sock.recv(1)
    return buf

def process_data(data):
    str = data.decode("utf-8")
    if "Server log" in str:
        print(str)
    else:
        print("skipped message + " + str + " + \n")


def main(ip, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((ip, port))

    while True:
        data = recv(sock)
        process_data(data)
        #print(data.decode("utf-8"))
        #sock.send(b"test\n")
        
        #time.sleep(0.1)


if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", dest="ip", type=str)
    parser.add_argument("--port", dest="port", type=int)
    args = parser.parse_args()

    main(args.ip, args.port)

