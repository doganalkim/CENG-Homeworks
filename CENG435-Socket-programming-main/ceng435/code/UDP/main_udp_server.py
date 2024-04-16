from udp_server import *
import constants
import zlib

def load_files():
    for i in range(10):
        with open("../../root/objects/large-" + str(i) + ".obj", "rb") as f:
            while True:
                chunk = f.read(constants.PACKET_PAYLOAD_SIZE)
                if not chunk:
                    break

                # We compress files thanks to advise from Ertan hoca
                # print(1 - len(zlib.compress(chunk)) / len(chunk), "% large file improvement")
                # print("large chunk size:", len(chunk))
                # print("large compressed chunk size:", len(zlib.compress(chunk)))
                yield zlib.compress(chunk)
        yield b''  # end of the file

        with open("../../root/objects/small-" + str(i) + ".obj", "rb") as f:
            while True:
                chunk = f.read(constants.PACKET_PAYLOAD_SIZE)
                if not chunk:
                    break

                # We compress files thanks to advise from Ertan hoca
                # print(1 - len(zlib.compress(chunk)) / len(chunk), "% large file improvement")
                # print("small chunk size:", len(chunk))
                # print("small compressed chunk size:", len(zlib.compress(chunk)))
                yield zlib.compress(chunk)
        yield b''  # end of the file


if __name__ == '__main__':
    # creates the udp server object, with server ip address, server port, client ip address and client port
    # loads all files as data to the udp server object, putting a file seperator b'' between them
    udp_server = udp_server("192.168.215.2", constants.UDP_SERVER_PORT,
                            "192.168.215.3", constants.UDP_CLIENT_PORT,
                            load_files())

    # sends the loaded files to the client
    udp_server.main_send()

    # closes the udp server socket
    udp_server.close_socket()
