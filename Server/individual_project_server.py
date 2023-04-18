import socket

IP = '192.168.1.71' # replace with your server's IP address
PORT = 8000

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
	server_socket.bind((IP, PORT))
	server_socket.listen(10)
	while True:
		client_socket, addr = server_socket.accept()
		with client_socket:
			data = client_socket.recv(1024).decode()
			if data:
				# print(data)
				POT_ID = int(data.split(' ')[0])
				with open('measurements/' + str(POT_ID) + '.txt', 'a') as f:
						s = ''.join(data[1:])[1:]
						f.write(s + '\n')