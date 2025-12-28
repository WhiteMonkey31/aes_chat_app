#include "s_box.hpp"
#include "logger.hpp"
#include <cstring>

void rotWord(uint8_t* w) {
    uint8_t tmp = w[0];
    w[0] = w[1];
    w[1] = w[2];
    w[2] = w[3];
    w[3] = tmp;
}

void subWord(uint8_t* w) {
    for (int i = 0; i < 4; i++) {
        w[i] = sbox[w[i]];
    }
}

void keyExpansion(const uint8_t* key, uint8_t* roundKeys) {
    // First round key = original key
    for (int i = 0; i < 16; i++) {
        roundKeys[i] = key[i];
    }

    int bytesGenerated = 16;
    int rconIteration = 1;
    uint8_t temp[4];

    while (bytesGenerated < 176) {
        // Read last 4 bytes
        for (int i = 0; i < 4; i++) {
            temp[i] = roundKeys[bytesGenerated - 4 + i];
        }

        if (bytesGenerated % 16 == 0) {
            rotWord(temp);
            subWord(temp);
            temp[0] ^= Rcon[rconIteration++];
        }

        for (int i = 0; i < 4; i++) {
            roundKeys[bytesGenerated] =
                roundKeys[bytesGenerated - 16] ^ temp[i];
            bytesGenerated++;
        }
    }
}

void loadState(uint8_t state[4][4], const uint8_t* block) {
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            state[row][col] = block[col * 4 + row];
        }
    }
}

void storeState(uint8_t* block, uint8_t state[4][4]) {
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            block[col * 4 + row] = state[row][col];
        }
    }
}

void addRoundKey(uint8_t state[4][4], const uint8_t* roundKey) {
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            state[row][col] ^= roundKey[col * 4 + row];
        }
    }
}

void subBytes(uint8_t state[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[i][j] = sbox[state[i][j]];
        }
    }
}

void shiftRows(uint8_t state[4][4]) {
    uint8_t temp;

    // Row 1 shift left by 1
    temp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = temp;

    // Row 2 shift left by 2
    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;
    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;

    // Row 3 shift left by 3
    temp = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = state[3][0];
    state[3][0] = temp;
}

void mixColumns(uint8_t state[4][4]) {
    uint8_t temp[4];

    for (int col = 0; col < 4; col++) {
        temp[0] = gmul(0x02, state[0][col]) ^
                  gmul(0x03, state[1][col]) ^
                  state[2][col] ^
                  state[3][col];

        temp[1] = state[0][col] ^
                  gmul(0x02, state[1][col]) ^
                  gmul(0x03, state[2][col]) ^
                  state[3][col];

        temp[2] = state[0][col] ^
                  state[1][col] ^
                  gmul(0x02, state[2][col]) ^
                  gmul(0x03, state[3][col]);

        temp[3] = gmul(0x03, state[0][col]) ^
                  state[1][col] ^
                  state[2][col] ^
                  gmul(0x02, state[3][col]);

        for (int row = 0; row < 4; row++) {
            state[row][col] = temp[row];
        }
    }
}

void aesEncryptBlock(uint8_t* block, const uint8_t* roundKeys) {
    uint8_t state[4][4];

    loadState(state, block);

    // Round 0
    addRoundKey(state, roundKeys);
    if (g_logger) g_logger->logState("State after AddRoundKey (Round 0):", state);

    // Rounds 1–9
    for (int round = 1; round <= 9; round++) {
        subBytes(state);
        if (g_logger) g_logger->logState("After SubBytes:", state);
        shiftRows(state);
        if (g_logger) g_logger->logState("After ShiftRows:", state);
        mixColumns(state);
        if (g_logger) g_logger->logState("After MixColumns:", state);
        addRoundKey(state, roundKeys + round * 16);
        if (g_logger) {
            g_logger->logRoundKey(round, roundKeys + round * 16);
            g_logger->logState(std::string("State after AddRoundKey (Round ") + std::to_string(round) + "):", state);
        }
    }

    // Final round (NO MixColumns)
    subBytes(state);
    if (g_logger) g_logger->logState("Final Round - After SubBytes:", state);
    shiftRows(state);
    if (g_logger) g_logger->logState("Final Round - After ShiftRows:", state);
    addRoundKey(state, roundKeys + 160);
    if (g_logger) {
        g_logger->logRoundKey(10, roundKeys + 160);
        g_logger->logState("State after AddRoundKey (Round 10):", state);
    }

    storeState(block, state);
}
