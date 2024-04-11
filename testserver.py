import socket

host = ''
port = 8080

with socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0) as s:
	s.bind((host, port))
	s.listen(1)
	while True:
		conn, addr = s.accept()
		with conn:
			data = conn.recv(1024).decode()
			if data:
				conn.sendall(data.upper().encode())
