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
#ifndef ST7789_HPP
#define ST7789_HPP

// standard includes
#include <cstdint>
#include <vector>

// esp32 includes
#include "esp_log.h"

// project includes
#include "St7789Job.hpp"
#include "St7789Types.hpp"

class St7789 {
    private:
    // FIXME a way to manage a buffer of small data chunk, for messages longer than the length of a pointer.

    /**
     * @brief When preparing variable length command, the external buffer is unmanaged ; and if the message happens to
     * be short, it is copied into the internal buffer instead.
     *
     * @param command the command being setup.
     * @param size the size of the message.
     * @param buffer the location of the message (unmanaged memory allocation).
     */
    void setupMessageFromUnmanagedMemory(St7789Command *command, uint32_t size, uint8_t *buffer) {
        if (MUST_USE_EXTERNAL_BUFFER(size)) {
            command->keepExternalBufferOnDelete = true; // the developper MUST manage the buffer
            command->externalBuffer = buffer;
        } else {
            uint8_t *message = command->internalBuffer;
            for (int i = 0; i < size; i++) {
                message[i] = buffer[i];
            }
        }
    }

    uint8_t *getBufferUsingManagedBufferIfNeeded(St7789Command *command) {
        if (MUST_USE_EXTERNAL_BUFFER(command->dataLength)) {
            // create and fill external buffer
            command->keepExternalBufferOnDelete = false; // managed buffer
            command->externalBuffer = new uint8_t[command->dataLength];
            return command->externalBuffer;
        } else {
            return command->internalBuffer;
        }
    }

    St7789Command *newCommand(St7789Opcode opcode, uint32_t dataLength, St7789CommandDirection dataDirection,
                              bool keepExternalBufferOnDelete) {
        St7789Command *command = new St7789Command;
        command->opcode = opcode;
        command->dataLength = dataLength;
        command->dataDirection = dataDirection;
        command->keepExternalBufferOnDelete = keepExternalBufferOnDelete;
        return command;
    }

    public:
    virtual ~St7789(){};
    /**
     * @brief Perform the operation as non blocking SPI transactions, returns when it is finished.
     *
     * @param command
     */
    virtual void schedule(St7789Command *command) = 0;
    virtual void schedule(std::vector<St7789Command *> *sequence) = 0;
    virtual void await(St7789Command *command) = 0;
    virtual void await(std::vector<St7789Command *> *sequence) = 0;
    St7789Command *nop() {
        St7789Command *command = newCommand(NOP, 0, NONE_7789, false);
        return command;
    }
    St7789Command *swreset() {
        St7789Command *command = newCommand(SWRESET, 0, NONE_7789, false);
        return command;
    }
    St7789Command *slpout() {
        St7789Command *command = newCommand(SLPOUT, 0, NONE_7789, false);
        return command;
    }
    St7789Command *noron() {
        St7789Command *command = newCommand(NORON, 0, NONE_7789, false);
        return command;
    }
    St7789Command *invon() {
        St7789Command *command = newCommand(INVON, 0, NONE_7789, false);
        return command;
    }
    St7789Command *dispon() {
        St7789Command *command = newCommand(DISPON, 0, NONE_7789, false);
        return command;
    }
    St7789Command *caset(uint16_t xs, uint16_t xe) {
        St7789Command *command = newCommand(CASET, 4, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = xs >> 8 & 0xff;
        message[1] = xs & 0xff;
        message[2] = xe >> 8 & 0xff;
        message[3] = xe & 0xff;
        return command;
    }
    St7789Command *raset(uint16_t ys, uint16_t ye) {
        St7789Command *command = newCommand(RASET, 4, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = ys >> 8 & 0xff;
        message[1] = ys & 0xff;
        message[2] = ye >> 8 & 0xff;
        message[3] = ye & 0xff;
        return command;
    }
    St7789Command *ramwr(uint32_t size, uint8_t *buffer) {
        St7789Command *command = newCommand(RAMWR, size, WRITE_7789, false);
        setupMessageFromUnmanagedMemory(command, size, buffer);
        return command;
    }
    St7789Command *madctl(uint8_t mode) {
        St7789Command *command = newCommand(MADCTL, 1, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = mode & 0xff;
        message[1] = 0; // FIXME fill only up to size of pointer
        message[2] = 0;
        message[3] = 0;
        return command;
    }
    St7789Command *colmod(St7789PixelFormat f) {
        St7789Command *command = newCommand(COLMOD, 1, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = f & 0xff;
        message[1] = 0; // FIXME fill only up to size of pointer
        message[2] = 0;
        message[3] = 0;
        return command;
    }
    St7789Command *ramwrc(uint32_t size, uint8_t *buffer) {
        St7789Command *command = newCommand(RAMWRC, size, WRITE_7789, false);
        setupMessageFromUnmanagedMemory(command, size, buffer);
        return command;
    }
    St7789Command *porctrl(uint8_t backPorch, uint8_t frontPorch, bool enable, uint8_t backPorchIdle,
                           uint8_t frontPorchIdle, uint8_t backPorchPartial, uint8_t frontPorchPartial) {
        St7789Command *command = newCommand(PORCTRL, 5, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = backPorch & 0xff;
        message[1] = frontPorch & 0xff;
        message[2] = enable ? 0x01 : 0x00;
        message[4] = ((backPorchIdle << 4) & 0xf0) | (frontPorchIdle & 0x0f);
        message[3] = ((backPorchPartial << 4) & 0xf0) | (backPorchPartial & 0x0f);
        return command;
    }
    St7789Command *gctrl(uint8_t vghSetting, uint8_t vglSetting) {
        St7789Command *command = newCommand(GCTRL, 1, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = ((vghSetting << 4) & 0xf0) | (vglSetting & 0x0f);
        message[1] = 0; // FIXME fill only up to size of pointer
        message[2] = 0;
        message[3] = 0;
        return command;
    }
    St7789Command *vcom(uint8_t vcomSetting) {
        St7789Command *command = newCommand(VCOM, 1, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = vcomSetting & 0xff;
        message[1] = 0; // FIXME fill only up to size of pointer
        message[2] = 0;
        message[3] = 0;
        return command;
    }
    St7789Command *lcmctrl(uint8_t lcmSetting) {
        St7789Command *command = newCommand(LCMCTRL, 1, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = lcmSetting & 0xff;
        message[1] = 0; // FIXME fill only up to size of pointer
        message[2] = 0;
        message[3] = 0;
        return command;
    }
    St7789Command *vdvvrhen(bool enable) {
        St7789Command *command = newCommand(VDVVRHEN, 2, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = enable ? 0x01 : 0x00;
        message[1] = 0xff;
        message[2] = 0; // FIXME fill only up to size of pointer
        message[3] = 0;
        return command;
    }
    St7789Command *vrhs(uint8_t vrhSetting) {
        St7789Command *command = newCommand(VRHS, 1, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = vrhSetting & 0xff;
        message[1] = 0; // FIXME fill only up to size of pointer
        message[2] = 0;
        message[3] = 0;
        return command;
    }
    St7789Command *vdvs(uint8_t vdvSetting) {
        St7789Command *command = newCommand(VDVS, 1, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = vdvSetting & 0xff;
        message[1] = 0; // FIXME fill only up to size of pointer
        message[2] = 0;
        message[3] = 0;
        return command;
    }
    St7789Command *frctrl2(uint8_t nla, uint8_t rtna) {
        St7789Command *command = newCommand(FRCTRL2, 1, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = ((nla << 5) & 0xe0) | (rtna & 0x1f);
        message[1] = 0; // FIXME fill only up to size of pointer
        message[2] = 0;
        message[3] = 0;
        return command;
    }
    St7789Command *pwctrl1(uint8_t avdd, uint8_t avcl, uint8_t vds) {
        St7789Command *command = newCommand(PWCTRL1, 2, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = 0xa4;
        message[1] = ((avdd << 6) & 0xc0) | ((avcl << 4) & 0x30) | (vds & 0x3);
        message[2] = 0; // FIXME fill only up to size of pointer
        message[3] = 0;
        return command;
    }

    /**
     * @brief Positive Voltage Gamma Control : The gamma correction is performed with 3 groups of registers, which are
     * gradient adjustment, contrast adjustment and fine- adjustment registers for positive and negative polarities, and
     * RGB can be adjusted individually.
     *
     * @param v0 [3:0]
     * @param v1 [5:0]
     * @param v2 [5:0]
     * @param v4 [4:0]
     * @param v6 [4:0]
     * @param v13 [3:0]
     * @param v20 [6:0]
     * @param v27 [2:0]
     * @param v36 [2:0]
     * @param v43 [6:0]
     * @param v50 [3:0]
     * @param v57 [4:0]
     * @param v59 [4:0]
     * @param v61 [5:0]
     * @param v62 [5:0]
     * @param v63 [3:0]
     * @param j0 [1:0]
     * @param j1 [1:0]
     * @return St7789Command* the command.
     */
    St7789Command *pvgamctrl(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v4, uint8_t v6, uint8_t v13, uint8_t v20,
                             uint8_t v27, uint8_t v36, uint8_t v43, uint8_t v50, uint8_t v57, uint8_t v59, uint8_t v61,
                             uint8_t v62, uint8_t v63, uint8_t j0, uint8_t j1) {
        St7789Command *command = newCommand(PVGAMCTRL, 14, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = ((v63 & 0x0f) << 4) | (v0 & 0x0f);
        message[1] = (v1 & 0x3f);
        message[2] = (v2 & 0x3f);
        message[3] = (v4 & 0x1f);
        message[4] = (v6 & 0x1f);
        message[5] = ((j0 & 0x03) << 4) | (v13 & 0x0f);
        message[6] = (v20 & 0x7f);
        message[7] = ((v36 & 0x07) << 4) | (v27 & 0x07);
        message[8] = (v43 & 0x7f);
        message[9] = ((j1 & 0x03) << 4) | (v50 & 0x0f);
        message[10] = (v57 & 0x1f);
        message[11] = (v59 & 0x1f);
        message[12] = (v61 & 0x3f);
        message[13] = (v62 & 0x3f);
        return command;
    }

    /**
     * @brief Negative Voltage Gamma Control : The gamma correction is performed with 3 groups of registers, which are
     * gradient adjustment, contrast adjustment and fine- adjustment registers for positive and negative polarities, and
     * RGB can be adjusted individually.
     *
     * @param v0 [3:0]
     * @param v1 [5:0]
     * @param v2 [5:0]
     * @param v4 [4:0]
     * @param v6 [4:0]
     * @param v13 [3:0]
     * @param v20 [6:0]
     * @param v27 [2:0]
     * @param v36 [2:0]
     * @param v43 [6:0]
     * @param v50 [3:0]
     * @param v57 [4:0]
     * @param v59 [4:0]
     * @param v61 [5:0]
     * @param v62 [5:0]
     * @param v63 [3:0]
     * @param j0 [1:0]
     * @param j1 [1:0]
     * @return St7789Command* the command.
     */
    St7789Command *nvgamctrl(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v4, uint8_t v6, uint8_t v13, uint8_t v20,
                             uint8_t v27, uint8_t v36, uint8_t v43, uint8_t v50, uint8_t v57, uint8_t v59, uint8_t v61,
                             uint8_t v62, uint8_t v63, uint8_t j0, uint8_t j1) {
        St7789Command *command = newCommand(NVGAMCTRL, 14, WRITE_7789, false);
        uint8_t *message = getBufferUsingManagedBufferIfNeeded(command);
        message[0] = ((v63 & 0x0f) << 4) | (v0 & 0x0f);
        message[1] = (v1 & 0x3f);
        message[2] = (v2 & 0x3f);
        message[3] = (v4 & 0x1f);
        message[4] = (v6 & 0x1f);
        message[5] = ((j0 & 0x03) << 4) | (v13 & 0x0f);
        message[6] = (v20 & 0x7f);
        message[7] = ((v36 & 0x07) << 4) | (v27 & 0x07);
        message[8] = (v43 & 0x7f);
        message[9] = ((j1 & 0x03) << 4) | (v50 & 0x0f);
        message[10] = (v57 & 0x1f);
        message[11] = (v59 & 0x1f);
        message[12] = (v61 & 0x3f);
        message[13] = (v62 & 0x3f);
        return command;
    }
};

#endif