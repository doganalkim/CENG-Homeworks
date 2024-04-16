from tcp_server import *
import socket
import constants

# Here is our main function for our TCP server, to operate server operations such as reading objects and sending them to the TCP client.

server_ip_address = ''

if __name__ == '__main__':
    # I initiate with some random ip
    server_ip_address = '0.0.0.0'
    try:
        # This part writes the server ip to the file. Here, our purpose is to
        # automate the system without entering the ip addresses by hand.
        # Therefore, you do not need to change anything related to IP addresses
        server_host = socket.gethostname()
        server_ip_address = socket.gethostbyname(server_host)
        with open("ip.txt","w") as ip_file:
            ip_file.write(server_ip_address)
    except socket.error as e:
        pass

    # We instantiate our tcp_server with the server_host and server_port parameters.
    tcp_server = tcp_server(server_ip_address, server_port=constants.TCP_PORT)

    # for each file index, we read a large and small file and send them by the tcp_server to the tcp_client
    for i in range(constants.NUMBER_OF_FILES):
        with open("../../root/objects/large-" + str(i) + ".obj", "rb") as binary_file:
            tcp_server.send(binary_file.read())
        with open("../../root/objects/small-" + str(i) + ".obj", "rb") as binary_file:
            tcp_server.send(binary_file.read())

    tcp_server.close()  # Since we didn't create our socket in tcp_server.py using with block, we should close the connection explicitly.