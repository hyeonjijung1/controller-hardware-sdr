# controller-hardware-sdr[sdr_readme.md](https://github.com/user-attachments/files/21612695/sdr_readme.md)
# 📡 Subsystem C: SDR Controller Hardware

A robust hardware controller built for **Software-Defined Radio (SDR)** systems, integrating **Local Oscillator (LO) generation**, **transmit/receive control**, and a **user interface** to support modular, high-accuracy signal processing.

---

## 🔧 Project Summary

This project centers around the design and validation of Subsystem C within a larger SDR framework. The subsystem focuses on generating a stable LO signal, managing RX/TX switching logic, and exposing a simple UI to enable operator-level control.

- 🔩 **Platform:** Custom 2-layer PCB (Altium Designer)
- 🧠 **MCU:** ATMEGA324PB
- 📡 **Signal Source:** Si5351A programmable clock generator
- 🎛️ **UI:** Push buttons + 16x2 I2C LCD
- 🛠️ **Validation Tools:** Oscilloscope, Function Generator

---

## ✨ Key Contributions

- Designed and implemented the **Local Oscillator (LO) subsystem** to generate a stable **10 MHz signal** for downstream SDR signal mixing.
- Developed a **user-friendly interface** featuring **mode toggles, frequency control**, and real-time display feedback.
- Programmed embedded C firmware for **Si5351A via I2C**, UART **CAT protocol**, and **LCD control**, enabling seamless system-level interaction.
- Wrote **Python/shell scripts** to automate testing of command response and LO accuracy.
- Performed board bring-up, soldering, and debug—achieving **±1 kHz LO stability** and **90° I/Q phase accuracy**.
- Led a **3-member team** through subsystem development, documentation, and integration.
- Delivered the **M2 technical design presentation** as team lead and presenter, covering architecture, timing, and ICD alignment.

---

## 🧠 Architecture

Subsystem C is architected around a centralized microcontroller managing three tightly-coupled components:

- **LO Signal Path:** Si5351A configured over I2C to produce a configurable 10 MHz clock
- **UI Layer:** Push buttons used to increment/decrement frequency, toggle TX/RX
- **TX Control Logic:** TXEN signal gated by UI and forwarded to external power amplifier

The microcontroller operates with clean FSM transitions and supports real-time adjustments during runtime.

---

## 🛠️ PCB Design

- **Top View:**
  ![Top](https://github.com/hyeonjijung1/Past_Project/blob/main/Screenshot%202025-03-07%20235327.png)

- **Bottom View:**
  ![Bottom](https://github.com/hyeonjijung1/Past_Project/blob/main/Screenshot%202025-03-07%20235332.png)

- **3D Model:**
  ![3D](https://github.com/hyeonjijung1/Past_Project/blob/main/Screenshot%202025-03-07%20235401.png)

---

## 📐 Schematic Snapshot

![Schematic](https://github.com/hyeonjijung1/Past_Project/blob/main/Screenshot%202025-03-08%20003007.png)

---

## 📊 Block Diagram

![Block Diagram](https://github.com/hyeonjijung1/Past_Project/blob/main/Screenshot%202025-03-08%20204710.png)

---

## 📂 Supporting Materials

- [📄 Technical Design Document](https://github.com/hyeonjijung1/Past_Project/blob/main/ECE295_Team4C_Technical_Design_Doc.pdf)
- [🧾 Wire Definitions + ICD Alignment](https://github.com/hyeonjijung1/Past_Project/blob/main/ECE295_Team4C_WD1.pdf)
- [📽️ M2 Technical Presentation Slides](https://github.com/hyeonjijung1/Past_Project/blob/main/M2_design_review_slides.pdf)
- [🎤 General Audience OP1 Slides](https://github.com/hyeonjijung1/Past_Project/blob/main/Team4C_OP1_Slides.pdf)
- [🎤 General Audience OP2 Slides](https://github.com/hyeonjijung1/Past_Project/blob/main/Team4C_OP2_Slides.pdf)

---

## 🧠 What We Learned

- Managing multi-protocol I/O (I2C + UART + GPIO) on a low-pin-count MCU
- Tuning signal chain accuracy within noise and tolerance margins
- Adhering to real-world integration specs via **Interface Control Documents (ICDs)**
- Planning PCB routing to isolate analog/RF pathways from noisy digital blocks
- Leading cross-functional communication between subsystem teams under tight timelines

---

## 💬 About

Led by **[Hyeonji Jung](https://www.linkedin.com/in/hyeonjijung-uoft/)** — Subsystem C Lead, Embedded Systems Designer, and M2 Technical Presenter.

For questions or collaboration, contact: **junghyeonji254@gmail.com**

