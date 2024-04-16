# echo-server.py

import socket

HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
PORT = 65432  # Port to listen on (non-privileged ports are > 1023)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as serverSocket:
    serverSocket.bind((HOST, PORT))
    serverSocket.listen()
    connectionSocket, address = serverSocket.accept()
    with connectionSocket:
        print(f"Connected by {address}")
        while True:
            data = connectionSocket.recv(1024)
            if not data:
                break
            connectionSocket.sendall(data)