from udp_client import *
import constants

if __name__ == '__main__':
    # creates the udp client object, with client ip address and client port
    udp_client = udp_client("192.168.215.3", constants.UDP_CLIENT_PORT)

    # receives the files (actually bytes) from the server
    udp_client.receive_from_server()

    # creates 20 file buffer array, 10 for small files and 10 for large files
    file_buffer =[bytes() for A in range(2 * constants.NUMBER_OF_FILES) ]

    # retrieves the extracted data
    for payload, stream_id in udp_client.receive_from_server():
        file_buffer[stream_id] += payload

    # writes file buffers to the corresponding file
    for i in range(2 * constants.NUMBER_OF_FILES):
        file_name = ("large-" if i % 2 == 0 else "small-") + str(i // 2) + ".obj"

        with open(file_name, "wb") as binary_file:
            binary_file.write(file_buffer[i])

    # closes the udp client socket
    udp_client.close_socket()