import socket
from packet import *
import constants
from collections import deque
import zlib


class udp_client:
    def __init__(self, clientHost, clientPort):
        # class members
        self.clientHost = clientHost
        self.clientPort = clientPort

        # creates the client socket and does the binding
        self.clientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.clientSocket.bind((self.clientHost, self.clientPort))

        # disables socket blocking
        self.clientSocket.setblocking(False)

        # sets the receiver buffer space
        self.clientSocket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 16777216)

        # class members with initial values
        self.sequence_number = 1

        # boolean array keeping track of the finished files
        self.finished_files = [False for _ in range(2 * constants.NUMBER_OF_FILES)] # 0...19

        # creates the client window and fills it
        self.client_window = deque()
        self.fill_window_with_ack()

    # as its name suggests, fills the window with acks
    def fill_window_with_ack(self):
        while len(self.client_window) < constants.WINDOW_SIZE:
            self.client_window.append(packet(self.sequence_number))
            #print(f"self.sequence_number: {self.sequence_number}")
            self.sequence_number += 1

    def transmission_not_ended(self):
        for stream_id in range(2 * constants.NUMBER_OF_FILES): # 0...19
            if not self.finished_files[stream_id]:
                return True
        return False

    def receive_from_server(self):
        while self.transmission_not_ended():
            try:
                data, server_address = self.clientSocket.recvfrom(constants.PACKET_TOTAL_SIZE)
                # retrieves the needed values
                sequence_number, payload, stream_id = unpack(data)

                # the packet is not corrupted and sequence number doesn't exceed the largest sequence number in the window
                if payload is not None and sequence_number <= self.client_window[-1].sequence_number:

                    # ignores the sequence number lower than the first sequence number in the window
                    if sequence_number < self.client_window[0].sequence_number:
                        continue

                    # match
                    if sequence_number == self.client_window[0].sequence_number:
                        # sets the retrieved values to the client_window[0]
                        self.client_window[0].payload = payload
                        self.client_window[0].stream_id = stream_id
                        self.client_window[0].state = constants.RECEIVED

                        while self.client_window:

                            # if the file has finished, pops the packet
                            if self.client_window[0].stream_id is not None and self.finished_files[self.client_window[0].stream_id] == True:
                                self.client_window.popleft()
                                continue

                            # if we find a received packet, break
                            if self.client_window[0].state != constants.RECEIVED:
                                break

                            # finished the file
                            if len(self.client_window[0].payload) == 0:
                                self.finished_files[self.client_window[0].stream_id] = True
                                self.client_window.popleft()
                                continue

                            # returns the payload and stream_id array in the background
                            yield zlib.decompress(self.client_window[0].payload), self.client_window[0].stream_id

                            self.client_window.popleft()

                            self.client_window.append(packet(self.sequence_number))
                            # print(f"self.sequence_number: {self.sequence_number}")
                            self.sequence_number += 1

                    # the sequence numbers don't match, finds that packet and sets its values
                    else:
                        for p in self.client_window:
                            if p.sequence_number == sequence_number and p.state == constants.WAITING:
                                p.payload = payload
                                p.state = constants.RECEIVED
                                p.stream_id = stream_id


                ack_packet = packet(self.client_window[0].sequence_number)
                self.clientSocket.sendto(ack_packet.pack_ack(), server_address)

            except BlockingIOError:
                pass


    # close the socket connection
    def close_socket(self):
        self.clientSocket.close()