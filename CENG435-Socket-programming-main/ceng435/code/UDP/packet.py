import constants
import struct
import utils

# This is the smallest unit of sending. We called it packet
# We have variables that are actually the fields of headers
class packet:
    # update header/payload size if there is a change here
    def __init__(self, sequence_number, stream_id=None, payload=None, state=constants.WAITING):
        self.sequence_number = sequence_number  # Sequence number ||| I -> 4 bytes
        self.stream_id = stream_id  # The id of the large of small file ||| H -> 2 bytes
        self.sent_time = None  # We record time for timeout case from RDT ||| d -> 8 bytes
        self.state = state  # We set state to find out whether waiting or sent ||| H -> 2 bytes
        self.payload = payload  # The data, or payload part ||| Variable length
        self.payload_size = None  # The size of the payload ||| I -> 4 bytes
        self.checksum = None  # Checksum for RDT ||| s -> 16 bytes

    # We have used struct library for conversion between Python values and C-style data representations.
    # This library helps us work with low-level binary representations
    # packs the packet into the bytes, also calculates the checksum before packing
    def pack(self):
        self.payload_size = len(self.payload)
        self.checksum = self.calculate_checksum()
        free_space = constants.PACKET_PAYLOAD_SIZE - self.payload_size
        return struct.pack(f'!I16sHdHI{self.payload_size}s{free_space}s', self.sequence_number, self.checksum,
                           self.stream_id, self.sent_time, self.state, self.payload_size, self.payload,
                           b'' * free_space)

    # packs the packet into the bytes, also calculates the checksum before packing
    def pack_ack(self):
        checksum = utils.calculate_checksum(bytes(str(self.sequence_number), 'utf8'))
        return struct.pack(f'!I16s', self.sequence_number, checksum)

    # calculates the checksum for the packet being sent
    def calculate_checksum(self):
        data = self.payload + bytes(str(self.sequence_number), 'utf8') + bytes(str(self.stream_id), 'utf8') + \
               bytes(str(self.state), 'utf8')

        return utils.calculate_checksum(data)

    # calculates the checksum for the ack packet being sent, using utf8 encoding
    def calculate_checksum_ack(self):
        data = bytes(str(self.sequence_number), 'utf8')
        return utils.calculate_checksum(data)

# extracts the sequence_number, payload, and stream_id fields from the packet
def unpack(received_packet):
    sequence_number, checksum, stream_id, sent_time, state, payload_size = struct.unpack(f'!I16sHdHI', received_packet[
                                                                                                       :constants.PACKET_HEADER_SIZE])
    # unused space, it's important for the fragmentation of the packets
    free_space = constants.PACKET_PAYLOAD_SIZE - payload_size

    # unpacks the packet with respect to the format string
    payload, _ = struct.unpack(f'{payload_size}s{free_space}s',
                               received_packet[constants.PACKET_HEADER_SIZE:constants.PACKET_TOTAL_SIZE])

    # if calculated checksum matches with the checksum being sent form the server, than return sequence number, payload and stream_id
    if checksum == utils.calculate_checksum(
            payload + bytes(str(sequence_number), 'utf8') + bytes(str(stream_id), 'utf8') + \
            bytes(str(state), 'utf8')):
        return sequence_number, payload, stream_id

    # else, return all None
    return None, None, None


# extracts the sequence number and checksum fields from the ack packet
def unpack_ack(received_packet):
    seq, checksum = struct.unpack(f'!I16s', received_packet)

    # if calculted checksum matches with the checksum being sent form the server, than return sequence number
    if checksum == utils.calculate_checksum(bytes(str(seq), 'utf8')):
        return seq
    # if they don't match, return None
    return None
