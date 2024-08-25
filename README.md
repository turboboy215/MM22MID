# MM22MID
Mega Man 2 (Game Boy/Game Boy Color) to MIDI converter

This tool converts music from Game Boy and Game Boy Color games using Hirotomo Nakamura (of Giraffe Soft)'s sound engine. Giraffe Soft worked on audio development for various games, most notably Mega Man II. Most of the games using this driver were only released in Japan. The driver was also used in Blaster Master: Enemy Below for Game Boy Color.
It works with ROM images. To use it, you must specify the name of the ROM followed by the number of the bank containing the sound data (in hex).

Examples:
* MM22MID "Megaman II (E) [!].gb" 8
* MM22MID "Kung-Fu Master (U).gb" 3
* MM22MID "Blaster Master - Enemy Below (U) [C][!].gbc" 40

This tool was based on my own reverse-engineering, with almost no actual disassembly involved. This driver is rather "messy" compared to most of the others that I have looked at, with an odd method of playing notes switching between octaves. Blaster Master uses completely different note values than other games, and it is also fully supported, although there are some minor desync issues in some songs due to anomalies regarding "hold note" commands.
Like most of my other programs, another converter, MM22TXT, is also included, which prints out information about the song data from each game. This is essentially a prototype of MM22TXT.

Supported games:
* Ayakashi no Shiro
* Blaster Master: Enemy Below
* Ca Da
* Kung-Fu Master
* Mega Man II
* Rentaiou
* The Shinri Game
* The Shinri Game 2: Daihan Hen
* Taiyou no Yuusha: Firebird GB
* Trump Boy
* Trump Boy II
* Undercover Cops Gaiden: Hakaishin Garumaa

Note: Nanonote also uses the driver but doesn't seem to have any music.

## To do:
  * Panning support
  * GBS file support
