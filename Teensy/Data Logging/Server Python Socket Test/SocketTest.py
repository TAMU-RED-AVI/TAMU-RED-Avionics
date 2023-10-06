import socket

HOST = '0.0.0.0'
PORT = 1234

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((HOST, PORT))
s.listen()

print(f"Listening for connections on port {PORT}...")
conn, addr = s.accept()
print(f"Connected by: {addr}")

try:
    while True:
        data = conn.recv(1024)
        if not data:
            break
        print("Received:", data.decode('utf-8'))

        # Prompt the user for input and send a command to the Teensy
        user_input = input(
            "Enter 'pause' to stop Teensy from sending data for 30 seconds or just press ENTER to continue: ")
        if user_input.strip().lower() == "pause":
            conn.sendall((user_input + '\n').encode('utf-8'))
finally:
    conn.close()
    print("Connection closed.")
