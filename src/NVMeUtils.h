#pragma once

enum {
	CMD_TYPE_READ = 0,
	CMD_TYPE_WRITE,
	CMD_TYPE_DEALLOCATE,
	CMD_TYPE_IDENTIFY,
	CMD_TYPE_GET_LOG_PAGE,
	CMD_TYPE_GET_FEATURE,
	CMD_TYPE_QUIT,
	CMD_TYPE_UNKNOWN
};

char* strConvertUTF8toMultiByte(const char* _str);
void printASCII(const char* _strLabel, const char* _strData);
int eGetCommandFromConsole(void);
int iGetConsoleInputHex(const char* _strPrompt, char* _strInput);
char cGetConsoleInput(const char* _strPrompt, char* _strInput);

