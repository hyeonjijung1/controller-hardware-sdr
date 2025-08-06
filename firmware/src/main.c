/*
 * File:   main.c
 * Authors: ECE295 Design Team 4C: Laura Marinescu, Hyeonji Jung, Chizaram Amobi-Oleka
 *
 * Created on March 1, 2025, 2:50 PM
 *
 */

 #include <avr/io.h>
 #include <util/delay.h>
 #include <avr/interrupt.h>
 #include <string.h>
 #include <stdio.h>
 #include <stdlib.h>
 
 #include "Si5351.h"
 #include "twi.h"
 
 /* GLOBAL VARIABLES AND DEFINES */
 #define PLL_FREQUENCY 800000000UL            // VCO frequency = 800 MHz
 #define INITIAL_OUTPUT_FREQUENCY 10000000UL    // 10 MHz default
 #define BAUD 9600
 #define MYUBRR (F_CPU/16/(BAUD-1))
 
 volatile uint32_t output_frequency = INITIAL_OUTPUT_FREQUENCY;
 volatile uint32_t step_size = 1000000UL;  // Default step = 1 MHz
 volatile uint8_t precision_mode = 0;      // 0: 1MHz, 1: 100kHz, etc.
 volatile int txen = 0;                    // TX state: 0 = RX, 1 = TX
 
 // Additional states for CAT commands:
 volatile uint8_t ai_state = 0;   // For AI command
 volatile uint8_t st_state = 0;   // For ST command
 
 /* FUNCTION PROTOTYPES */
 void configure_si5351(uint32_t frequency);
 void incFreq(void);
 void decFreq(void);
 void toggle_txen(void);
 void change_step_size(void);
 
 void UART_init(unsigned int ubrr);
 void UART_transmit(unsigned char data);
 unsigned char UART_receive(void);
 
 /* CAT protocol functions */
 void comp_ctrl_mode(void);
 static void parseCATCommand(char *cmd);
 static void sendCATResponse(const char *resp);
 
 int main(void) {  
     /* Initialize I2C, Si5351, etc. */
     twi_init();
     si5351_init();
     reset_pll();
     configure_si5351(output_frequency);
     
     /* Initialize UART */
     UART_init(MYUBRR);
     
     /* Initialize pins */
     // Button input pins: PD2, PD3, PD5, PB2
     DDRD &= ~(1 << DDD2);
     DDRD &= ~(1 << DDD3);
     DDRD &= ~(1 << DDD5);
     DDRB &= ~(1 << DDB2);

     // enable pull up resistors on input pins
     PORTD |= (1 << PD2) | (1 << PD3) | (1 << PD5);
     PORTB |= (1 << PB2);
     
     // TXEN output on PD4, start disabled (TX off)
     DDRD |= (1 << DDD4);
     PORTD &= ~(1 << PD4);
     
     // USB-to-UART pins:
     // DTR input on PE2, RXD (PD1) as input, TXD (PD0) as output
     DDRE &= ~(1 << DDE2);
     DDRD &= ~(1 << DDD1);
     DDRD |= (1 << DDD0);
             
     volatile uint8_t b1_value, b2_value, b3_value, b4_value;
     volatile uint8_t dtr_val;
  
     while (1) {
         b1_value = (PIND & (1 << PIND2)) >> PIND2;
         b2_value = (PIND & (1 << PIND3)) >> PIND3;
         b3_value = (PINB & (1 << PINB2)) >> PINB2; // pin B2 is the input of B3 which controls state of TXEN
         b4_value = (PIND & (1 << PIND5)) >> PIND5;
          
         // Check DTR (used to enter CAT mode)
         dtr_val = (PINE & (1 << PINE2)) >> PINE2;
         if (dtr_val) {
             comp_ctrl_mode();
         }
          
         // Button-driven manual controls
         if (b1_value) incFreq();
         if (b2_value) decFreq();
         if (!b3_value) toggle_txen();   // ADDED ! CONDITION. When button is pressed, we should toggle TXEN.
         if (b4_value) change_step_size();
     }
 }
  
 /* Configure Si5351 Frequency */
 void configure_si5351(uint32_t frequency) {
     uint32_t divider = PLL_FREQUENCY / frequency;
     setup_PLL(SI5351_PLL_A, PLL_FREQUENCY / SI5351_XTAL_FREQ, 0, 1);
     setup_clock(SI5351_PLL_A, SI5351_PORT0, divider, 0, 1);
     setup_clock(SI5351_PLL_A, SI5351_PORT1, divider, 0, 1);        // ADDED LINE FOR PORT1
     set_phase(90);
     enable_clocks(true);
 }
  
 /* Increase Frequency */
 void incFreq(void) {
     if (output_frequency + step_size <= 16000000UL) {
         output_frequency += step_size;
         configure_si5351(output_frequency);
     }
 }
  
 /* Decrease Frequency */
 void decFreq(void) {
     if (output_frequency - step_size >= 8000000UL) {
         output_frequency -= step_size;
         configure_si5351(output_frequency);
     }
 }
  
 /* Change Frequency Step Size */
 void change_step_size(void) {
     precision_mode = (precision_mode + 1) % 4;
     switch (precision_mode) {
         case 0: step_size = 1000000UL; break;  // 1 MHz
         case 1: step_size = 100000UL;  break;   // 100 kHz
         case 2: step_size = 10000UL;   break;   // 10 kHz
         case 3: step_size = 1000UL;    break;   // 1 kHz
     }
 }
  
 /* Toggle TX/RX state via TXEN pin */
 void toggle_txen(void) {
     txen ^= 1;
     if (txen) {
         PORTD |= (1 << PD4);   // TX enabled
     } else {
         PORTD &= ~(1 << PD4);  // TX disabled
     }
 }
  
 /* UART Initialization */
 void UART_init(unsigned int ubrr) {
     UBRR0H = (unsigned char)(ubrr >> 8);
     UBRR0L = (unsigned char)ubrr;
     
     // Enable receiver and transmitter.
     UCSR0B = (1 << RXEN) | (1 << TXEN);
     
     // 8 data bits, 1 stop bit, no parity.
     // Set UCSZ1 and UCSZ0 bits to get 8-bit character size.
     UCSR0C = (1 << UCSZ1) | (1 << UCSZ0);
 }
  
 /* UART Transmit */
 void UART_transmit(unsigned char data) {
     while (!(UCSR0A & (1 << UDRE0))) { }
     UDR0 = data;
 }
  
 /* UART Receive */
 unsigned char UART_receive(void) {
     while (!(UCSR0A & (1 << RXC0))) { }
     return UDR0;
 }
  
 //
 // CAT Protocol Handling
 //
  
 /**
  * Reads characters until a semicolon is received (end of command)
  * and then calls parseCATCommand() to process the command.
  */
 void comp_ctrl_mode(void) {
     char rx_buffer[32] = {0};
     uint8_t i = 0;
      
     while ((PINE & (1 << PINE2)) >> PINE2) { // While DTR is active
         char received = UART_receive();
         if (received == ';') {
             rx_buffer[i] = '\0';
             parseCATCommand(rx_buffer);
             i = 0;
             memset(rx_buffer, 0, sizeof(rx_buffer));
         } else {
             if (i < (sizeof(rx_buffer) - 1)) {
                 rx_buffer[i++] = received;
             }
         }
     }
 }
  
 /**
  * Parses CAT commands.
  * Commands supported:
  *   TX – Transmit/Receive state
  *   FA – Frequency set/query (9-digit frequency in Hz)
  *   AI – Additional command state (0 or 1)
  *   ID – Returns static ID ("ID0650;")
  *   MD – Mode query; returns "MD0C;"
  *   SH – Returns "SH0000;"
  *   NA – Returns "NA00;"
  *   IF – Returns status string ("IF001%09lu+000000C00000;")
  *   ST – Secondary state command
  */
 static void parseCATCommand(char *cmd) {
     // TX Command
     if (strncmp(cmd, "TX", 2) == 0) {
         if (cmd[2] == '\0') { // Query
             char resp[8];
             sprintf(resp, "TX%d;", txen);
             sendCATResponse(resp);
         } else {
             if (cmd[2] == '0') {
                 if (txen != 0) { toggle_txen(); }
             } else if (cmd[2] == '1') {
                 if (txen != 1) { toggle_txen(); }
             }
         }
     }
     // FA Command (Frequency)
     else if (strncmp(cmd, "FA", 2) == 0) {
         if (cmd[2] == '\0') { // Query frequency
             char resp[16];
             sprintf(resp, "FA%09lu;", (unsigned long)output_frequency);
             sendCATResponse(resp);
         } else { // Set frequency
             uint32_t newFreq = strtoul(&cmd[2], NULL, 10);
             if (newFreq >= 8000000UL && newFreq <= 16000000UL) {
                 output_frequency = newFreq;
                 configure_si5351(output_frequency);
             }
         }
     }
     // AI Command
     else if (strncmp(cmd, "AI", 2) == 0) {
         if (cmd[2] == '\0') { // Query
             char resp[8];
             sprintf(resp, "AI%d;", ai_state);
             sendCATResponse(resp);
         } else { // Set
             if (cmd[2] == '0' || cmd[2] == '1') {
                 ai_state = cmd[2] - '0';
             }
         }
     }
     // ID Command
     else if (strncmp(cmd, "ID", 2) == 0) {
         sendCATResponse("ID0650;");
     }
     // MD Command
     else if (strncmp(cmd, "MD", 2) == 0) {
         if (cmd[2] == '0') {
             sendCATResponse("MD0C;");
         }
     }
     // SH Command
     else if (strncmp(cmd, "SH", 2) == 0) {
         sendCATResponse("SH0000;");
     }
     // NA Command
     else if (strncmp(cmd, "NA", 2) == 0) {
         sendCATResponse("NA00;");
     }
     // IF Command
     else if (strncmp(cmd, "IF", 2) == 0) {
         char resp[32];
         sprintf(resp, "IF001%09lu+000000C00000;", (unsigned long)output_frequency);
         sendCATResponse(resp);
     }
     // ST Command
     else if (strncmp(cmd, "ST", 2) == 0) {
         if (cmd[2] == '\0') { // Query
             char resp[8];
             sprintf(resp, "ST%d;", st_state);
             sendCATResponse(resp);
         } else {
             if (cmd[2] == '0' || cmd[2] == '1') {
                 st_state = cmd[2] - '0';
             }
         }
     }
     // Unrecognized command: No response
 }
  

 static void sendCATResponse(const char *resp) {
     while (*resp) {
         UART_transmit(*resp++);
     }
 }
 
