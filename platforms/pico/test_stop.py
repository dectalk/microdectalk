#!/usr/bin/env python3
"""
Test script for the 0x90 stop edge case.
Tests: mid-synthesis stop, stop-then-text (\x90Hello World), and spam stops.

Usage: python3 test_stop.py [port]
Default port: /dev/ttyACM1
"""

import serial
import threading
import time
import sys

PORT = sys.argv[1] if len(sys.argv) > 1 else '/dev/ttyACM1'
BAUD = 115200  # USB CDC — baud rate is ignored by the kernel but must be set

RESET = b'\x90'
LONG_TEXT = b"This is a very long sentence that the engine should still be speaking when we interrupt it with a stop command.\n"
STOP_THEN_TEXT = b"\x90Hello World, the quick brown fox jumps over the lazy dog\n"

def ts():
    return f"{time.time():.3f}"

# Reader thread: prints all serial output with timestamps
def reader(ser, done):
    buf = b""
    while not done.is_set():
        n = ser.in_waiting
        if n:
            buf += ser.read(n)
            while b"\n" in buf:
                line, buf = buf.split(b"\n", 1)
                print(f"  [{ts()}] {line.decode('utf-8', errors='replace')}")
        else:
            time.sleep(0.005)
    # flush remainder
    if buf:
        print(f"  [{ts()}] {buf.decode('utf-8', errors='replace')}")

def send(ser, data, label):
    print(f"\n[{ts()}] SEND: {label!r}")
    ser.write(data)
    ser.flush()

def wait(seconds, reason):
    print(f"[{ts()}] waiting {seconds}s — {reason}")
    time.sleep(seconds)

def run_tests(ser):
    # ------------------------------------------------------------------ #
    # Test 1: stop with nothing being synthesised (idle stop edge case)   #
    # ------------------------------------------------------------------ #
    print("\n=== TEST 1: stop while idle ===")
    send(ser, RESET, "0x90 while idle")
    wait(1, "expect: core1 clears stop_requested while idle, core0 unblocks")

    # ------------------------------------------------------------------ #
    # Test 2: \x90 immediately followed by text in the same write         #
    # ------------------------------------------------------------------ #
    print("\n=== TEST 2: stop then text in one write ===")
    send(ser, STOP_THEN_TEXT, r"\x90Hello World...")
    wait(3, "expect: Hello World spoken, not silenced")

    # ------------------------------------------------------------------ #
    # Test 3: stop mid-synthesis                                          #
    # ------------------------------------------------------------------ #
    print("\n=== TEST 3: stop mid-synthesis ===")
    send(ser, LONG_TEXT, "long sentence")
    wait(0.3, "let synthesis start")
    send(ser, RESET + b"Interrupted.\n", "0x90 + new text mid-speech")
    wait(3, "expect: long sentence cut off, 'Interrupted.' spoken")

    # ------------------------------------------------------------------ #
    # Test 4: spam stops                                                  #
    # ------------------------------------------------------------------ #
    print("\n=== TEST 4: spam stops ===")
    send(ser, LONG_TEXT, "long sentence before spam")
    wait(0.2, "let synthesis start")
    for i in range(5):
        send(ser, RESET, f"spam stop #{i+1}")
        time.sleep(0.05)
    send(ser, b"Still alive after spam.\n", "recovery check")
    wait(3, "expect: engine still functional, recovery text spoken")

    # ------------------------------------------------------------------ #
    # Test 5: the original edge case verbatim                             #
    # ------------------------------------------------------------------ #
    print("\n=== TEST 5: original edge case (verbatim) ===")
    send(ser, b"\n\n\x90Hello World, the quick brown fox jumps over the lazy dog\n\n",
         r"\n\n\x90Hello World...")
    wait(4, "expect: Hello World + fox sentence spoken")

def main():
    print(f"Opening {PORT} at {BAUD} baud")
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
    except serial.SerialException as e:
        print(f"Failed to open port: {e}")
        sys.exit(1)

    done = threading.Event()
    t = threading.Thread(target=reader, args=(ser, done), daemon=True)
    t.start()

    # Wait for the device to print READY before sending any tests
    #print(f"[{ts()}] waiting for device READY...")
    #deadline = time.time() + 30
    #while time.time() < deadline:
    #    if ser.in_waiting:
    #        line = ser.read_until(b"\n").decode("utf-8", errors="replace").strip()
    #        print(f"  [{ts()}] {line}")
    #        if "READY" in line:
    #            break
    #    time.sleep(0.05)
    #else:
    #    print("ERROR: timed out waiting for READY")
    #    done.set()
    #    sys.exit(1)
    #print(f"[{ts()}] device ready, starting tests\n")

    try:
        run_tests(ser)
    finally:
        wait(1, "final flush")
        done.set()
        t.join(timeout=2)
        ser.close()
        print(f"\n[{ts()}] done")

if __name__ == '__main__':
    main()
