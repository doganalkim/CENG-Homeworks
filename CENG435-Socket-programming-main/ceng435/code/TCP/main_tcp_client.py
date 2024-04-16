from tcp_client import *
import constants

server_ip_address = ''

if __name__ == '__main__':
    # we read ip.txt file in order to learn the ip of the server
    with open("ip.txt","r") as ip_file:
        server_ip_address = ip_file.read()

    # we initiate the client
    tcp_client = tcp_client(server_host=server_ip_address, server_port=constants.TCP_PORT)

    # we send each file one by one staring with large files followed by small ones
    for i in range(constants.NUMBER_OF_FILES):
        tcp_client.large_receive(i)
        tcp_client.small_receive(i)

    tcp_client.close()  # Since we didn't create our socket using with block, we should close the connection explicitly.
