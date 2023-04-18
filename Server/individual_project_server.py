import socket

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

IP = '192.168.1.71' # replace with your server's IP address
PORT = 8000
server_socket.bind((IP, PORT))

server_socket.listen(10)

while True:
    client_socket, addr = server_socket.accept()

    data = client_socket.recv(1024).decode()

    # print(data)

    POT_ID = int(data.split(' ')[0])
    with open('measurements/' + str(POT_ID) + '.txt', 'a') as f:
            s = ''.join(data[1:])[1:]
            f.write(s + '\n')

    client_socket.close()