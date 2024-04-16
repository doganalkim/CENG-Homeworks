import socket
from collections import deque


from packet import *




class udp_server:
    def __init__(self, serverHost, serverPort, clientHost, clientPort, data=None):
        # class members
        self.serverHost = serverHost
        self.serverPort = serverPort
        self.clientHost = clientHost
        self.clientPort = clientPort
        self.data = data

        # creates the socket and does the binding
        self.serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.serverSocket.bind((self.serverHost, self.serverPort))

        # disables socket blocking
        self.serverSocket.setblocking(False)

        # sets the sender buffer space
        self.serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 16777216)

        # class members with initial values
        self.sequence_number = 1
        self.stream_id = 0
        self.duplicate_ack_count = 0

        # creates the server_window and fills it
        self.server_window = deque()
        self.fill_window()

    # fills the window until window is full
    def fill_window(self):
        while len(self.server_window) < constants.WINDOW_SIZE and self.stream_id < 20:
            try:
                # retrives the next yielded chunk
                chunk = next(self.data)

                # creates a new packet with its fields
                self.server_window.append(packet(self.sequence_number, self.stream_id, chunk, constants.WAITING))
                self.sequence_number += 1

                # the file is completely transferred
                if len(chunk) == 0:
                    self.stream_id += 1

            except StopIteration:
                break



    def main_send(self):
        while self.server_window:
            # finds the first waiting packet and sends to the client
            for p in self.server_window:
                if p.state == constants.WAITING:
                    packet_to_send = p
                    cur_time = utils.get_current_timestamp()
                    packet_to_send.sent_time = cur_time
                    packet_to_send.state = constants.SENT
                    self.serverSocket.sendto(packet_to_send.pack(), (self.clientHost, self.clientPort))
                    break # if we do not use break, process slows down
            self.send_to_client()

    # Main magic happens here
    def send_to_client(self):
            try:
                # We initally wait for ack that comes from receiver
                ack_packet, _ = self.serverSocket.recvfrom(constants.ACK_PACKET_SIZE)
                ack_seq = unpack_ack(ack_packet)

                if ack_seq is None:
                    return

                # If ACK comes for formerly ACKed data, we handle it with duplicate ACK counter
                if ack_seq <= self.server_window[0].sequence_number:
                    self.duplicate_ack_count += 1
                    if self.duplicate_ack_count == 3:
                        packet_to_send = self.server_window[0]
                        self.serverSocket.sendto(packet_to_send.pack(), (self.clientHost, self.clientPort))
                        self.duplicate_ack_count = 0
                    return

                # We reset because appropriate packet has come
                self.duplicate_ack_count = 0

                # This is the application of cumulative ack. Receiver sends the expected ACK sequence,
                # which mean we should skip the packets with smaller sequence numbers than the received ACK
                while self.server_window and self.server_window[0].sequence_number < ack_seq:
                    self.server_window.popleft()

            except BlockingIOError:
                pass


            # The transmission timeouts are checked. If the timeout has occured, just resend.
            # This is used if the ACK coming from the receiver is lost
            for p in self.server_window:
                if p.sent_time is None:
                    continue
                cur_time = utils.get_current_timestamp()
                if cur_time - p.sent_time > constants.PACKET_TIMEOUT_IN_SECONDS:
                    p.sent_time = cur_time
                    self.serverSocket.sendto(p.pack(), (self.clientHost, self.clientPort))

            # Just keep the window size equal to predetermined window size
            self.fill_window()

    # We close the socket
    def close_socket(self):
        self.serverSocket.close()