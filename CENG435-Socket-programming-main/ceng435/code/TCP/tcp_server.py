import socket
import constants

class tcp_server:
    def __init__(self, server_host, server_port):
        self.serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_host = server_host
        self.server_port = server_port

        self.serverSocket.bind((self.server_host, self.server_port))
        self.serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 16777216)
        self.serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.serverSocket.listen(constants.MAX_NUMBER_OF_CONNECTION)
        self.connectionSocket, _ = self.serverSocket.accept()

    # This function is used to send the data
    def send(self, binary_file):
        # We first send the length of the file. By doing that, the receiver can
        # know how much data exactly must be waited for receive function
        self.connectionSocket.send('{:16d}'.format(len(binary_file)).encode()) # 1 byte is enough
        self.connectionSocket.sendall(binary_file)

    # we should close the socket
    def close(self):
        # We first close the connection
        self.connectionSocket.close()

        # Then, we close the main socket
        self.serverSocket.close()