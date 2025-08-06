# Subsystem C: SDR Controller Hardware  
_Led a 3-person team and delivered the Technical Review Presentation as Team Lead_

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

A high-performance hardware controller built for **Software-Defined Radio (SDR)** systems, integrating **Local Oscillator (LO) generation**, **TX/RX control**, and a **user interface** to support modular, high-accuracy signal processing.

---

## Project Summary

This repository contains the design and firmware for **Subsystem C** of a larger SDR transceiver. It handles:

- Generating a stable, precision **10 MHz LO** for quadrature mixers  
- **TX/RX gating** via an active-low enable signal  
- A **front-panel UI** (push-buttons + 16×2 I²C LCD) for frequency tuning and mode switching  
- Full compliance with the course **Interface Control Document (ICD)**  

**Platform:** Custom 2-layer PCB (Altium Designer)  
**MCU:** ATmega324PB  
**Clock Generator:** Si5351A (I²C-programmable)  
**UI:** Push buttons + 16×2 character I²C LCD  
**Validation Tools:** Oscilloscope, Function Generator  

---

## Key Contributions & Impact

- **±1 kHz LO stability** (0.01% error) and **90° I/Q phase accuracy** under lab conditions  
- **50% faster** bring-up cycles using automated Python/bash test scripts  
- Designed & PCB-routed a **compact 128 × 51 mm** daughtercard, isolating RF traces from digital noise  
- Authored the **Technical Design Document**, detailing architecture, component trade-offs, and test plans  
- Programmed embedded C for **Si5351A (I²C)**, **UART (CAT protocol)**, and **LCD control**  
- **Led a 3-member team** end-to-end: schematic → PCB → firmware → integration  
- **Presented** the M2 Technical Review, covering timing closure, signal integrity, and ICD alignment  

---

## Getting Started

```bash
# Clone the Subsystem C repo
git clone https://github.com/hyeonjijung1/Controller-Hardware-for-SDR-Local-Oscillator-User-Interface.git
cd Controller-Hardware-for-SDR-Local-Oscillator-User-Interface

# Build firmware
make all

# Flash the ATmega324PB (via AVRDUDE or your preferred tool)
make flash
```
Once programmed, power the mainboard at 12 VDC, attach your UI module to J7/J8, and observe the LCD. Use the push-buttons to tune frequency and toggle TX/RX.

---

## 📁 Repo Structure

```bash
Controller-Hardware-for-SDR-Local-Oscillator-User-Interface/
├── hardware/                # Altium project files & PCB source
│   ├── SubsystemC.PrjPcb
│   └── SubsystemC.PrjSch
├── firmware/                # AVR C source & build scripts
│   ├── src/
│   │   ├── main.c
│   │   ├── si5351.c
│   │   └── uart_cat.c
│   ├── include/
│   └── Makefile
├── docs/                    # PDF deliverables & slides
│   ├── ECE295_Team4C_Technical_Design_Doc.pdf
│   ├── ECE295_Team4C_WD1.pdf
│   ├── M2_design_review_slides.pdf
│   └── Team4C_OP1_Slides.pdf
├── scripts/                 # Test automation & helpers
│   └── test_lo_accuracy.sh
├── images/                  # Screenshots used in README
├── LICENSE
└── README.md
```

---

## Architecture

![Block Diagram](https://github.com/hyeonjijung1/Past_Project/blob/main/Screenshot%202025-03-08%20204710.png)
*Figure 1. Subsystem C SDR Block Diagram.*

### PCB Design:

- **3D View:**  
  ![PCB 3D](https://github.com/hyeonjijung1/Past_Project/blob/main/Screenshot%202025-03-07%20235401.png)
- **Top View:**  
  ![PCB Top](https://github.com/hyeonjijung1/Past_Project/blob/main/Screenshot%202025-03-07%20235327.png)
- **Bottom View:**  
  ![PCB Bottom](https://github.com/hyeonjijung1/Past_Project/blob/main/Screenshot%202025-03-07%20235332.png)

### Schematic:

![Altium Schematic](https://github.com/hyeonjijung1/Past_Project/blob/main/Screenshot%202025-03-08%20003007.png)

## Supporting Materials

- 📄 [Technical Design Document (PDF)](https://github.com/hyeonjijung1/Past_Project/raw/main/ECE295_Team4C_Technical_Design_Doc.pdf)  
- 🧾 [Wire Definitions & ICD Alignment (PDF)](https://github.com/hyeonjijung1/Past_Project/raw/main/ECE295_Team4C_WD1.pdf)  
- 🎤 [M2 Technical Presentation Slides (PDF)](https://github.com/hyeonjijung1/Past_Project/raw/main/M2_design_review_slides.pdf)  
- 🎤 [OP1 General Audience Slides (PDF)](https://github.com/hyeonjijung1/Past_Project/raw/main/Team4C_OP1_Slides.pdf)  
- 🎤 [OP2 General Audience Slides (PDF)](https://github.com/hyeonjijung1/Past_Project/raw/main/Team4C_OP2_Slides.pdf)  

## What We Learned

- Orchestrating multi-protocol I/O (I²C + UART + GPIO) on a constrained MCU  
- Achieving tight frequency tolerance within noise and thermal drift margins  
- ICD-driven design: enforcing electrical & mechanical interface specs across teams  
- PCB routing strategies to isolate analog/RF from noisy digital sections  
- Leadership: coordinating cross-functional goals, documentation, and presentations under deadline  

## Contact & Contributors

**Lead Engineer & Presenter:**  
Hyeonji Jung • junghyeonji254@gmail.com  

**Contributors:**  
- Laura Marinescu  
- Chizaram Amobi-Oleka  
