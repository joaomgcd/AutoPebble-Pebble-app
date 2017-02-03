#include "AutoPebble.h"

void logString(char * text);
void logChar(char text);
void logNr(int nr);
bool contains(char * str, char * containsThis);
char * strndup (const char *s, size_t n);
int indexOf(char* source, char toFind);
char * subStringBefore(char * source, char separator);
char * subStringAfter(char * source, char separator);
uint32_t countString(char * source, char charToFind);
void resetArray(char ** arr, int sizeOfArray);
bool isTimeVariable(char * str);
char * resetString(char * destination, char * str);
char * setString(char * destination, char * str);
char** setItemInArray(char** arr, int sizeOfArray, int index, char * str);


typedef struct IntsFromCsv{
	uint32_t * nrs;
	uint32_t size;
}IntsFromCsv;
struct IntsFromCsv * getIntsFromCsv(char * str);

bool sendCommandToPhone(uint32_t key, const char* text);
void sendIntCommandToPhone(uint32_t key, uint32_t command);
bool sendCommandActionToPhone(const char* text);
bool sendActionWithPrefixToPhone(const char* prefix, const char* text);
bool getShouldPopWindowAfterCommand();
void setShouldPopWindowAfterCommand(bool should);