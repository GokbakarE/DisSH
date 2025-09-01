#This code is licensed under the GNU AGPLv3
#Copyright (c) 2025 GokbakarE
#Date: 28-08-2025
all:
	g++ -std=c++20 src/*.cpp -o DisSH.elf

clean:
	rm -f DisSH