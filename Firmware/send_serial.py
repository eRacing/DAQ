#
#	Envoie sur un port COM
#



import serial
from time import sleep

def unslip(buffer):
	# SLIP special characters
	# END = 0xC0
	# ESC = 0xBD
	# ESC_END = 0xDC
	# ESC_ESC = 0xDD

	return buffer.replace("\xC0", "").replace("\xBD\xDC", "\xC0").replace("\xBD\xDD", "\xBD")

def slip(buffer):
	return buffer.replace("\xBD", "\xBD\xDD").replace("\xC0", "\xBD\xDC") + "\xC0"


ser = serial.Serial(
	port = 'COM8',
	baudrate = 115200,
	parity = serial.PARITY_NONE,
	stopbits = serial.STOPBITS_ONE,
	bytesize = serial.EIGHTBITS
	)

print "Port opened :", ser.isOpen()

# Command buffers
buffer_ping = "\x0A" 			# ping command
buffer_set_bitrate = "\x0B\x02" # set bitrate command
buffer_get_config = "\x0C" 		# get config command

# Select buffer to send
#buffer = buffer_get_config
#buffer = buffer_set_bitrate
buffer = buffer_ping

print "<<" + " ".join("{:02x}".format(ord(c)) for c in buffer)

# Send buffer
ser.write(slip(buffer))

# Let time to react
sleep(0.1);

# Read response
out = ser.read(ser.inWaiting())

# Display response
out = unslip(out)
print ">>" + " ".join("{:02x}".format(ord(c)) for c in out)

# Close port
ser.close();