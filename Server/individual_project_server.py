import socket

# create a socket object
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# specify the IP address and port to listen on
ip = '192.168.1.71' # replace with your server's IP address
port = 8000
server_socket.bind((ip, port))

# become a server socket
server_socket.listen(5)

while True:
    # establish a connection
    client_socket, addr = server_socket.accept()

    # receive data from the client
    data = client_socket.recv(1024)

    # print the received data
    print(data)

    # close the connection
    client_socket.close()