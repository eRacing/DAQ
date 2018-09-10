import csv
import sys

filename = sys.argv[1]

with open(filename, "rb") as infile:
    with open(filename.lower()+".csv", 'w', newline='') as outfile:
        writer = csv.writer(outfile)
        row = []
        row.append("timestamp [ms]")
        row.append("msg number")
        row.append("msg id")
        row.append("data")
        writer.writerow(row)
        byte = infile.read(18)
        while byte:
            row = []
            row.append(int.from_bytes(byte[0:4], byteorder='little'))
            row.append(int.from_bytes(byte[4:8], byteorder='little'))
            row.append(int.from_bytes(byte[8:10], byteorder='little'))
            row.append(int.from_bytes(byte[10:18], byteorder='little'))
            writer.writerow(row)
            byte = infile.read(18)
