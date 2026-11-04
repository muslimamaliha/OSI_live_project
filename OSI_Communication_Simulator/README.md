# OSI Communication Simulator

An educational OSI 7-layer encapsulation and decapsulation simulation using:
- C-based client and server applications
- Real TCP socket communication
- Python user application to build, launch, and display progress
- Layer-by-layer encapsulation and decapsulation logs
- Progress output after every two layers

## Important technical note

This is an educational simulation of all seven OSI layers. The C program adds and removes labelled simulation headers for each layer. The client and server exchange the resulting data using a real TCP socket. The program does **not** replace the operating system's actual Ethernet, IP, or TCP stack. Physical and Data Link processing are represented as educational layers because ordinary user-space TCP sockets do not expose direct control over those operations.

## Requirements

- Windows 10/11 with MinGW-w64 GCC and Python 3, or Linux with GCC and Python 3
- VS Code is recommended

Check installations:

```bash
gcc --version
python --version
```

## Run on Windows (PowerShell)

Open this project folder in VS Code. Open the integrated terminal and run:

```powershell
python python/app.py
```

The Python application compiles `client.c` and `server.c`, starts the server, asks for a message, runs the client, and prints both sides' progress.

If GCC is not recognized, install MinGW-w64 and add its `bin` directory to the Windows PATH. Restart VS Code after changing PATH.

## Run on Linux

```bash
python3 python/app.py
```

## Expected flow

Sender:
1. Application
2. Presentation
3. Session
4. Transport
5. Network
6. Data Link
7. Physical

The C client adds a labelled header at each simulated layer. It then sends the encapsulated message to the server using TCP.

Receiver:
1. Physical
2. Data Link
3. Network
4. Transport
5. Session
6. Presentation
7. Application

The C server removes each simulated header and displays the original message. It sends an acknowledgement back to the client.

## Project structure

```text
OSI_Communication_Simulator/
├── src/
│   ├── client.c
│   ├── server.c
│   └── common.h
├── python/
│   └── app.py
├── screenshots/
└── README.md
```

## Suggested screenshots for submission

1. Python application showing the entered message and sender encapsulation.
2. Server output showing receiver decapsulation and original message.
3. Screenshot showing PC username or desktop date/time as required by the instructor.
4. Optional Wireshark capture showing the TCP connection on port 5050.

Before submission, add your own screenshots to `screenshots/` and include your GitHub repository URL in your course submission.

## Troubleshooting

- **GCC not found:** Install MinGW-w64 GCC and add its `bin` folder to PATH.
- **Port 5050 already in use:** Close another copy of this simulator or change `PORT` in `src/common.h`.
- **Connection failed:** Run the Python application so it starts the server before the client.
