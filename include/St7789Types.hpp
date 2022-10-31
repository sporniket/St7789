// Copyright 2022 David SPORN
// ---
// This file is part of 'St7789'.
// ---
// 'St7789' is free software: you can redistribute it and/or 
// modify it under the terms of the GNU General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.

// 'St7789' is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General 
// Public License for more details.

// You should have received a copy of the GNU General Public License along 
// with 'St7789'. If not, see <https://www.gnu.org/licenses/>. 
#ifndef ST7789TYPES_HPP
#define ST7789TYPES_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes

#define SIZE_OF_POINTER_TO_BYTE_ARRAY (sizeof(uint8_t *))

#define SHOULD_USE_INTERNAL_BUFFER(x) ((x) <= (sizeof(uint8_t *)))

#define MUST_USE_EXTERNAL_BUFFER(x) ((sizeof(uint8_t *)) < (x))

enum St7789Opcode {
    NOP = 0x00,
    SWRESET = 0x01,
    SLPOUT = 0x11,
    NORON = 0x13,
    INVON = 0x21,
    DISPON = 0x29,
    CASET = 0x2a,
    RASET = 0x2b,
    RAMWR = 0x2c,
    MADCTL = 0x36,
    COLMOD = 0x3a,
    RAMWRC = 0x3c,
    PORCTRL = 0xb2,
    GCTRL = 0xb7,
    VCOM = 0xbb,
    LCMCTRL = 0xc0,
    VDVVRHEN = 0xc2,
    VRHS = 0xc3,
    VDVS = 0xc4,
    FRCTRL2 = 0xc6,
    PWCTRL1 = 0xd0,
    PVGAMCTRL = 0xe0,
    NVGAMCTRL = 0xe1,
};

enum St7789PixelFormat { BPP12 = 0x03, BPP16 = 0x54, BPP18 = 0x55 };

/**
 * @brief human friendly setup of the screen orientation, will be translated into MADCTL flags.
 *
 */
enum St7789Orientation { NORTH, EAST, WEST, SOUTH };

/**
 * @brief attached to a native transaction to tell the pre-transaction listener what the transaction is.
 */
enum St7789TransactionNature {
    /**
     * @brief D/C pin = 0 ; R/W pin = 0.
     */
    COMMAND,
    /**
     * @brief D/C pin = 1 ; R/W pin = 1.
     */
    DATA_READ,
    /**
     * @brief D/C pin = 1 ; R/W pin = 0.
     */
    DATA_WRITE,
};

/**
 * @brief Tells whether a command will have data to be read from or written to the controller ; when there is no data,
 * there is no direction.
 *
 */
enum St7789CommandDirection { READ_7789, WRITE_7789, NONE_7789 };

/**
 * @brief Describe a command : opcode and data buffer to send or buffer to receive.
 *
 */
struct St7789Command {
    /**
     * @brief the opcode to send as a COMMAND transaction.
     *
     */
    St7789Opcode opcode;

    /**
     * @brief Length of the DATA transaction.
     *
     */
    uint32_t dataLength;

    /**
     * @brief How data will move during the DATA transaction.
     *
     */
    St7789CommandDirection dataDirection;

    /**
     * @brief if true, do not delete external buffer (allocated by the developper), meaningless when using internal
     * buffer.
     *
     */
    bool keepExternalBufferOnDelete;

    /**
     * @brief Depending on the length of data.
     *
     */
    union {
        /**
         * @brief When the length of data is shorter than the size of a pointer, store the data in the structure.
         */
        uint8_t internalBuffer[SIZE_OF_POINTER_TO_BYTE_ARRAY];
        /**
         * @brief When the length of data is longer than the size of a pointer, point to an external buffer to be
         * managed.
         */
        uint8_t *externalBuffer;
    };
};

#endif