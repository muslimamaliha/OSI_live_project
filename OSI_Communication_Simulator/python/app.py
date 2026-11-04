import os
import platform
import shutil
import subprocess
import sys
import threading
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC = ROOT / "src"
BIN = ROOT / "bin"
BIN.mkdir(exist_ok=True)

IS_WINDOWS = platform.system() == "Windows"
SERVER = BIN / ("server.exe" if IS_WINDOWS else "server")
CLIENT = BIN / ("client.exe" if IS_WINDOWS else "client")

def compile_programs():
    gcc = shutil.which("gcc")
    if not gcc:
        print("ERROR: GCC was not found. Install MinGW-w64 (Windows) or GCC (Linux).")
        sys.exit(1)

    common_flags = ["-Wall", "-Wextra"]
    server_cmd = [gcc, *common_flags, str(SRC / "server.c"), "-o", str(SERVER)]
    client_cmd = [gcc, *common_flags, str(SRC / "client.c"), "-o", str(CLIENT)]
    if IS_WINDOWS:
        server_cmd += ["-lws2_32"]
        client_cmd += ["-lws2_32"]

    for cmd in (server_cmd, client_cmd):
        result = subprocess.run(cmd, cwd=ROOT, text=True)
        if result.returncode != 0:
            print("Compilation failed. Check the GCC installation and source files.")
            sys.exit(result.returncode)
    print("[BUILD] Client and Server compiled successfully.\n")

def stream_server_output(process, ready_event):
    for line in iter(process.stdout.readline, ""):
        print(line.rstrip())
        if "Listening on port" in line:
            ready_event.set()
    process.stdout.close()

def main():
    print("=" * 62)
    print("             OSI 7-LAYER COMMUNICATION SIMULATOR")
    print("=" * 62)
    print("This project simulates all seven OSI layers and uses a real TCP socket.")
    print("Note: OSI layer headers are educational simulation headers.\n")

    compile_programs()
    message = input("Enter a message to send: ").strip()
    if not message:
        message = "Hello from the OSI Communication Simulator"
    if len(message) > 3000:
        print("Please enter 3000 characters or fewer.")
        return

    server = subprocess.Popen(
        [str(SERVER)], cwd=ROOT, stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT, text=True, bufsize=1
    )
    ready = threading.Event()
    reader = threading.Thread(target=stream_server_output, args=(server, ready), daemon=True)
    reader.start()

    if not ready.wait(timeout=5):
        print("[ERROR] Server did not start. Check whether port 5050 is already in use.")
        server.terminate()
        return

    print("\n" + "-" * 62)
    print("[PYTHON USER APPLICATION] Starting C Client...\n")
    client_result = subprocess.run(
        [str(CLIENT), "127.0.0.1", message],
        cwd=ROOT, text=True
    )

    time.sleep(0.5)
    if server.poll() is None:
        server.terminate()
    try:
        server.wait(timeout=3)
    except subprocess.TimeoutExpired:
        server.kill()

    print("\n" + "-" * 62)
    if client_result.returncode == 0:
        print("[PYTHON USER APPLICATION] Simulation completed.")
    else:
        print("[PYTHON USER APPLICATION] Simulation failed. Review the messages above.")
    print("=" * 62)

if __name__ == "__main__":
    main()
