import socket
import argparse
import sys
import time

light_duration = "2"
irrigation_duration = "3"
start_time = 0


def recv(sock):
    data = sock.recv(1)
    buf = b""
    while data.decode("utf-8") != "\n":
        buf += data
        data = sock.recv(1)
    return buf

def process_data(data, sock):
    str = data.decode("utf-8")
    
    if "Server log" in str:

        #process the message received
        str_split = str.split("[INFO: App       ] ")
        src = str_split[2].split(" ")[1]
        dst = str_split[3].split(" ")[1]
        payload = str_split[4].split(" ")[1]
        print(f"src: {src}, subgateway: {dst}, payload: {payload}\n")
        
        #check if payload is an ack for irrigation system
        if payload == "ACK":
            print(f"ACK irrigation received from {src}\n")

        #payload is a light sensor data
        else :
            payload_value = int(payload)
            if payload_value < 50:
                print("payload value is less than 50, starting lights\n")
                addr = dst.replace(".","")
                data = f"L {addr} {light_duration}\n"
                sock.send(bytes(data, "utf-8"))

    else:
        return

def main(ip, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((ip, port))
    start_time = time.time()
    irrigation_done = False
    while True:
        data = recv(sock)
        process_data(data, sock)

        #set the irrigation to start 40sec after starting the program to simulate specific time of day
        if time.time() - start_time > 75 and not irrigation_done:
            print("Time for irrigation system\n")
            message = f"I {irrigation_duration}\n"
            sock.send(bytes(message, "utf-8"))
            irrigation_done = True
        
     


if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", dest="ip", type=str)
    parser.add_argument("--port", dest="port", type=int)
    args = parser.parse_args()

    main(args.ip, args.port)

