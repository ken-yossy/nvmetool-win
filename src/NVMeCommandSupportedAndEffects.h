#pragma once

#include <windows.h>

/**
 * iVerboseLevel
 *  0: very simple
 *  1: do not display opcodes of unsupported commands (default)
 *  2: display opcodes of unsupported commands
 */
int iNVMeGetCommandSupportedAndEffects(HANDLE _hDevice, int iVerboseLevel);
