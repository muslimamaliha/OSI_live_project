# Experiment Report: OSI Communication Simulation

**Name:** [Your name]  
**Student ID:** [Your ID]  
**Course:** [Course name/code]  
**Date and Time:** [Actual date and time]  
**GitHub URL:** [Your repository URL]

## 1. Objective
To demonstrate the OSI seven-layer model through educational encapsulation and decapsulation, using C client-server applications and real TCP socket communication, with a Python user application to display progress.

## 2. Tools and Technologies
- C
- GCC / MinGW-w64
- Python 3
- TCP socket programming
- Visual Studio Code
- Wireshark (optional, for observing the TCP traffic)

## 3. Working Principle
The client adds a labelled simulation header for each of the seven OSI layers, from Application to Physical. The resulting data is transmitted to the server using a real TCP socket. The server removes the headers in reverse order, from Physical to Application, and displays the original message.

## 4. Procedure
1. Open the project folder in VS Code.
2. Confirm GCC and Python are installed.
3. Run `python python/app.py`.
4. Enter a test message.
5. Observe sender encapsulation, TCP transmission, and receiver decapsulation.
6. Record the actual output and take screenshots.

## 5. Results
[Insert actual output and screenshots. Do not invent packet counts or performance values.]

## 6. Discussion
Explain how encapsulation adds layer information and decapsulation removes it. Mention that the socket communication is real TCP, while the seven labelled layer headers are an educational simulation.

## 7. Conclusion
[Write a short conclusion based on your successful test.]

## 8. GitHub Repository
[Paste your repository URL]
