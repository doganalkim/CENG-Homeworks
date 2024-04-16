import socket
import constants


class tcp_client:
    def __init__(self, server_host, server_port):
        # This is the constructor. We initalize the socket and call bind
        self.clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_host = server_host
        self.server_port = server_port

        self.clientSocket.connect((self.server_host, self.server_port))
        self.clientSocket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 16777216)
        self.clientSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


    # the function used for large data transfer
    def large_receive(self, file_index):

        large_file_size = int(self.clientSocket.recv(constants.FILE_SIZE_LENGTH))

        large_buffer = b''

        while len(large_buffer) < large_file_size:
            # 1024 is needed because it reads a portion of a different file.
            # it should not read the following packet, so we use min operator
            large_buffer += self.clientSocket.recv(min(large_file_size - len(large_buffer), 1024))

        large_file_name = "large-" + str(file_index) + ".obj"
        with open(large_file_name, "wb") as f:
            f.write(large_buffer)

    # this function is used for small data transfer
    def small_receive(self, file_index):
        small_file_size = int(self.clientSocket.recv(constants.FILE_SIZE_LENGTH))

        small_buffer = b''

        while len(small_buffer) < small_file_size:
            # 1024 is needed because it reads a portion of a different file.
            # it should not read the following packet, so we use min operator
            small_buffer += self.clientSocket.recv(min(small_file_size - len(small_buffer), 1024))

        small_file_name = "small-" + str(file_index) + ".obj"
        with open(small_file_name, "wb") as f:
            f.write(small_buffer)

    # we should close the socket
    def close(self):
        self.clientSocket.close()
