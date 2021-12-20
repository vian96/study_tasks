// TODO написать на почту деду, что файл пишется только в одном из файлов (и каком)

#define TX_USE_SPEAK
#include "E:/WindowsFolders/Desktop/prog/TX/TXLib.h"

#define AT_SAY(...) txSpeak ("\v" __VA_ARGS__)
