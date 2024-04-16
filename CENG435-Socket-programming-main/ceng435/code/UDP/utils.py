import datetime
import hashlib

# returns UNIX timestamp in terms of seconds
def get_current_timestamp():
    return datetime.datetime.now().timestamp()

# uses hashlib library for hash generation
def calculate_checksum(data):
    return hashlib.md5(data).digest()