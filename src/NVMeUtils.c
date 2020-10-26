#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#include "WinFunc.h"
#include "NVMeUtils.h"

void PrintDataBuffer(PUCHAR DataBuffer, ULONG BufferLength)
{
    ULONG Cnt;
    UCHAR Str[32] = { 0 };

    printf("        00  01  02  03  04  05  06  07   08  09  0A  0B  0C  0D  0E  0F\n");
    printf("        ---------------------------------------------------------------\n");

    int i = 0;
    for (Cnt = 0; Cnt < BufferLength; Cnt++)
    {
        // print address
        if ((Cnt) % 16 == 0)
        {
            printf(" 0x%03X  ", Cnt);
        }

        // print hex data
        printf("%02X  ", DataBuffer[Cnt]);
        if (isprint(DataBuffer[Cnt]))
        {
            Str[i] = DataBuffer[Cnt];
        }
        else
        {
            Str[i] = '.';
        }
        i++;
        if ((Cnt + 1) % 8 == 0)
        {
            printf(" ");
            Str[i++] = ' ';
        }

        // print ascii character if printable
        if ((Cnt + 1) % 16 == 0)
        {
            Str[i++] = '\0';
            i = 0;
            printf("%s\n", Str);
        }
    }
    printf("\n\n");
}

char* strConvertUTF8toMultiByte(const char* _str)
{
    int numChar = 0;
    wchar_t* strTemp = NULL;
    char* strDest = NULL;

    // 1) Convert from UTF-8 to UTF-16
    // see https://docs.microsoft.com/en-us/windows/desktop/api/stringapiset/nf-stringapiset-multibytetowidechar

    // Note that, numChar includes a terminating NULL character, because we specified -1 to the parameter cbMultiByte
    numChar = MultiByteToWideChar(CP_UTF8, 0, _str, -1, NULL, 0);
    if (numChar == 0)
    {
        vPrintSystemError(GetLastError(), "MultiByteToWideChar");
        goto error_exit;
    }

    // we need "wide char" ...
    strTemp = (wchar_t*)calloc(numChar, sizeof(wchar_t));
    if (strTemp == NULL)
    {
        vPrintSystemError(GetLastError(), "calloc");
        goto error_exit;
    }

    if (MultiByteToWideChar(CP_UTF8, 0, _str, -1, (LPWSTR)strTemp, numChar) == 0)
    {
        vPrintSystemError(GetLastError(), "MultiByteToWideChar");
        goto error_exit;
    }

    // 2) Convert from UTF-16 to UTF-16
    // see https://docs.microsoft.com/ja-jp/windows/desktop/api/stringapiset/nf-stringapiset-widechartomultibyte
    numChar = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)strTemp, -1, NULL, 0, NULL, NULL);
    if (numChar == 0)
    {
        vPrintSystemError(GetLastError(), "WideCharToMultiByte");
        goto error_exit;
    }

    // we needs "multi byte"...
    strDest = (char*)calloc(((long long)numChar)*2, sizeof(char));
    if (WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)strTemp, -1, (LPSTR)strDest, numChar, NULL, NULL) == 0)
    {
        vPrintSystemError(GetLastError(), "WideCharToMultiByte");
    }

error_exit:
    if (strTemp != NULL)
    {
        free(strTemp);
    }
    return strDest;
}

void printASCII(const char* _strLabel, const char* _strData, bool _bNewLine)
{
    printf("%s", _strLabel);
    size_t len = strlen(_strData);
    for (int i = 0; i < len; i++)
    {
        if (!isascii(_strData[i]) || iscntrl(_strData[i]))
        {
            break; // stop printing
        }
        printf("%c", _strData[i]);
    }
    if ( _bNewLine ) printf("\n");
}

//
// @return                  top character of user input from stdin
// @arg _strPrompt  [in]    prompt string to be displayed to user
// @arg _strInput   [out]   user input from stdin
//
char cGetConsoleInput(const char* _strPrompt, char* _strInput)
{
    printf("%s\n", _strPrompt);
    printf("> ");
    fgets(_strInput, 128, stdin);

    return _strInput[0];
}

//
// @return                  integer value (in hex) converted from stdin
// @arg _strPrompt  [in]    prompt string to be displayed to user
// @arg _strInput   [out]   user input from stdin
//
int iGetConsoleInputHex(const char* _strPrompt, char* _strInput)
{
    printf("%s\n", _strPrompt);
    printf("> ");
    fgets(_strInput, 128, stdin);

    int iRet = 0;
    if (sscanf_s(_strInput, "%x", &iRet) != 1)
    {
        fprintf(stderr, "[E] Something wrong in getting input.\n\n");
    }
    return iRet;
}

//
// @return                  integer value (in decimal) converted from stdin
// @arg _strPrompt  [in]    prompt string to be displayed to user
// @arg _strInput   [out]   user input from stdin
//
int iGetConsoleInputDec(const char* _strPrompt, char* _strInput)
{
    printf("%s\n", _strPrompt);
    printf("> ");
    fgets(_strInput, 128, stdin);

    int iRet = 0;
    if (sscanf_s(_strInput, "%d", &iRet) != 1)
    {
        fprintf(stderr, "[E] Something wrong in getting input.\n\n");
    }
    return iRet;
}

int eGetCommandFromConsole(void)
{
    char cCmd;
    char strCmd[256];
    int iMajorCmd = NVME_TOOL_COMMAND_UNKNOWN;

    cCmd = cGetConsoleInput(
        "\n# Input command:\n"
        "#  - r: Read data from LBA = 0\n"
        "#  - w: Write data to LBA = 0\n"
        "#  - d: Deallocate LBA = 0\n"
        "#  - l: Get Log Page\n"
        "#  - i: Identify\n"
        "#  - g: Get Feature\n"
        "#  - s: Set Feature\n"
        "#  - f: Flush\n"
        "#  - t: Device Self-test\n"
        "#  - z: Format NVM\n"
        "#  - v: Security Receive\n"
        "#\n"
        "#  press 'q' to quit program\n",
        strCmd);

    switch (cCmd)
    {
    case 'q':
    case 'Q':
        iMajorCmd = NVME_TOOL_COMMAND_QUIT; // quit this program
        break;

    case 'r':
    case 'R':
        cCmd = cGetConsoleInput("\n# Read data from LBA = 0, Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iMajorCmd = NVME_COMMAND_READ;
        }
        break;

    case 'w':
    case 'W':
        cCmd = cGetConsoleInput("\n# Write data to LBA = 0, Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iMajorCmd = NVME_COMMAND_WRITE;
        }
        break;

    case 'd':
    case 'D':
        cCmd = cGetConsoleInput("\n# Deallocate LBA = 0, Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iMajorCmd = NVME_COMMAND_DSM;
        }
        break;

    case 'i':
    case 'I':
        iMajorCmd = NVME_COMMAND_IDENTIFY;
        break;

    case 'l':
    case 'L':
        iMajorCmd = NVME_COMMAND_GET_LOG_PAGE;
        break;

    case 'g':
    case 'G':
        iMajorCmd = NVME_COMMAND_GET_FEATURES;
        break;

    case 's':
    case 'S':
        iMajorCmd = NVME_COMMAND_SET_FEATURES;
        break;

    case 't':
    case 'T':
        cCmd = cGetConsoleInput("\n# Device Self-test, Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iMajorCmd = NVME_COMMAND_DST;
        }
        break;

    case 'f':
    case 'F':
        cCmd = cGetConsoleInput("\n# Flush, Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iMajorCmd = NVME_COMMAND_FLUSH;
        }
        break;

    case 'z':
    case 'Z':
        printf("\n# Format NVM : CAUTION!! all data are erased on the target drive");
        cCmd = cGetConsoleInput("\n# Format NVM : Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iMajorCmd = NVME_COMMAND_FORMAT_NVM;
        }
        break;

    case 'v':
    case 'V':
        cCmd = cGetConsoleInput("\n# Security Receive, Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iMajorCmd = NVME_COMMAND_SECURITY_RECV;
        }
        break;

    default:
        break;
    }

    printf("\n");
    return iMajorCmd;
}
