
"""
SDR Local Oscillator Auto-Validation Script
-------------------------------------------
Author: Hyeonji Jung
Email: junghyeonji254@gmail.com

Automates voltage and frequency measurement of the SDR Local Oscillator using
the Keysight EDU34450A Digital Multimeter via SCPI commands (PyVISA).

- Logs results to CSV
- Checks measured values against pass/fail tolerances
- Clear output and robust error handling

Requires: pyvisa (`pip install pyvisa`)
"""

import pyvisa
import time
import csv
import sys

DMM_ADDRESS = 'USB0::0x2A8D::0x3502::MY12345678::INSTR'

# Validation specs
EXPECTED_FREQ = 10_000_000     # 10 MHz
FREQ_TOL = 1000                # ±1 kHz
EXPECTED_VOLT = 1.0            # Example expected voltage (V)
VOLT_TOL = 0.1                 # ±0.1 V

CSV_LOG = "lo_validation_results.csv"

def log_result(filename, result_row):
    """Append a measurement result to CSV."""
    with open(filename, 'a', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(result_row)

def main():
    try:
        rm = pyvisa.ResourceManager()
        dmm = rm.open_resource(DMM_ADDRESS)
        print("Connected to DMM:", dmm.query("*IDN?").strip())
    except Exception as e:
        print(f"Could not connect to DMM ({DMM_ADDRESS}): {e}")
        sys.exit(1)

    # Initialize instrument
    try:
        dmm.write("*RST")
        time.sleep(0.5)
    except Exception as e:
        print(f"Could not reset DMM: {e}")
        sys.exit(1)

    # Measure DC Voltage
    try:
        dmm.write("CONF:VOLT:DC AUTO")
        voltage = float(dmm.query("MEAS:VOLT:DC?"))
    except Exception as e:
        print(f"Voltage measurement failed: {e}")
        voltage = None

    # Measure Frequency
    try:
        dmm.write("CONF:FREQ")
        freq = float(dmm.query("MEAS:FREQ?"))
    except Exception as e:
        print(f"Frequency measurement failed: {e}")
        freq = None

    # Show Results
    print("\n=== SDR LO Validation Results ===")
    print(f"Measured Voltage:   {voltage:.3f} V" if voltage is not None else "Voltage: N/A")
    print(f"Measured Frequency: {freq:.1f} Hz" if freq is not None else "Frequency: N/A")

    volt_pass = voltage is not None and abs(voltage - EXPECTED_VOLT) <= VOLT_TOL
    freq_pass = freq is not None and abs(freq - EXPECTED_FREQ) <= FREQ_TOL

    print(f"Voltage PASS:   {volt_pass}")
    print(f"Frequency PASS: {freq_pass}")

    # Log result to CSV
    log_result(CSV_LOG, [
        time.strftime('%Y-%m-%d %H:%M:%S'),
        voltage if voltage is not None else "N/A",
        freq if freq is not None else "N/A",
        "PASS" if volt_pass else "FAIL",
        "PASS" if freq_pass else "FAIL"
    ])

    dmm.close()
    rm.close()
    print(f"\nResults logged to {CSV_LOG}")

if __name__ == '__main__':
    main()
