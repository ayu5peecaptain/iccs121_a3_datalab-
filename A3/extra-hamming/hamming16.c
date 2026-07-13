#include "hamming16.h"
#include <stdio.h>
#include <stdlib.h>
/*
* Data structure
*
* +--------+--------+--------+--------+
* | ( 0)EP | ( 1)P  | ( 2)P  | ( 3)D  |  <-- row 0
* +--------+--------+--------+--------+
* | ( 4)P  | ( 5)D  | ( 6)D  | ( 7)D  |  <-- row 1
* +--------+--------+--------+--------+
* | ( 8)P  | ( 9)D  | (10)D  | (11)D  |  <-- row 2
* +--------+--------+--------+--------+
* | (12)D  | (13)D  | (14)D  | (15)D  |  <-- row 3
* +--------+--------+--------+--------+
*          ↑        ↑        ↑        ↑
*        col 0    col 1    col 2    col 3
*/

/*
 * extendedHammingEncode:
 *  - data: lower 11 bits represent the 11-bit data.
 *  - Returns a 16-bit code word with:
 *      - Extended Parity (EP) at bit 0.
 *      - Hamming parity bits at positions 1, 2, 4, 8.
 *      - Data bits at positions 3, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15.
 */
uint16_t extendedHammingEncode(uint16_t data) {
    uint16_t code = 0;

    // TODO: Insert the 11 data bits into the proper positions of the 15-bit Hamming code.
    // For example, assign bit0 of data to position 3, bit1 to position 5, etc.
    // Each line reads one bit of "data" and writes it into the
    // matching data-position of "code", per the placement list
    // above (data bit 0 -> position 3 -> inde 2, and so on)

    if (data & (1 << 0)){ //data bit 0 -> position 3
        code |= (1 << 2);
    }

    if (data & (1 << 1)){   // data bit 1 -> position 5
        code |= (1 << 4);
    }
    if (data & (1 << 2)){   // data bit 2 -> position 6
        code |= (1 << 5);
    }
    if (data & (1 << 3)){   // data bit 3 -> position 7
        code |= (1 << 6);
    }
    if (data & (1 << 4)){   // data bit 4 -> position 9
        code |= (1 << 8);   
    }
    if (data & (1 << 5)){   // data bit 5 -> position 10
        code |= (1 <<9);
    }
    if (data & (1 << 6)){   // data bit 6 -> position 11
        code |= (1 << 10);
    }
    if (data & (1 << 7)){   // data bit 7 -> position 12
        code |= (1 << 11);
    }
    if (data & (1 << 8)){   // data bit 8 -> position 13
        code |= (1 << 12);
    }
    if (data & (1 << 9)){   // data bit 9 -> position 14
        code |= (1 << 13);
    }
    if (data & (1 << 10)){// data bit 1 -> position 15
        code |= (1 << 14);
    }

    // TODO: Compute the parity bits for positions 1, 2, 4, and 8.
    // For each parity bit, XOR all bits in the code word covered by that parity position.

    int p1 = ((code >> 2) & 1) ^ ((code >> 4) & 1) ^ ((code >> 6) & 1) ^    // position 3,5,
         ((code >> 8) & 1) ^ ((code >> 10) & 1) ^ ((code >> 12) & 1) ^      // 9, 11, 13,
         ((code >> 14) & 1);                                                // 15

    int p2 = ((code >> 2) & 1) ^ ((code >> 5) & 1) ^ ((code >> 6) & 1) ^    // position 3,6,
         ((code >> 9) & 1) ^ ((code >> 10) & 1) ^ ((code >> 13) & 1) ^      // position 10, 11,14,
         ((code >> 14) & 1);                                                // position 15

    int p4 = ((code >> 4) & 1) ^ ((code >> 5) & 1) ^ ((code >> 6) & 1) ^    // position 5,6
         ((code >> 11) & 1) ^ ((code >> 12) & 1) ^ ((code >> 13) & 1) ^     // 12, 13, 14,
         ((code >> 14) & 1);                                                // 15

    int p8 = ((code >> 8) & 1) ^ ((code >> 9) & 1) ^ ((code >> 10) & 1) ^   // position 9,10
         ((code >> 11) & 1) ^ ((code >> 12) & 1) ^ ((code >> 13) & 1) ^     // position 12, 13,14
         ((code >> 14) & 1);                                                // 15

    // TODO: Insert the computed parity bits into the code word.
    if (p1){
        code |= (1 << 0); // position 1
    } 
    if (p2){
        code |= (1 << 1); // position 2
    } 
    if (p4){
        code |= (1 << 3); // position 4
    } 
    if (p8){
        code |= (1 << 7); // position 8
    }
    // TODO: Compute the overall parity bit (bit 0) as the XOR of bits 1 through 15.
    int overallParity = 0;
    for (int i = 0; i <15; i++){
        overallParity ^=(code >> i) & 1;
    }
    if (overallParity){
        code |= (1 << 15);  // position 16
    }
    // Then, set bit 0 accordingly.
    
    return code;
}

/*
 * extendedHammingDecode:
 *  - code: 16-bit Extended Hamming code word.
 *  - errorPos: pointer to an integer to store the bit position of a corrected error.
 *  - Returns the original 11-bit data (in the lower 11 bits).
 *
 * The function should:
 *   1. Recompute the parity bits to form a syndrome.
 *   2. Check the overall parity (bit 0) to confirm if an error occurred.
 *   3. If a single-bit error is detected, correct the error by flipping that bit.
 *   4. Extract and return the 11-bit data.
 */
uint16_t extendedHammingDecode(uint16_t code, int *errorPos) {
    uint16_t correctedCode = code;
    int syndrome = 0;

    // TODO: Recompute parity bits for positions 1, 2, 4, and 8.
    // Combine the parity check results to form a syndrome (a number from 0 to 15).

    // positions 1,3,5,7,9,11,13,15
    int c1 = ((code >> 0) & 1) ^ ((code >> 2) & 1) ^ ((code >> 4) & 1) ^
         ((code >> 6) & 1) ^ ((code >> 8) & 1) ^ ((code >> 10) & 1) ^
         ((code >> 12) & 1) ^ ((code >> 14) & 1);                   

    // positions 2,3,6,7,10,11,14,15   
    int c2 = ((code >> 1) & 1) ^ ((code >> 2) & 1) ^ ((code >> 5) & 1) ^
         ((code >> 6) & 1) ^ ((code >> 9) & 1) ^ ((code >> 10) & 1) ^
         ((code >> 13) & 1) ^ ((code >> 14) & 1);

    // positions 4,5,6,7,12,13,14,15
    int c4 = ((code >> 3) & 1) ^ ((code >> 4) & 1) ^ ((code >> 5) & 1) ^
         ((code >> 6) & 1) ^ ((code >> 11) & 1) ^ ((code >> 12) & 1) ^
         ((code >> 13) & 1) ^ ((code >> 14) & 1);

    // positions 8,9,10,11,12,13,14,15     
    int c8 = ((code >> 7) & 1) ^ ((code >> 8) & 1) ^ ((code >> 9) & 1) ^
         ((code >> 10) & 1) ^ ((code >> 11) & 1) ^ ((code >> 12) & 1) ^
         ((code >> 13) & 1) ^ ((code >> 14) & 1);

    // TODO: Check the overall parity bit (bit 0) to decide if an error occurred.
    // If syndrome != 0 and overall parity is incorrect, then a single-bit error occurred.
    syndrome = (c1 << 0) | (c2 << 1) | (c4 << 2) | (c8 << 3); 
    // Correct the error by flipping the bit at the position indicated by syndrome.
    // Set *errorPos to the corrected bit position (1-indexed).
    // If no error, set *errorPos to 0.
    // If error detection indicates an uncorrectable error, set *errorPos to -1.

    // TODO: Extract the original 11-bit data from the corrected code word.
    int overallParity = 0;
    for (int i = 0; i <15; i++){
        overallParity ^=(code >> i) & 1;
    }
    int receivedOverallParityBit = (code >> 15) & 1;
    int parityMismatch = (overallParity != receivedOverallParityBit);
    uint16_t data = 0;

    if (syndrome == 0 && !parityMismatch) {
    *errorPos = 0;

    } else if (syndrome == 0 && parityMismatch) {
        *errorPos = 16;
        correctedCode = code ^ (1 << 15);

    } else if (syndrome != 0 && parityMismatch) {   // ← this branch is missing
        *errorPos = syndrome;
        correctedCode = code ^ (1 << (syndrome - 1));

    } else {
        // syndrome != 0 && !parityMismatch
        *errorPos = -1;
    }
    
    if ((correctedCode >> 2) & 1){  //position 3 -> data bit 0
        data |= (1 << 0);
    }
    if ((correctedCode >> 4) & 1){ // position 5 -> data bit 1
        data |= (1 << 1);
    }
    if ((correctedCode >> 5) & 1){ // position 6 -> data bit 2
        data |= (1 << 2);
    }
    if ((correctedCode >> 6) & 1){ // position 7 -> data bit 3
        data |= (1 << 3);
    }
    if ((correctedCode >> 8) & 1){ // position 9 -> data bit 4
        data |= (1 << 4);
    }
    if ((correctedCode >> 9) & 1){ // position 10 -> data bit 5
        data |= (1 << 5);
    }
    if ((correctedCode >> 10) & 1){ // position 11 -> data bit 6
        data |= (1 << 6);
    }
    if ((correctedCode >> 11) & 1){ // position 12 -> data bit 7
        data |= (1 << 7);
    }
    if ((correctedCode >> 12) & 1){ // position 13 -> data bit 8
        data |= (1 << 8);
    }
    if ((correctedCode >> 13) & 1){ // position 14 -> data bit 9
        data |= (1 << 9);
    }
    if ((correctedCode >> 14) & 1){ // position 15 -> data bit 10
        data |= (1 << 10);
    }
    

    return data;
}

/*
 * introduceError:
 *  - code: a 16-bit code word.
 *  - bitPos: bit position (1-indexed, 1 to 16) to flip.
 *  - Returns the code word with the specified bit flipped.
 */
uint16_t introduceError(uint16_t code, int bitPos) {
    return code ^ (1 << (bitPos - 1));
}
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <hex_data> [error_bit_position]\n", argv[0]);
        return 1;
    }

    uint16_t data = (uint16_t)strtol(argv[1], NULL, 0);  // base 0 auto-detects "0x..." prefix
    uint16_t code = extendedHammingEncode(data);
    printf("Original data:  0x%03x\n", data);
    printf("Encoded code:   0x%04x\n", code);

    if (argc >= 3) {
        int bitPos = atoi(argv[2]);
        code = introduceError(code, bitPos);
        printf("After error at position %d: 0x%04x\n", bitPos, code);
    }

    int errorPos;
    uint16_t decoded = extendedHammingDecode(code, &errorPos);

    printf("Detected errorPos: %d\n", errorPos);
    printf("Decoded data:   0x%03x\n", decoded);
    printf("Matches original: %s\n", (decoded == data) ? "YES" : "NO");

    return 0;
}
    
