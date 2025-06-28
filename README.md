# TCP Chat App (C++ – Windows)

This is a simple **client-server chat application** built in **C++** using **TCP sockets** on **Windows**.  
Multiple clients can connect to the server and exchange messages using basic commands.

---

## Features

- Multiple clients can connect to the server
- `LIST` command shows connected client IDs
- `SEND <id> <message>` sends a message to a specific client
- Built with Windows Sockets (Winsock2)

---

## How to Run

### Requirements

- Windows OS
- g++ compiler (via **MSYS2**, **MinGW**, or similar)
- Terminal / Command Prompt

---

### Clone the Repository

```bash
git clone https://github.com/your-username/tcp-chat-app.git
cd tcp-chat-app
```

### To Complile the code 
``` bash
g++ server.cpp -o server -lws2_32
g++ client.cpp -o client -lws2_32
```

### Run the Server

```./server.exe
```

### Run the client(in seperate terminal use mingw terminal in case of errors

```./client.exe
```

The code is working
