/*
 * This code is licensed under the GNU AGPLv3
 * Copyright (c) 2025 GokbakarE
 * Date: 28-08-2025
 */
#include "SuperH4.hpp"
#include <iostream>
#include <bitset>
#include <unordered_map>
#include <string>
#include <string_view>
#include <format>
static std::unordered_map<std::string, std::string> OpcodeMap = {
    {"0011nnnnmmmm1100", "ADD $M, $N"},
    {"0011nnnnmmmm1110", "ADDC $M, $N"},
    {"0011nnnnmmmm1111", "ADDV $M, $N"},
    {"0010nnnnmmmm1001", "AND $M, $N"},
    {"11001001iiiiiiii", "AND #$I, R0"},
    {"11001101iiiiiiii", "AND.B #$I, @(R0, GBR"},
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
    {"0010nnnnmmmm1100", "CMP/STR $M, $N"},
    {"0100nnnn00010101", "CMP/PL $N"},
    {"0100nnnn00010001", "CMP/PZ $N"},
    {"0100nnnn00010001", "CMP/PZ #$I, R0"},
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
    {"1111wwww01011101", "FABS $W"},
    {"1111qqq001011101", "FABS $Q"},
    {"1111wwwwxxxx0000", "FADD $X, $W"},
    {"1111qqq0zzz00000", "FADD $Z, $Q"},
    {"1111wwwwxxxx0100", "FCMP/EQ $X, $W"},
    {"1111qqq0zzz00100", "FCMP/EQ $Z, $Q"},
    {"1111wwwwxxxx0101", "FCMP/GT $X, $W"},
    {"1111qqq0zzz00101", "FCMP/GT $Z, $Q"},
    {"1111qqq010111101", "FCNVDS $Q, FPUL"},
    {"1111qqq010101101", "FCNVSD FPUL, $Q"},
    {"1111wwwwxxxx0011", "FDIV $X, $W"},
    {"1111qqq0zzz00011", "FDIV $Z, $Q"},
    {"1111ccvv11101101", "FIPR $V, $C"},
    {"1111wwww10001101", "FLDI0 $W"},
    {"1111wwww10011101", "FLDI1 $W"},
    {"1111wwww00011101", "FLDS $W, FPUL"},
    {"1111wwww00101101", "FLOAT FPUL, $W"},
    {"1111qqq000101101", "FLOAT FPUL, $Q"},
    {"1111wwwwxxxx1110", "FMAC FR0, $X, $W"},
    {"1111wwwwxxxx1100", "FMOV $X, $W"},
    {"1111qqq0zzz01100", "FMOV $Z, $Q"},
    {"1111nnnnxxxx1010", "FMOV.S $X, @$N"},
    {"1111nnnnzzz01010", "FMOV $Z, @$N"},
    {"1111wwwwmmmm1000", "FMOV.S @$M, $W"},
    {"1111qqq0mmmm1000", "FMOV @$M, $Q"},
    {"1111wwwwmmmm1001", "FMOV.S @$M+, $W"},
    {"1111qqq0mmmm1001", "FMOV @$M+, $Q"},
    {"1111nnnnxxxx1011", "FMOV.S $X, @-$N"},
    {"1111nnnnzzz01011", "FMOV $Z, @-$N"},
    {"1111wwwwmmmm0110", "FMOV.S @(R0, $M), $W"},
    {"1111qqq0mmmm0110", "FMOV @(R0, $M), $Q"},
    {"1111nnnnxxxx0111", "FMOV.S $X, @(R0, $N)"},
    {"1111nnnnzzz00111", "FMOV $Z, @(R0, $N)"},
    {"1111nnnnlll11010", "FMOV $L, @$N"},
    {"1111kkk1mmmm1000", "FMOV @$M, $K"},
    {"1111kkk1mmmm1001", "FMOV @$M+, $K"},
    {"1111nnnnlll11011", "FMOV $L, @-$N"},
    {"1111kkk1mmmm0110", "FMOV @(R0, $M), $K"},
    {"1111nnnnlll10111", "FMOV $L, @(R0,$N"},
    {"1111kkk1lll11100", "FMOV $L, $K"},
    {"1111qqq0lll11100", "FMOV $L, $Q"},
    {"1111kkk1zzz01100", "FMOV $Z, $K"},
    {"1111wwwwxxxx0010", "FMUL $X, $W"},
    {"1111qqq0zzz00010", "FMUL $Z, $Q"},
    {"1111wwww01001101", "FNEG $W"},
    {"1111qqq001001101", "FNEG $Q"},
    {"1111101111111101", "FRCHG"},
    {"1111001111111101", "FSCHG"},
    {"1111wwww01101101", "FSQRT $W"},
    {"1111qqq001101101", "FSQRT $Q"},
    {"1111wwww00001101", "FSTS FPUL, $W"},
    {"1111wwwwxxxx0001", "FSUB $X, $W"},
    {"1111qqq0zzz00001", "FSUB $Z, $Q"},
    {"1111wwww00111101", "FTRC $W, FPUL"},
    {"1111qqq000111101", "FTRC $Q, FPUL"},
    {"1111cc0111111101", "FTRV XMTRX, $C"},
    {"0100nnnn00101011", "JMP @$N"},
    {"0100nnnn00001011", "JSR @$N"},
    {"0100nnnn00001110", "LDC $N, SR"},
    {"0100nnnn00011110", "LDC $N, GBR"},
    {"0100nnnn00101110", "LDC $N, VBR"},
    {"0100nnnn00111110", "LDC $N, SSR"},
    {"0100nnnn01001110", "LDC $N, SPC"},
    {"0100nnnn11111010", "LDC $N, DBR"},
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
    {"0100nnnn11110110", "LDC.L @$N+, DBR"},
    {"0100nnnn10000111", "LDC.L @$N+, R0_BANK"},
    {"0100nnnn10010111", "LDC.L @$N+, R1_BANK"},
    {"0100nnnn10100111", "LDC.L @$N+, R2_BANK"},
    {"0100nnnn10110111", "LDC.L @$N+, R3_BANK"},
    {"0100nnnn11000111", "LDC.L @$N+, R4_BANK"},
    {"0100nnnn11010111", "LDC.L @$N+, R5_BANK"},
    {"0100nnnn11100111", "LDC.L @$N+, R6_BANK"},
    {"0100nnnn11110111", "LDC.L @$N+, R7_BANK"},
    {"0100nnnn01011010", "LDS $N, FPUL"},
    {"0100nnnn01010110", "LDS.L @$N+, FPUL"},
    {"0100nnnn01101010", "LDS $N, FPSCR"},
    {"0100nnnn01100110", "LDS.L @$N+, FPSCR"},
    {"0100nnnn00001010", "LDS $N, MACH "},
    {"0100nnnn00011010", "LDS $N, MACL"},
    {"0100nnnn00101010", "LDS $N, PR"},
    {"0100nnnn00000110", "LDS.L @$N+, MACH"},
    {"0100nnnn00010110", "LDS.L @$N+, MACL"},
    {"0100nnnn00100110", "LDS.L @$N+, PR"},
    {"0000000000111000", "LDTLB"},
    {"0000nnnnmmmm1111", "MAC.L @$M+, @$N+"},
    {"0100nnnnmmmm1111", "MAC.W @$M+, @$N+ "},
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
    {"0101nnnnmmmmssss", "MOV.L @($S, $M), $N"},
    {"11000111pppppppp", "MOVA @($P, PC), R0"},
    {"0000nnnn11000011", "MOVCA.L R0, @$N"},
    {"0000nnnn00101001", "MOVT $N"},
    {"0000nnnnmmmm0111", "MUL.L $M, $N"},
    {"0010nnnnmmmm1111", "MULS.W $M, $N"},
    {"0010nnnnmmmm1110", "MULU.W $M, $N"},
    {"0110nnnnmmmm1011", "NEG $M, $N"},
    {"0110nnnnmmmm1010", "NEGC $M, $N"},
    {"0000000000001001", "NOP"},
    {"0110nnnnmmmm0111", "NOT $M, $N"},
    {"0000nnnn10010011", "OCBI @$N"},
    {"0000nnnn10100011", "OCBP @$N"},
    {"0000nnnn10110011", "OCBWB @$N"},
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
    {"0100nnnn00000001", "SHLR $N"},
    {"0100nnnn00001001", "SHLR2 $N"},
    {"0100nnnn00011001", "SHLR8 $N"},
    {"0100nnnn00101001", "SHLR16 $N"},
    {"0000000000011011", "SLEEP"},
    {"0000nnnn00000010", "STC SR, $N"},
    {"0000nnnn00010010", "STC GBR, $N"},
    {"0000nnnn00100010", "STC VBR, $N"},
    {"0000nnnn00110010", "STC SSR, $N"},
    {"0000nnnn01000010", "STC SPC, $N"}, 
    {"0000nnnn11111010", "STC DBR, $N"},
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
    {"0100nnnn11110010", "STC.L DBR, @-$N"},
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
    {"0100nnnn00000010", "STS.L MACH, @-$N"},
    {"0100nnnn00010010", "STS.L MACL, @-$N"},
    {"0100nnnn00100010", "STS.L PR, @-$N"},
    {"0000nnnn01011010", "STS FPUL, $N"},
    {"0000nnnn01101010", "STS FPSCR, $N"},
    {"0100nnnn01010010", "STS.L FPUL, @-$N"},
    {"0100nnnn01100010", "STS.L FPSCR, @-$N"},
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
    {"0111nnnniiiiiiii", "ADD #$I, $N"}
};

static std::unordered_map<std::string, std::string> registerMap = {
    {"0000", "R0"}, {"0001", "R1"}, {"0010", "R2"}, {"0011", "R3"},
    {"0100", "R4"}, {"0101", "R5"}, {"0110", "R6"}, {"0111", "R7"},
    {"1000", "R8"}, {"1001", "R9"}, {"1010", "R10"}, {"1011", "R11"},
    {"1100", "R12"}, {"1101", "R13"}, {"1110", "R14"}, {"1111", "R15"}
};

static std::unordered_map<std::string, std::string> FloatregisterMap = {
    {"0000", "FR0"}, {"0001", "FR1"}, {"0010", "FR2"}, {"0011", "FR3"},
    {"0100", "FR4"}, {"0101", "FR5"}, {"0110", "FR6"}, {"0111", "FR7"},
    {"1000", "FR8"}, {"1001", "FR9"}, {"1010", "FR10"}, {"1011", "FR11"},
    {"1100", "FR12"}, {"1101", "FR13"}, {"1110", "FR14"}, {"1111", "FR15"}
};

static std::unordered_map<std::string, std::string> FVregisterMap = {
    {"00", "FV0"}, {"01", "FV4"}, {"10", "FV8"}, {"11", "FV12"}
};

static std::unordered_map<std::string, std::string> DFloatregisterMap = {
    {"000", "DR0"}, {"001", "DR1"}, {"010", "DR2"}, {"011", "DR3"},
    {"100", "DR4"}, {"101", "DR5"}, {"110", "DR6"}, {"111", "DR7"}
};

static std::unordered_map<std::string, std::string> XDFloatregisterMap = {
    {"000", "XD0"}, {"001", "XD1"}, {"010", "XD2"}, {"011", "XD3"},
    {"100", "XD4"}, {"101", "XD5"}, {"110", "XD6"}, {"111", "XD7"}
};

std::string SuperH4(const std::string_view binaryCode) {
    for (const auto& [pattern, assembly] : OpcodeMap) {
        bool match = true;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (std::isalpha(pattern[i])) continue;
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
            std::string regBits = std::string(binaryCode.substr(4, 3));
            result.replace(pos, 2, DFloatregisterMap.at(regBits));
        }
        while ((pos = result.find("$W")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(4, 4));
            result.replace(pos, 2, FloatregisterMap.at(regBits));
        }
        while ((pos = result.find("$Z")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(8, 3));
            result.replace(pos, 2, DFloatregisterMap.at(regBits));
        }
        while ((pos = result.find("$L")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(8, 3));
            result.replace(pos, 2, XDFloatregisterMap.at(regBits));
        }
        while ((pos = result.find("$K")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(4, 3));
            result.replace(pos, 2, XDFloatregisterMap.at(regBits));
        }
        while ((pos = result.find("$X")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(8, 4));
            result.replace(pos, 2, FloatregisterMap.at(regBits));
        }
        while ((pos = result.find("$C")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(4, 2));
            result.replace(pos, 2, FVregisterMap.at(regBits));
        }
        while ((pos = result.find("$V")) != std::string::npos) {
            std::string regBits = std::string(binaryCode.substr(6, 2));
            result.replace(pos, 2, FVregisterMap.at(regBits));
        }
        while ((pos = result.find("$I")) != std::string::npos) {
            std::string bits = std::string(binaryCode.substr(8, 8));
            long val = std::bitset<8>(bits).to_ulong();
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
