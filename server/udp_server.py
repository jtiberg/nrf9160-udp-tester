import datetime
import socket

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind the socket to the port
server_address = ("0.0.0.0", 42501)
sock.bind(server_address)

print('starting up on {} port {}'.format(*server_address))
while True:
    data, address = sock.recvfrom(512)

    tstr = datetime.datetime.now().isoformat()[0:len("2019-09-12T11:25:41")]
    msg = data.decode("utf-8").strip()
    print('%s\t%s\t%s' % (tstr, address, msg))

    #if data:
    #    sent = sock.sendto(data, address)
    #    print('sent {} bytes back to {}'.format(
    #        sent, address))
