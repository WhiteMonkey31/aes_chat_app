#include "s_box.hpp"
#include "logger.hpp"
#include <cstring>

void invSubBytes(uint8_t state[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[i][j] = inv_sbox[state[i][j]];
        }
    }
}

void invShiftRows(uint8_t state[4][4]) {
    uint8_t temp;

    // Row 1 shift right by 1
    temp = state[1][3];
    state[1][3] = state[1][2];
    state[1][2] = state[1][1];
    state[1][1] = state[1][0];
    state[1][0] = temp;

    // Row 2 shift right by 2
    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;
    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;

    // Row 3 shift right by 3
    temp = state[3][0];
    state[3][0] = state[3][1];
    state[3][1] = state[3][2];
    state[3][2] = state[3][3];
    state[3][3] = temp;
}

void invMixColumns(uint8_t state[4][4]) {
    uint8_t temp[4];

    for (int col = 0; col < 4; col++) {
        temp[0] = gmul(0x0e, state[0][col]) ^
                  gmul(0x0b, state[1][col]) ^
                  gmul(0x0d, state[2][col]) ^
                  gmul(0x09, state[3][col]);

        temp[1] = gmul(0x09, state[0][col]) ^
                  gmul(0x0e, state[1][col]) ^
                  gmul(0x0b, state[2][col]) ^
                  gmul(0x0d, state[3][col]);

        temp[2] = gmul(0x0d, state[0][col]) ^
                  gmul(0x09, state[1][col]) ^
                  gmul(0x0e, state[2][col]) ^
                  gmul(0x0b, state[3][col]);

        temp[3] = gmul(0x0b, state[0][col]) ^
                  gmul(0x0d, state[1][col]) ^
                  gmul(0x09, state[2][col]) ^
                  gmul(0x0e, state[3][col]);

        for (int row = 0; row < 4; row++) {
            state[row][col] = temp[row];
        }
    }
}

void aesDecryptBlock(uint8_t* block, const uint8_t* roundKeys) {
    uint8_t state[4][4];

    loadState(state, block);

    // Round 10
    addRoundKey(state, roundKeys + 160);
    if (g_logger) {
        g_logger->logRoundKey(10, roundKeys + 160);
        g_logger->logState("State after AddRoundKey (Round 10):", state);
    }

    // Rounds 9–1
    for (int round = 9; round >= 1; round--) {
        invShiftRows(state);
        if (g_logger) g_logger->logState("After invShiftRows:", state);
        invSubBytes(state);
        if (g_logger) g_logger->logState("After invSubBytes:", state);
        addRoundKey(state, roundKeys + round * 16);
        if (g_logger) {
            g_logger->logRoundKey(round, roundKeys + round * 16);
            g_logger->logState(std::string("State after AddRoundKey (Round ") + std::to_string(round) + "):", state);
        }
        invMixColumns(state);
        if (g_logger) g_logger->logState("After invMixColumns:", state);
    }

    // Final round
    invShiftRows(state);
    if (g_logger) g_logger->logState("Final Round - After invShiftRows:", state);
    invSubBytes(state);
    if (g_logger) g_logger->logState("Final Round - After invSubBytes:", state);
    addRoundKey(state, roundKeys);
    if (g_logger) {
        g_logger->logRoundKey(0, roundKeys);
        g_logger->logState("State after AddRoundKey (Round 0):", state);
    }

    storeState(block, state);
}
