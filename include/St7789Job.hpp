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
#ifndef ST7789JOB_HPP
#define ST7789JOB_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes
#include "St7789Types.hpp"

/**
 * @brief Essentially a collection of pointers that will have to be freed later.
 *
 * @tparam SpiTransaction the native type of transaction
 */
template <typename SpiTransaction> class St7789Job {
    private:
    St7789Command *command;
    SpiTransaction *transactionOfCommand;
    SpiTransaction *transactionOfData;

    public:
    St7789Job(St7789Command *cmd, SpiTransaction *tcmd, SpiTransaction *tdat)
        : command(cmd), transactionOfCommand(tcmd), transactionOfData(tdat) {}

    virtual ~St7789Job() {
        if (command) {
            if (MUST_USE_EXTERNAL_BUFFER(command->dataLength) && !(command->keepExternalBufferOnDelete)) {
                delete command->externalBuffer;
            }
            delete command;
        }
        if (transactionOfCommand) {
            delete transactionOfCommand;
        }
        if (transactionOfData) {
            delete transactionOfData;
        }
    }

    SpiTransaction *getTransactionOfCommand() { return transactionOfCommand; }
    SpiTransaction *getTransactionOfData() { return transactionOfData; }
};

#endif