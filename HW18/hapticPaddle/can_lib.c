#include "can_lib.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// 500kbit/s at 150MHz = 300 cycles per bit
// Using busy-wait loops calibrated to 150MHz
#define BIT_CYCLES 300
#define BIT_US 8

// Global Variables
static int  same_bit_count;
static uint8_t last_bit;

// Helper Functions
static inline void delay_bit(void) {
    sleep_us(BIT_US);
}

static inline void tx_bit(uint8_t bit) {
    gpio_put(CAN_TX_PIN, bit);
    delay_bit();
}

static inline uint8_t rx_bit(void) {
    uint8_t val = gpio_get(CAN_RX_PIN);
    delay_bit();
    return val;
}

void can_init(void) {
    gpio_init(CAN_TX_PIN);
    gpio_init(CAN_RX_PIN);
    gpio_set_dir(CAN_TX_PIN, GPIO_OUT);
    gpio_set_dir(CAN_RX_PIN, GPIO_IN);
    gpio_put(CAN_TX_PIN, 1);  // recessive (idle)
    sleep_ms(10);
}

// CAN sends a 15-bit CRC for error checking -> created using polynomial 0x4599
static uint16_t can_crc15(uint8_t *data, int len_bits) {
    uint16_t crc = 0;
    // data is packed MSB first; len_bits is total number of bits
    for (int i = 0; i < len_bits; i++) {
        uint8_t byte = data[i / 8];
        uint8_t bit  = (byte >> (7 - (i % 8))) & 1;
        uint16_t msb = (crc >> 14) & 1;
        crc <<= 1;
        crc &= 0x7FFF;
        if (bit ^ msb) crc ^= 0x4599;
    }
    return crc & 0x7FFF;
}

// CAN may fall out of sync when there are too many 1s (no edge detection)
// create_edge will insert the opposite bit after the sent bit if an edge needs to be created
static void create_edge(uint8_t bit) {
    tx_bit(bit);

    if (bit == last_bit) {
        same_bit_count++;
        if (same_bit_count == 5) { // Standard CAN inserts an edge after 5 same bits
            // bit stuffing
            uint8_t stuff_bit = !bit;
            tx_bit(stuff_bit);
            last_bit = stuff_bit;
            same_bit_count = 1;
            return;
        }
    } else {
        same_bit_count = 1;
    }
    last_bit = bit;
}

// send n bits from value, MSB first, with bit stuffing
static void send_bits(uint32_t value, int n) {
    for (int i = n - 1; i >= 0; i--) {
        create_edge((value >> i) & 1);
    }
}

// send raw bit (no stuffing) — used for CRC delimiter, ACK, EOF
static void send_raw(uint8_t bit) {
    tx_bit(bit);
}

// Transmit a classic CAN data frame with an 11-bit standard ID and 4-byte payload
// Returns true if ACK was received from the STM32
bool can_send_float(uint32_t id, float value) {
    // --- build the raw frame bits for CRC calculation ---
    // Classic CAN frame (standard ID, 4 bytes data):
    // SOF(1) + ID(11) + RTR(1) + IDE(1) + r0(1) + DLC(4) + DATA(32) = 51 bits
    // Then CRC is computed over these 51 bits

    uint8_t data_bytes[4];
    memcpy(data_bytes, &value, 4);  // float → bytes, little-endian

    // Pack frame bits for CRC: SOF + ID(11) + RTR + IDE + r0 + DLC(4) + DATA(32)
    // Total = 51 bits = 7 bytes (with 5 bits used in last byte)
    uint8_t frame_bits[7] = {0};

    uint64_t header = 0;
    header = (header << 1) | 0;                    // SOF
    header = (header << 11) | (id & 0x7FF);        // ID
    header = (header << 1) | 0;                    // RTR
    header = (header << 1) | 0;                    // IDE
    header = (header << 1) | 0;                    // r0
    header = (header << 4) | 4;                    // DLC = 4

    // Pack into frame_bits (51 bits total)
    // header is 19 bits, then 32 bits of data
    for (int i = 0; i < 7; i++) frame_bits[i] = 0;

    // write 19 header bits starting at bit 0
    for (int i = 18; i >= 0; i--) {
        int bit_pos = 18 - i;
        frame_bits[bit_pos / 8] |= (((header >> i) & 1) << (7 - (bit_pos % 8)));
    }
    // write 32 data bits starting at bit 19
    for (int b = 0; b < 4; b++) {
        for (int i = 7; i >= 0; i--) {
            int bit_pos = 19 + (b * 8) + (7 - i);
            frame_bits[bit_pos / 8] |= (((data_bytes[b] >> i) & 1) << (7 - (bit_pos % 8)));
        }
    }

    uint16_t crc = can_crc15(frame_bits, 51);

    // --- transmit ---
    same_bit_count = 1;
    last_bit    = 0;  // SOF is dominant, so start count from there

    // SOF — dominant, no stuffing reset needed (start of frame)
    tx_bit(0);
    last_bit    = 0;
    same_bit_count = 1;

    // ID (11 bits)
    send_bits(id & 0x7FF, 11);

    // RTR = 0 (data frame)
    create_edge(0);

    // IDE = 0 (standard frame)
    create_edge(0);

    // r0 = 0
    create_edge(0);

    // DLC = 4
    send_bits(4, 4);

    // DATA (4 bytes, MSB first)
    for (int b = 0; b < 4; b++) {
        send_bits(data_bytes[b], 8);
    }

    // CRC (15 bits, no stuffing after delimiter)
    send_bits(crc, 15);

    // CRC delimiter — recessive, no stuffing
    send_raw(1);

    // ACK slot — send recessive, check if STM32 pulls dominant
    gpio_put(CAN_TX_PIN, 1);  // recessive
    delay_bit();
    bool acked = (gpio_get(CAN_RX_PIN) == 0);  // dominant = ACK received

    // ACK delimiter — recessive
    send_raw(1);

    // EOF — 7 recessive bits
    for (int i = 0; i < 7; i++) send_raw(1);

    // Intermission — 3 recessive bits
    for (int i = 0; i < 3; i++) send_raw(1);

    return acked;
}