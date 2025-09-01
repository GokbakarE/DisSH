/*
 * This code is licensed under the GNU AGPLv3
 * Copyright (c) 2025 GokbakarE
 * Date: 28-08-2025
 */

#include <iostream>
#include <string>
#include <string_view>
#include <cstdlib>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <optional>
#include <cctype>

#include "SuperH1.hpp"
#include "SuperH2.hpp"
#include "SuperH3DSP.hpp"
#include "SuperH3E.hpp"
#include "SuperH3.hpp"
#include "SuperH4A.hpp"
#include "SuperH4.hpp"
#include "SuperHDSP.hpp"

void printUsage(const char* progName) {
    std::cout << "Usage:\n"
              << "  " << progName << " --SuperH* <binarystring>\n"
              << "  " << progName << " --file <filename> [section] [--SuperH*] [--number <N>]\n\n"
              << "Options:\n"
              << "  --SuperH*      Choose the target ISA architecture. Supported:\n"
              << "                 --SuperH1, --SuperH2, --SuperH3, --SuperH3E,\n"
              << "                 --SuperH3DSP, --SuperH4, --SuperH4A, --SuperHDSP\n"
              << "  <binarystring> A 16-bit binary instruction string (e.g., 1100001111000011)\n"
              << "  <filename>     ELF file or binary to analyze with objdump\n"
              << "  [section]      Optional section to filter (e.g., .text)\n"
              << "  --number <N>   Optional limit on number of instructions to decode (default: 50)\n\n"
              << "Examples:\n"
              << "  " << progName << " --SuperH4 1100001111000011\n"
              << "  " << progName << " --file program.elf .text --SuperH1 --number 122\n"
              << "  " << progName << " --help\n";
}

bool isHexChar(char c) {
    return std::isxdigit(static_cast<unsigned char>(c));
}

unsigned char hexPairToByte(char high, char low) {
    auto hexVal = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        throw std::runtime_error("Invalid hex digit");
    };
    return (hexVal(high) << 4) | hexVal(low);
}

bool processDisSHFile(const std::string& disshFilename, const std::optional<std::string>& section = std::nullopt) {
    std::ifstream file(disshFilename);
    if (!file) {
        std::cerr << "Failed to open file: " << disshFilename << "\n";
        return false;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) lines.push_back(line);
    file.close();

    auto split = [](const std::string& str) {
        std::istringstream iss(str);
        std::vector<std::string> tokens;
        std::string token;
        while (iss >> token) tokens.push_back(token);
        return tokens;
    };

    auto containsDot = [](const std::string& w) {
        return w.find('.') != std::string::npos;
    };

    if (section) {
        std::string sectionHeader = "Contents of section " + *section + ":";
        int start = -1;
        for (size_t i = 0; i < lines.size(); ++i)
            if (lines[i].find(sectionHeader) != std::string::npos)
                start = static_cast<int>(i);

        if (start == -1) {
            std::cerr << "Section not found: " << *section << "\n";
            return false;
        }

        int end = static_cast<int>(lines.size());
        for (size_t i = start + 1; i < lines.size(); ++i)
            if (lines[i].find("Contents") != std::string::npos) {
                end = static_cast<int>(i);
                break;
            }

        lines = std::vector<std::string>(lines.begin() + start, lines.begin() + end);
        if (!lines.empty()) lines.erase(lines.begin());
    } else {
        if (lines.size() > 2)
            lines.erase(lines.begin(), lines.begin() + 2);
    }

    std::vector<std::string> cleaned;
    for (const auto& l : lines) {
        if (l.find("Contents") != std::string::npos) continue;
        auto words = split(l);
        if (words.size() <= 2) continue;

        std::vector<std::string> mid(words.begin() + 1, words.end() - 1);
        if (mid.size() > 4) mid.erase(mid.begin() + 4);
        mid.erase(std::remove_if(mid.begin(), mid.end(), containsDot), mid.end());

        std::string joined;
        for (auto& w : mid) joined += w;
        cleaned.push_back(joined);
    }

    std::ofstream out(disshFilename, std::ios::trunc);
    if (!out) {
        std::cerr << "Failed to write file: " << disshFilename << "\n";
        return false;
    }

    for (auto& l : cleaned) out << l << "\n";
    return true;
}

std::string hexToBinaryString(const std::string& hex4) {
    static const char* lut[16] = {
        "0000","0001","0010","0011","0100","0101","0110","0111",
        "1000","1001","1010","1011","1100","1101","1110","1111"
    };

    if (hex4.length() != 4)
        throw std::runtime_error("Invalid hex length, expected 4 characters.");

    std::string bin;
    for (char c : hex4) {
        if (!isHexChar(c)) throw std::runtime_error("Invalid hex character.");
        int val = std::isdigit(c) ? c - '0' : std::tolower(c) - 'a' + 10;
        bin += lut[val];
    }
    return bin;
}

using ISAFunction = std::string(*)(std::string_view);

ISAFunction getISAFunction(const std::string& isaFlag) {
    if (isaFlag == "--SuperH1") return SuperH1;
    if (isaFlag == "--SuperH2") return SuperH2;
    if (isaFlag == "--SuperH3DSP") return SuperH3DSP;
    if (isaFlag == "--SuperH3E") return SuperH3E;
    if (isaFlag == "--SuperH3") return SuperH3;
    if (isaFlag == "--SuperH4A") return SuperH4A;
    if (isaFlag == "--SuperH4") return SuperH4;
    if (isaFlag == "--SuperHDSP") return SuperHDSP;
    return nullptr;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || std::string(argv[1]) == "--help") {
        printUsage(argv[0]);
        return 0;
    }

    std::string arg1 = argv[1];

    if (arg1 == "--file") {
        if (argc < 3) {
            std::cerr << "--file requires a filename.\n";
            printUsage(argv[0]);
            return 1;
        }

        std::string filename = argv[2];
        std::optional<std::string> section = std::nullopt;
        ISAFunction isaFunc = SuperH4;  
        size_t numberToProcess = 50;    

        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.rfind("--SuperH", 0) == 0) {
                isaFunc = getISAFunction(arg);
                if (!isaFunc) {
                    std::cerr << "Unknown ISA flag: " << arg << "\n";
                    return 1;
                }
            } else if (arg == "--number") {
                if (i + 1 >= argc) {
                    std::cerr << "--number requires a value.\n";
                    return 1;
                }
                numberToProcess = std::stoul(argv[++i]);
            } else if (!section.has_value()) {
                section = arg;
            }
        }

        std::string cmd = "objdump -s " + filename + " >> " + filename + ".DisSH";
        if (system(cmd.c_str()) != 0) {
            std::cerr << "Failed to run objdump.\n";
            return 1;
        }

        if (!processDisSHFile(filename + ".DisSH", section)) {
            std::cerr << "File processing failed.\n";
            return 1;
        }

        std::ifstream processed(filename + ".DisSH");
        if (!processed) {
            std::cerr << "Processed file could not be read.\n";
            return 1;
        }

        std::vector<std::string> allChunks;
        std::string line;
        while (std::getline(processed, line)) {
            for (size_t i = 0; i + 4 <= line.size(); i += 4)
                allChunks.push_back(line.substr(i, 4));
        }

        size_t limit = std::min(numberToProcess, allChunks.size());
        for (size_t i = 0; i < limit; ++i) {
            try {
                std::string binaryStr = hexToBinaryString(allChunks[i]);
                std::string result = isaFunc(binaryStr);
                std::cout << "[" << allChunks[i] << "] -> " << result << "\n";
            } catch (const std::exception& e) {
                std::cerr << "Conversion error (" << allChunks[i] << "): " << e.what() << "\n";
            }
        }

        return 0;
    }

    if (argc != 3) {
        std::cerr << "Invalid usage.\n";
        printUsage(argv[0]);
        return 1;
    }

    std::string isaArg = argv[1];
    std::string_view binaryCode = argv[2];

    try {
        ISAFunction isaFunc = getISAFunction(isaArg);
        if (!isaFunc) {
            std::cerr << "Unknown ISA: " << isaArg << "\n";
            return 1;
        }

        std::string result = isaFunc(binaryCode);
        std::cout << "Assembly: " << result << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
