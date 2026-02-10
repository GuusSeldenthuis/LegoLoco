#!/usr/bin/env python3

# 2023-08-29
# This files originally comes from:
# https://gist.github.com/shinyquagsire23/20a86e65206ece28683a9b4c5597e172
# And is updated by: 


import struct
import os
import io
from pathlib import Path

path = Path(__file__).parent.absolute()
# Set working directory to file path
os.chdir(path)

rfh_file = "resource.RFH"
rfd_file = "resource.RFD"


def huff_decompress(data):
    byte = 0
    decomp = io.BytesIO()

    node = node_0 = struct.unpack_from('<H', data, 4)[0]  # byte 4 to 5 contain the first node

    for bytepos in range(0x808, len(data)):  # 0x808 is the startpoint of the compressed data
        byte = struct.unpack_from('<B', data, bytepos)[0]  # unpack 1 byte from data
        for bit_pos in range(0, 8):  # go through all bits
            bit = byte & 1
            byte = byte >> 1  # Bits are shifted to the next relevant bit

            # calculate the node position in the file
            nodepos = (node * 4) + (bit * 2) + 8
            # extract the next node from the new node position
            node = struct.unpack_from('<H', data, nodepos)[0]
            isterminal = (node & 0x100) == 0

            if isterminal:
                # write the decoded character down
                decomp.write(node.to_bytes(1, 'little'))
                # reset to the root node again
                node = node_0

    return decomp

def extract_files(header_data, pos, file_pos):
    # determine the length of the string (filename)
    # by unpacking the first 4 bytes
    try:
        str_len = struct.unpack_from("<L", header_data, pos)[0]
    except struct.error:
        return (-1, -1)
    pos += 4

    filename = "resource_rfd/"
    for i in range(0, str_len-1):
        filename += chr(header_data[pos])
        pos += 1
    pos += 1

    file_size = struct.unpack_from("<L", header_data, pos)[0]
    pos += 4

    flags = struct.unpack_from("<L", header_data, pos)[0]
    pos += 4

    filename = filename.replace("\\", "/")
    print(f"{filename:<30}, Size: {file_size:8d} bytes, Flags: {flags}\n")


    if not os.path.exists(os.path.dirname(filename)):
        os.makedirs(os.path.dirname(filename))

    with open(rfd_file, 'rb') as blob_f:
        # Read the next segment of the data file
        blob_f.seek(file_pos)
        new_data = blob_f.read(file_size)
        file_pos = blob_f.tell()

    if flags & 1:
        # Give compressed files a unique filename
        # filename = filename[:-4] + '_decompr' + filename[-4:]

        # Decompress the file
        new_data = huff_decompress(new_data)

    with open(filename, 'wb') as file:
        if flags & 1:
            # Write the decompressed data
            file.write(new_data.getvalue())
            new_data.close()
        else:
            file.write(new_data)

    return (pos, file_pos)

def extract_all():

    with open(rfh_file, 'rb') as rfh:
        # Read the whole header file and save it into the variable
        header_file = rfh.read()

    pos = 0
    file_pos = 0
    while True:
        pos, file_pos = extract_files(header_file, pos, file_pos)
        if pos == -1:
            break
    print("All files have been extracted!")

def read_header_line(header_data, pos):

    try:
        str_len = struct.unpack_from("<L", header_data, pos)[0]
    except struct.error:
        return (-1, -1)

    pos += 4

    filename = ''
    for i in range(0, str_len-1):
        filename += chr(header_data[pos])
        pos += 1
    pos += 1

    file_size = struct.unpack_from("<L", header_data, pos)[0]
    pos += 4

    flags = struct.unpack_from("<L", header_data, pos)[0]
    pos += 4

    filename = filename.replace("\\", "/")
    #print(f"{filename:<30}, Size: {file_size:8d} bytes, Flags: {flags}")

    return (pos, filename, file_size, flags)

def print_file_list(files_n: int = 8):

    pos = 0
    with open(rfh_file, 'rb') as rfh:
        # Read the whole header file and save it into the variable
        header_data = rfh.read()

        for i in range(files_n):
            pos = read_header_line(header_data, pos)[0]

def analyse_header():
    pos = 0
    data = {'count': 0, 'compressed': 0,}
    n = 0
    with open(rfh_file, 'rb') as rfh:
        # Read the whole header file and save it into the variable
        header_data = rfh.read()

        while pos != -1 and n < 10000:
            try:
                pos, name, size, flag = read_header_line(header_data, pos)
            except ValueError:
                break

            data['count'] += 1

            ending = '.' + name[-3:].lower()
            if ending not in data:
                data[ending] = 1
            else:
                data[ending] += 1

            if flag == 1:
                data['compressed'] += 1
            n += 1

    for key in data.items():
        print(f"{key[0]}: {key[1]}")
    print(f"{data['compressed'] / data['count'] * 100:.1f} % of the files are compressed.")

def decompress_file(filename: str):
    with open(filename, 'rb') as f:
        # Read the whole file and save it into the variable
        file_data = f.read()

    new_data = huff_decompress(file_data)

    filename = filename.replace('_dec0', '')
    filename = filename[:-4] + '_dec1' + filename[-4:]

    with open(filename, 'wb') as f:
        f.write(new_data.getvalue())
        new_data.close()

extract_all()
#print_file_list(50)
#decompress_file("bush_dec0.but")
#analyse_header()
