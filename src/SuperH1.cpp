/*
 * This code is licensed under the GNU AGPLv3
 * Copyright (c) 2025 GokbakarE
 * Date: 28-08-2025
 */

#include "SuperH1.hpp"
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
    {"11001001iiiiiiii", "AND $I, R0"},
    {"11001101iiiiiiii", "AND.B $I, @(R0, GBR)"},
    {"10001011dddddddd", "BF $D"},
    {"1010ffffffffffff", "BRA $F"},
    {"10001001dddddddd", "BT $D"},
    {"0000000000101000", "CLRMAC"},
    {"0000000000001000", "CLRT"},
    {"0011nnnnmmmm0000", "CMP/EQ $M, $N"},
    {"0011nnnnmmmm0011", "CMP/GE $M, $N"},
    {"0011nnnnmmmm0111", "CMP/GT $M, $N"},
    {"0011nnnnmmmm0110", "CMP/HI $M, $N"},
    {"0011nnnnmmmm0010", "CMP/HS $M, $N"},
    {"0100nnnn00010101", "CMP/PL $N"},
    {"0100nnnn00010001", "CMP/PZ $N"},
    {"0010nnnnmmmm1100", "CMP/STR $M, $N"},
    {"10001000iiiiiiii", "CMP/EQ $I, R0"},
    {"0010nnnnmmmm0111", "DIV0S $M, $N"},
    {"0000000000011001", "DIV0U"},
    {"0011nnnnmmmm0100", "DIV1 $M, $N"},
    {"0110nnnnmmmm1110", "EXTS.B $M, $N"},
    {"0110nnnnmmmm1111", "EXTS.W $M, $N"},
    {"0110nnnnmmmm1100", "EXTU.B $M, $N"},
    {"0110nnnnmmmm1101", "EXTU.W $M, $N"},
    {"0100nnnn00101011", "JMP @$N"},
    {"0100nnnn00001011", "JSR @$N"},
    {"0100nnnn00001110", "LDC $N, SR"},
    {"0100nnnn00011110", "LDC $N, GBR"},
    {"0100nnnn00101110", "LDC $N, VBR"},
    {"0100nnnn01011110", "LDC $N, MOD"},
    {"0100nnnn01111110", "LDC $N, RE"},
    {"0100nnnn01101110", "LDC $N, RS"},
    {"0100nnnn00000111", "LDC.L @$N+, SR"},
    {"0100nnnn00010111", "LDC.L @$N+, GBR"},
    {"0100nnnn00100111", "LDC.L @$N+, VBR"},
    {"0100nnnn01010111", "LDC.L @$N+, MOD"},
    {"0100nnnn01110111", "LDC.L @$N+, RE"},
    {"0100nnnn01100111", "LDC.L @$N+, RS"},
    {"0100nnnn00001010", "LDS $N, MACH"},
    {"0100nnnn00011010", "LDS $N, MACL"},
    {"0100nnnn00101010", "LDS $N, PR"},
    {"0100nnnn00000110", "LDS.L @$N+, MACH"},
    {"0100nnnn00010110", "LDS.L @$N+, MACL"},
    {"0100nnnn00100110", "LDS.L @$N+, PR"},
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
    {"1110nnnniiiiiiii", "MOV $I, $N"},
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
    {"0010nnnnmmmm1111", "MULS.W $M, $N"},
    {"0010nnnnmmmm1110", "MULU.W $M, $N"},
    {"0110nnnnmmmm1011", "NEG $M, $N"},
    {"0110nnnnmmmm1010", "NEGC $M, $N"},
    {"0000000000001001", "NOP"},
    {"0110nnnnmmmm0111", "NOT $M, $N"},
    {"0010nnnnmmmm1011", "OR $M, $N"},
    {"11001011iiiiiiii", "OR $I, R0"},
    {"11001111iiiiiiii", "OR.B $I, @(R0, GBR)"},
    {"0100nnnn00100100", "ROTCL $N"},
    {"0100nnnn00100101", "ROTCR $N"},
    {"0100nnnn00000100", "ROTL $N"},
    {"0100nnnn00000101", "ROTR $N"},
    {"0000000000101011", "RTE"},
    {"0000000000001011", "RTS"},
    {"0000000000011000", "SETT"},
    {"0100nnnn00100000", "SHAL $N"},
    {"0100nnnn00100001", "SHAR $N"},
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
    {"0100nnnn00000011", "STC.L SR, @-$N"},
    {"0100nnnn00010011", "STC.L GBR, @-$N"},
    {"0100nnnn00100011", "STC.L VBR, @-$N"},
    {"0000nnnn00001010", "STS MACH, $N"},
    {"0000nnnn00011010", "STS MACL, $N"},
    {"0000nnnn00101010", "STS PR, $N"},
    {"0100nnnn00000010", "STS.L MACH, @-$N"},
    {"0100nnnn00010010", "STS.L MACL, @-$N"},
    {"0100nnnn00100010", "STS.L PR, @-$N"},
    {"0011nnnnmmmm1000", "SUB $M, $N"},
    {"0011nnnnmmmm1010", "SUBC $M, $N"},
    {"0011nnnnmmmm1011", "SUBV $M, $N"},
    {"0110nnnnmmmm1000", "SWAP.B $M, $N"},
    {"0110nnnnmmmm1001", "SWAP.W $M, $N"},
    {"0100nnnn00011011", "TAS.B $N"},
    {"11000011iiiiiiii", "TRAPA $I"},
    {"0010nnnnmmmm1000", "TST $M, $N"},
    {"11001000iiiiiiii", "TST $I, R0"},
    {"11001100iiiiiiii", "TST.B $I, @(R0, GBR)"},
    {"0010nnnnmmmm1010", "XOR $M, $N"},
    {"11001010iiiiiiii", "XOR $I, R0"},
    {"11001110iiiiiiii", "XOR.B $I, @(R0, GBR)"},
    {"0010nnnnmmmm1101", "XTRCT $M, $N"},
    {"0111nnnniiiiiiii", "ADD $I, $N"}
};

static const std::unordered_map<std::string_view, std::string_view> registerMap = {
    {"0000", "R0"}, {"0001", "R1"}, {"0010", "R2"}, {"0011", "R3"},
    {"0100", "R4"}, {"0101", "R5"}, {"0110", "R6"}, {"0111", "R7"},
    {"1000", "R8"}, {"1001", "R9"}, {"1010", "R10"}, {"1011", "R11"},
    {"1100", "R12"}, {"1101", "R13"}, {"1110", "R14"}, {"1111", "R15"}
};

std::string SuperH1(const std::string_view binaryCode) {
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
