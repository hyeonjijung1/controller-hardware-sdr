"""
Subsystem C CAT Command Tester
-----------------------------

Author: Hyeonji Jung
Email: junghyeonji254@gmail.com
Date: 2025-04-06
License: MIT

A Python utility for verifying the CAT (Computer Aided Tuning) command set on the Subsystem C SDR Controller.
Tests set/query commands and validates expected responses over UART.

Requires: pyserial (pip install pyserial)
"""

import argparse
import sys
import time

try:
    import serial
except ImportError:
    print("pyserial is not installed. Install it using: pip install pyserial")
    sys.exit(1)

def parse_args():
    parser = argparse.ArgumentParser(
        description="Subsystem C CAT Protocol Tester - SDR Controller Hardware"
    )
    parser.add_argument('--port', '-p', default='COM11', help='Serial port (e.g., COM11 or /dev/ttyUSB0)')
    parser.add_argument('--baud', '-b', type=int, default=9600, help='Baud rate (default: 9600)')
    parser.add_argument('--timeout', '-t', type=float, default=1, help='Serial timeout (seconds)')
    return parser.parse_args()

class CATTester:
    def __init__(self, port, baud, timeout):
        self.port = port
        self.baud = baud
        self.timeout = timeout
        self.passed = True
        try:
            self.ser = serial.Serial(port=self.port, baudrate=self.baud, timeout=self.timeout)
            if self.ser.isOpen():
                self.ser.close()
            self.ser.open()
        except Exception as e:
            print(f"Failed to open serial port {self.port}: {e}")
            sys.exit(1)

    def close(self):
        self.ser.close()

    def send_and_expect(self, send_cmd, query_cmd, expected):
        """Send a command, then query, and check the expected response."""
        self.ser.write(send_cmd.encode())
        self.ser.write(query_cmd.encode())
        response = self.ser.readline().decode().strip()
        print(f"Sent: {send_cmd.strip()} | Queried: {query_cmd.strip()} | Got: {response} | Expect: {expected}")
        if response == expected:
            print("  [PASS]\n")
        else:
            print("  [FAIL]\n")
            self.passed = False

    def query_and_expect(self, query_cmd, expected):
        """Send only a query and check the response."""
        self.ser.write(query_cmd.encode())
        response = self.ser.readline().decode().strip()
        print(f"Queried: {query_cmd.strip()} | Got: {response} | Expect: {expected}")
        if response == expected:
            print("  [PASS]\n")
        else:
            print("  [FAIL]\n")
            self.passed = False

    def run_tests(self):
        print(f"========== Subsystem C CAT Protocol Test ==========")
        print(f"Serial Port: {self.port} | Baud: {self.baud} | Timeout: {self.timeout}s\n")

        # FA: Frequency set/query
        fa_val = 14074000
        fa_set = f"FA{fa_val:09d};"
        print("Frequency Set/Query Test:")
        self.send_and_expect(fa_set, "FA;", fa_set)

        # TX: Transmit mode ON/OFF
        print("Transmit Command Test:")
        self.send_and_expect("TX1;", "TX;", "TX1;")
        self.send_and_expect("TX0;", "TX;", "TX0;")

        # AI: Auto Info ON/OFF
        print("Auto Info Test:")
        self.send_and_expect("AI1;", "AI;", "AI1;")
        self.send_and_expect("AI0;", "AI;", "AI0;")

        # ID: Device ID Query
        print("Device ID Query:")
        self.query_and_expect("ID;", "ID0650;")

        # MD: Mode Query
        print("Mode Query:")
        self.query_and_expect("MD0;", "MD0C;")

        # SH: Shift Query
        print("Shift Query:")
        self.query_and_expect("SH0;", "SH0000;")

        # NA: Narrow Filter Query
        print("Narrow Filter Query:")
        self.query_and_expect("NA0;", "NA00;")

        # IF: Interface Data Query
        print("Interface Data Query:")
        self.query_and_expect("IF;", "IF001014074000+000000C00000;")

        # ST: Split Mode
        print("Split Mode Test:")
        self.send_and_expect("ST1;", "ST;", "ST1;")
        self.send_and_expect("ST0;", "ST;", "ST0;")

        print("===================================================")
        print("\nOverall Result: {}".format("ALL TESTS PASSED ✔️" if self.passed else "SOME TESTS FAILED ❌"))

def main():
    args = parse_args()
    tester = CATTester(args.port, args.baud, args.timeout)
    try:
        tester.run_tests()
    finally:
        tester.close()

if __name__ == '__main__':
    main()