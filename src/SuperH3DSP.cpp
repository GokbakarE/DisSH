/*
 * This code is licensed under the GNU AGPLv3
 * Copyright (c) 2025 GokbakarE
 * Date: 28-08-2025
 */
#include "SuperH3DSP.hpp"
#include <iostream>
#include <bitset>
#include <unordered_map>
#include <string>
#include <string_view>
#include <format>

static const std::unordered_map<std::string_view, std::string_view> OpcodeMap = {
    {"0011nnnnmmmm1100", "ADD $M, $N"},
    {"0011nnnnmmmm1110", "ADDC $M, $N"},
    {"0011nnnnmmmm1111", "ADDV $M, $N"},
    {"0010nnnnmmmm1001", "AND $M, $N"},
    {"11001001iiiiiiii", "AND #$I, R0"},
    {"11001101iiiiiiii", "AND.B #$I, @(R0, GBR)"},
    {"10001011dddddddd", "BF $D"},
    {"10001111dddddddd", "BF/S $D"},
    {"1010ffffffffffff", "BRA $F"},
    {"0000nnnn00100011", "BRAF $N"},
    {"1011ffffffffffff", "BSR $F"},
    {"0000nnnn00000011", "BSRF $N"},
    {"10001001dddddddd", "BT $D"},
    {"10001101dddddddd", "BT/S $D"},
    {"0000000000101000", "CLRMAC"},
    {"0000000001001000", "CLRS"},
    {"0000000000001000", "CLRT"},
    {"0011nnnnmmmm0000", "CMP/EQ $M, $N"},
    {"0011nnnnmmmm0011", "CMP/GE $M, $N"},
    {"0011nnnnmmmm0111", "CMP/GT $M, $N"},
    {"0011nnnnmmmm0110", "CMP/HI $M, $N"},
    {"0011nnnnmmmm0010", "CMP/HS $M, $N"},
    {"0100nnnn00010101", "CMP/PL $N"},
    {"0100nnnn00010001", "CMP/PZ $N"},
    {"0010nnnnmmmm1100", "CMP/STR $M, $N"},
    {"10001000iiiiiiii", "CMP/EQ #$I, R0"},
    {"0010nnnnmmmm0111", "DIV0S $M, $N"},
    {"0000000000011001", "DIV0U"},
    {"0011nnnnmmmm0100", "DIV1 $M, $N"},
    {"0011nnnnmmmm1101", "DMULS.L $M, $N"},
    {"0011nnnnmmmm0101", "DMULU.L $M, $N"},
    {"0100nnnn00010000", "DT $N"},
    {"0110nnnnmmmm1110", "EXTS.B $M, $N"},
    {"0110nnnnmmmm1111", "EXTS.W $M, $N"},
    {"0110nnnnmmmm1100", "EXTU.B $M, $N"},
    {"0110nnnnmmmm1101", "EXTU.W $M, $N"},
    {"0100nnnn00101011", "JMP @$N"},
    {"0100nnnn00001011", "JSR @$N"},
    {"0100nnnn00001110", "LDC $N, SR"},
    {"0100nnnn00011110", "LDC $N, GBR"},
    {"0100nnnn00101110", "LDC $N, VBR"},
    {"0100nnnn00111110", "LDC $N, SSR"},
    {"0100nnnn01001110", "LDC $N, SPC"},
    {"0100nnnn01011110", "LDC $N, MOD"},//SH3-DSP
    {"0100nnnn01111110", "LDC $N, RE"},//SH3-DSP
    {"0100nnnn01101110", "LDC $N, RS"},//SH3-DSP
    {"0100nnnn10001110", "LDC $N, R0_BANK"},
    {"0100nnnn10011110", "LDC $N, R1_BANK"},
    {"0100nnnn10101110", "LDC $N, R2_BANK"},
    {"0100nnnn10111110", "LDC $N, R3_BANK"},
    {"0100nnnn11001110", "LDC $N, R4_BANK"},
    {"0100nnnn11011110", "LDC $N, R5_BANK"},
    {"0100nnnn11101110", "LDC $N, R6_BANK"},
    {"0100nnnn11111110", "LDC $N, R7_BANK"},
    {"0100nnnn00000111", "LDC.L @$N+, SR"},
    {"0100nnnn00010111", "LDC.L @$N+, GBR"},
    {"0100nnnn00100111", "LDC.L @$N+, VBR"},
    {"0100nnnn00110111", "LDC.L @$N+, SSR"},
    {"0100nnnn01000111", "LDC.L @$N+, SPC"},
    {"0100nnnn01010111", "LDC.L @$N+, MOD"},//SH3-DSP
    {"0100nnnn01110111", "LDC.L @$N+, RE"},//SH3-DSP
    {"0100nnnn01100111", "LDC.L @$N+, RS"},//SH3-DSP
    {"0100nnnn10000111", "LDC.L @$N+, R0_BANK"},
    {"0100nnnn10010111", "LDC.L @$N+, R1_BANK"},
    {"0100nnnn10100111", "LDC.L @$N+, R2_BANK"},
    {"0100nnnn10110111", "LDC.L @$N+, R3_BANK"},
    {"0100nnnn11000111", "LDC.L @$N+, R4_BANK"},
    {"0100nnnn11010111", "LDC.L @$N+, R5_BANK"},
    {"0100nnnn11100111", "LDC.L @$N+, R6_BANK"},
    {"0100nnnn11110111", "LDC.L @$N+, R7_BANK"},
    {"0100nnnn00001010", "LDS $N, MACH"},
    {"0100nnnn00011010", "LDS $N, MACL"},
    {"0100nnnn00101010", "LDS $N, PR"},
    {"0100nnnn01101010", "LDS $N, DSR"},//SH3-DSP only
    {"0100nnnn01111010", "LDS $N, A0"},//SH3-DSP only
    {"0100nnnn10001010", "LDS $N, X0"},//SH3-DSP only//SH3-DSP only
    {"0100nnnn10011010", "LDS $N, X1"},//SH3-DSP only
    {"0100nnnn10101010", "LDS $N, Y0"},//SH3-DSP only
    {"0100nnnn10111010", "LDS $N, Y1"},//SH3-DSP only
    {"0100nnnn00000110", "LDS.L @$N+, MACH"},
    {"0100nnnn00010110", "LDS.L @$N+, MACL"},
    {"0100nnnn00100110", "LDS.L @$N+, PR"},
    {"0100nnnn01100110", "LDS.L @$N+, DSR"},//SH3-DSP only
    {"0100nnnn01110110", "LDS.L @$N+, A0"},//SH3-DSP only
    {"0100nnnn10000110", "LDS.L @$N+, X0"},//SH3-DSP only
    {"0100nnnn10010110", "LDS.L @$N+, X1"},//SH3-DSP only
    {"0100nnnn10100110", "LDS.L @$N+, Y0"},//SH3-DSP only
    {"0100nnnn10110110", "LDS.L @$N+, Y1"},//SH3-DSP only
    {"10001110pppppppp", "LDRE @($P, PC)"},//SH3-DSP only
    {"10001100pppppppp", "LDRS @($P, PC)"},//SH3-DSP only
    {"0000000000111000", "LDTLB"},
    {"0000nnnnmmmm1111", "MAC.L $M, $N"},
    {"0100nnnnmmmm1111", "MAC.W $M, $N"},
    {"0110nnnnmmmm0011", "MOV $M, $N"},
    {"0010nnnnmmmm0000", "MOV.B $M, @$N"},
    {"0010nnnnmmmm0001", "MOV.W $M, @$N"},
    {"0010nnnnmmmm0010", "MOV.L $M, @$N"},
    {"0110nnnnmmmm0000", "MOV.B @$M, $N"},
    {"0110nnnnmmmm0001", "MOV.W @$M, $N"},
    {"0110nnnnmmmm0010", "MOV.L @$M, $N"},
    {"0010nnnnmmmm0100", "MOV.B $M, @-$N"},
    {"0010nnnnmmmm0101", "MOV.W $M, @-$N"},
    {"0010nnnnmmmm0110", "MOV.L $M, @-$N"},
    {"0110nnnnmmmm0100", "MOV.B @$M+, $N"},
    {"0110nnnnmmmm0101", "MOV.W @$M+, $N"},
    {"0110nnnnmmmm0110", "MOV.L @$M+, $N"},
    {"0000nnnnmmmm0100", "MOV.B $M, @(R0, $N)"},
    {"0000nnnnmmmm0101", "MOV.W $M, @(R0, $N)"},
    {"0000nnnnmmmm0110", "MOV.L $M, @(R0, $N)"},
    {"0000nnnnmmmm1100", "MOV.B @(R0, $M), $N"},
    {"0000nnnnmmmm1101", "MOV.W @(R0, $M), $N"},
    {"0000nnnnmmmm1110", "MOV.L @(R0, $M), $N"},
    {"1110nnnniiiiiiii", "MOV #$I, $N"},
    {"1001nnnnpppppppp", "MOV.W @($P, PC), $N"},
    {"1101nnnnpppppppp", "MOV.L @($P, PC), $N"},
    {"11000100pppppppp", "MOV.B @($P, GBR), R0"},
    {"11000101pppppppp", "MOV.W @($P, GBR), R0"},
    {"11000110pppppppp", "MOV.L @($P, GBR), R0"},
    {"11000000pppppppp", "MOV.B R0, @($P, GBR)"},
    {"11000001pppppppp", "MOV.W R0, @($P, GBR)"},
    {"11000010pppppppp", "MOV.L R0, @($P, GBR)"},
    {"10000000mmmmssss", "MOV.B R0, @($S, $M)"},
    {"10000001mmmmssss", "MOV.W R0, @($S, $M)"},
    {"0001nnnnmmmmssss", "MOV.L $M, @($S, $N)"},
    {"10000100mmmmssss", "MOV.B @($S, $M), R0"},
    {"10000101mmmmssss", "MOV.W @($S, $M), R0"},
    {"0101nnnnmmmmssss", "MOV.L @($S, $N), $M"},
    {"11000111pppppppp", "MOVA @($P, PC), R0"},
    {"0000nnnn00101001", "MOVT $N"},
    {"0000nnnnmmmm0111", "MUL.L $M, $N"},
    {"0010nnnnmmmm1111", "MULS.W $M, $N"},
    {"0010nnnnmmmm1110", "MULU.W $M, $N"},
    {"0110nnnnmmmm1011", "NEG $M, $N"},
    {"0110nnnnmmmm1010", "NEGC $M, $N"},
    {"0000000000001001", "NOP"},
    {"0110nnnnmmmm0111", "NOT $M, $N"},
    {"0010nnnnmmmm1011", "OR $M, $N"},
    {"11001011iiiiiiii", "OR #$I, R0"},
    {"11001111iiiiiiii", "OR.B #$I, @(R0, GBR)"},
    {"0000nnnn10000011", "PREF @$N"},
    {"0100nnnn00100100", "ROTCL $N"},
    {"0100nnnn00100101", "ROTCR $N"},
    {"0100nnnn00000100", "ROTL $N"},
    {"0100nnnn00000101", "ROTR $N"},
    {"0000000000101011", "RTE"},
    {"0000000000001011", "RTS"},
    {"0100nnnn00010100", "SETRC $N"},
    {"10000010iiiiiiii", "SETRC #$I"},
    {"0000000001011000", "SETS"},
    {"0000000000011000", "SETT"},
    {"0100nnnnmmmm1100", "SHAD $M, $N"},
    {"0100nnnn00100000", "SHAL $N"},
    {"0100nnnn00100001", "SHAR $N"},
    {"0100nnnnmmmm1101", "SHLD $M, $N"},
    {"0100nnnn00000000", "SHLL $N"},
    {"0100nnnn00001000", "SHLL2 $N"},
    {"0100nnnn00011000", "SHLL8 $N"},
    {"0100nnnn00101000", "SHLL16 $N"},
    {"0100nnnn00000001", "SHRL $N"},
    {"0100nnnn00001001", "SHRL2 $N"},
    {"0100nnnn00011001", "SHRL8 $N"},
    {"0100nnnn00101001", "SHRL16 $N"},
    {"0000000000011011", "SLEEP"},
    {"0000nnnn00000010", "STC SR, $N"},
    {"0000nnnn00010010", "STC GBR, $N"},
    {"0000nnnn00100010", "STC VBR, $N"},
    {"0000nnnn00110010", "STC SSR, $N"},
    {"0000nnnn01000010", "STC SPC, $N"},
    {"0000nnnn01010010", "STC MOD, $N"},//DSP
    {"0000nnnn01110010", "STC RE, $N"},//DSP
    {"0000nnnn01100010", "STC RS, $N"},//DSP
    {"0000nnnn10000010", "STC R0_BANK, $N"},
    {"0000nnnn10010010", "STC R1_BANK, $N"},
    {"0000nnnn10100010", "STC R2_BANK, $N"},
    {"0000nnnn10110010", "STC R3_BANK, $N"},
    {"0000nnnn11000010", "STC R4_BANK, $N"},
    {"0000nnnn11010010", "STC R5_BANK, $N"},
    {"0000nnnn11100010", "STC R6_BANK, $N"},
    {"0000nnnn11110010", "STC R7_BANK, $N"},
    {"0100nnnn00000011", "STC.L SR, @-$N"},
    {"0100nnnn00010011", "STC.L GBR, @-$N"},
    {"0100nnnn00100011", "STC.L VBR, @-$N"},
    {"0100nnnn00110011", "STC.L SSR, @-$N"},
    {"0100nnnn01000011", "STC.L SPC, @-$N"},
    {"0100nnnn01010011", "STC.L MOD, @-$N"},//DSP
    {"0100nnnn01110011", "STC.L RE, @-$N"},//DSP
    {"0100nnnn01100011", "STC.L RS, @-$N"},//DSP
    {"0100nnnn10000011", "STC.L R0_BANK, @-$N"},
    {"0100nnnn10010011", "STC.L R1_BANK, @-$N"},
    {"0100nnnn10100011", "STC.L R2_BANK, @-$N"},
    {"0100nnnn10110011", "STC.L R3_BANK, @-$N"},
    {"0100nnnn11000011", "STC.L R4_BANK, @-$N"},
    {"0100nnnn11010011", "STC.L R5_BANK, @-$N"},
    {"0100nnnn11100011", "STC.L R6_BANK, @-$N"},
    {"0100nnnn11110011", "STC.L R7_BANK, @-$N"},
    {"0000nnnn00001010", "STS MACH, $N"},
    {"0000nnnn00011010", "STS MACL, $N"},
    {"0000nnnn00101010", "STS PR, $N"},
    {"0000nnnn01101010", "STS DSR, $N"},//DSP
    {"0000nnnn01111010", "STS A0, $N"},//DSP
    {"0000nnnn10001010", "STS X0, $N"},//DSP
    {"0000nnnn10011010", "STS X1, $N"},//DSP
    {"0000nnnn10101010", "STS Y0, $N"},//DSP
    {"0000nnnn10111010", "STS Y1, $N"},//DSP
    {"0100nnnn00000010", "STS.L MACH, @-$N"},
    {"0100nnnn00010010", "STS.L MACL, @-$N"},
    {"0100nnnn00100010", "STS.L PR, @-$N"},
    {"0100nnnn01100010", "STS.L DSR, @-$N"},//DSP
    {"0100nnnn01100010", "STS.L A0, @-$N"},//DSP
    {"0100nnnn10000010", "STS.L X0, @-$N"},//DSP
    {"0100nnnn10010010", "STS.L X1, @-$N"},//DSP
    {"0100nnnn10100010", "STS.L Y0, @-$N"},//DSP
    {"0100nnnn10110010", "STS.L Y1, @-$N"},//DSP
    {"0011nnnnmmmm1000", "SUB $M, $N"},
    {"0011nnnnmmmm1010", "SUBC $M, $N"},
    {"0011nnnnmmmm1011", "SUBV $M, $N"},
    {"0110nnnnmmmm1000", "SWAP.B $M, $N"},
    {"0110nnnnmmmm1001", "SWAP.W $M, $N"},
    {"0100nnnn00011011", "TAS.B @$N"},
    {"11000011iiiiiiii", "TRAPA #$I"},
    {"0010nnnnmmmm1000", "TST $M, $N"},
    {"11001000iiiiiiii", "TST #$I, R0"},
    {"11001100iiiiiiii", "TST.B #$I, @(R0, GBR)"},
    {"0010nnnnmmmm1010", "XOR $M, $N"},
    {"11001010iiiiiiii", "XOR #$I, R0"},
    {"11001110iiiiiiii", "XOR.B #$I, @(R0, GBR)"},
    {"0010nnnnmmmm1101", "XTRCT $M, $N"},
    {"11110122qqqq0010", "MOVS.L @-$2, $Q"},
    {"11110122qqqq0110", "MOVS.L @2, $Q"},
    {"11110122qqqq1010", "MOVS.L @$2+, $Q"},
    {"11110122qqqq1110", "MOVS.L @$2+ Ix, $Q"},
    {"11110122qqqq0011", "MOVS.L $Q, @-$2"},
    {"11110122qqqq0111", "MOVS.L $Q, @$2"},
    {"11110122qqqq1011", "MOVS.L $Q, @$2+"},
    {"11110122qqqq1111", "MOVS.L $Q, @$2+ Ix"},
    {"11110122qqqq0000", "MOVS.W @-$2, $Q"},
    {"11110122qqqq0100", "MOVS.W @$2, $Q"},
    {"11110122qqqq1000", "MOVS.W @$2+, $Q"},
    {"11110122qqqq1100", "MOVS.W @$2+ Ix, $Q"},
    {"11110122qqqq0001", "MOVS.W $Q, @-$2"},
    {"11110122qqqq0101", "MOVS.W $Q, @$2"},
    {"11110122qqqq1001", "MOVS.W $Q, @$2+"},
    {"11110122qqqq1101", "MOVS.W $Q, @$2+ Ix"},
//     {"1111000*0*0*00**", "NOPx"},
//     {"111100*0*0*0**00", "NOPy"},
//     {"1111003*4*0*01**", "MOVX.W @$3, $4"},
//     {"1111003*4*0*10**", "MOVX.W @$3+, $4"},
//     {"1111003*4*0*11**", "MOVX.W @$3+ Ix, $4"},
//     {"1111003*5*1*01**", "MOVX.W $5, @$3"},
//     {"1111003*5*1*10**", "MOVX.W $5, @$3+"},
//     {"1111003*5*1*11**", "MOVX.W $5, @$3+ Ix"},
//     {"111100*6*7*0**01", "MOVY.W @$6, $7"},
//     {"111100*6*7*0**10", "MOVY.W @$6+, $7"},
//     {"111100*6*7*0**11", "MOVY.W @$6+ Iy, $7"},
//     {"111100*6*8*1**01", "MOVY.W $8, @$6"},
//     {"111100*6*8*1**10", "MOVY.W $8, @$6+"},
//     {"111100*6*8*1**11", "MOVY.W $8, @$6+ Iy"},
    {"0111nnnniiiiiiii", "ADD #$I, $N"},

};

static const std::unordered_map<std::string_view, std::string_view> registerMap = {
    {"0000", "R0"}, {"0001", "R1"}, {"0010", "R2"}, {"0011", "R3"},
    {"0100", "R4"}, {"0101", "R5"}, {"0110", "R6"}, {"0111", "R7"},
    {"1000", "R8"}, {"1001", "R9"}, {"1010", "R10"}, {"1011", "R11"},
    {"1100", "R12"}, {"1101", "R13"}, {"1110", "R14"}, {"1111", "R15"}
};

static std::unordered_map<std::string, std::string> AAmap = {
    {"00", "R4"}, 
    {"01", "R5"},
    {"10", "R2"},
    {"11", "R3"}
};
static std::unordered_map<std::string, std::string> AXmap = {
    {"0", "R4"}, 
    {"1", "R5"}
};
static std::unordered_map<std::string, std::string> AYmap = {
    {"0", "R6"}, 
    {"1", "R7"}
};
static std::unordered_map<std::string, std::string> DxXmap = {
    {"0", "X0"}, 
    {"1", "X1"}
};
static std::unordered_map<std::string, std::string> DaXmap = {
    {"0", "A0"}, 
    {"1", "A1"}
};
static std::unordered_map<std::string, std::string> DyYmap = {
    {"0", "Y0"}, 
    {"1", "Y1"}
};
static std::unordered_map<std::string, std::string> DaYmap = {
    {"0", "A0"}, 
    {"1", "A1"}
};
static std::unordered_map<std::string, std::string> DDDDmap = {
    {"0101", "A1"}, 
    {"1110", "M1"}, 
    {"1101", "A1G"},
    {"1100", "M0"},
    {"1011", "Y1"},
    {"1010", "Y0"},
    {"1001", "X1"},
    {"1000", "X0"},
    {"0111", "A0"},
    {"1111", "A0G"}
};


std::string SuperH3DSP(const std::string_view binaryCode) {
    for (const auto& [pattern, assembly] : OpcodeMap) {
        bool match = true;
        // First verify the fixed bits match
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i] == '*' || std::isalpha(pattern[i])) continue;
            if (pattern[i] != binaryCode[i]) {
                match = false;
                break;
            }
        }
        if (!match) continue;

        auto substitute = [&](size_t pos, size_t len) -> std::string {
            std::string bits = std::string(binaryCode.substr(pos, len));
            return std::to_string(std::bitset<16>(bits).to_ulong());
        };

        std::string result(assembly);
        size_t pos;
        while ((pos = result.find("$N")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(4, 4));
            result.replace(pos, 2, registerMap.at(regBits));
        }
        while ((pos = result.find("$M")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(8, 4));
            result.replace(pos, 2, registerMap.at(regBits));
        }
        
        while ((pos = result.find("$Q")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(8, 4));
            result.replace(pos, 2, DDDDmap.at(regBits));
        }
        while ((pos = result.find("$2")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(6, 2));
            result.replace(pos, 2, AAmap.at(regBits));
        }
        while ((pos = result.find("$3")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(6, 1));
            result.replace(pos, 2, AXmap.at(regBits));
        }
        while ((pos = result.find("$4")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(8, 1));
            result.replace(pos, 2, DxXmap.at(regBits));
        }
        while ((pos = result.find("$5")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(8, 1));
            result.replace(pos, 2, DaXmap.at(regBits));
        }

        while ((pos = result.find("$6")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(7, 1));
            result.replace(pos, 2, AYmap.at(regBits));
        }
        while ((pos = result.find("$7")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(9, 1));
            result.replace(pos, 2, DyYmap.at(regBits));
        }
        while ((pos = result.find("$8")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(9, 1));
            result.replace(pos, 2, DaYmap.at(regBits));
        }
        while ((pos = result.find("$I")) != std::string::npos) {
            std::string bits = std::string(binaryCode.substr(8, 8));
            long val = std::bitset<8>(bits).to_ulong();
            result.replace(pos, 2, std::to_string(val));
        }
        while ((pos = result.find("$J")) != std::string::npos) {
            std::string bits = std::string(binaryCode.substr(5, 7));
            long val = std::bitset<7>(bits).to_ulong();
            result.replace(pos, 2, std::to_string(val));
        }
        while ((pos = result.find("$P")) != std::string::npos) {
            std::string bits = std::string(binaryCode.substr(8, 8));
            unsigned long val = std::bitset<8>(bits).to_ulong();
            result.replace(pos, 2, std::to_string(val));
        }
        while ((pos = result.find("$S")) != std::string::npos) {
            std::string bits = std::string(binaryCode.substr(12, 4));
            unsigned long val = std::bitset<4>(bits).to_ulong();
            result.replace(pos, 2, std::to_string(val));
        }
        while ((pos = result.find("$D")) != std::string::npos) {
            std::string bits = std::string(binaryCode.substr(8, 8));
            unsigned long val = std::bitset<8>(bits).to_ulong();
            result.replace(pos, 2, std::format("{:X}", val));
        }
        while ((pos = result.find("$F")) != std::string::npos) {
            std::string bits = std::string(binaryCode.substr(4, 12));
            unsigned long val = std::bitset<12>(bits).to_ulong();
            result.replace(pos, 2, std::format("{:X}", val));
        }

        return result;
    }
    return std::string("word") + std::string(binaryCode);
}
