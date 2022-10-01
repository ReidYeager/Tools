
#ifndef YTOOLS_LOGGER_H_
#define YTOOLS_LOGGER_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace Ytools {

enum LogTypes
{
  Log_Type_Info,
  Log_Type_Debug,
  Log_Type_Warning,
  Log_Type_Error,
  Log_Type_Fatal
};

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
void PrintToConsole(const char* _message, unsigned int _color)
{
  //                        Info , Debug, Warning, Error , Fatal
  //                        White, Cyan , Yellow , Red   , White-on-Red
  unsigned int colors[] = { 0xf  , 0xb  , 0xe    , 0x4   , 0xcf };

  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(console, colors[_color]);
  OutputDebugStringA(_message);
  unsigned long long length = strlen(_message);
  LPDWORD written = 0;
  WriteConsoleA(console, _message, (DWORD)length, written, 0);
  SetConsoleTextAttribute(console, 0xf);
}
#endif // Platforms


void LoggerAssembleMessage(LogTypes _type, const char* _message, ...)
{
  // Limit 2048 characters per message
  const short length = 0x800;
  char outMessage[0x800];
  //memset(outMessage, 0, length);

  va_list args;
  va_start(args, _message);
  vsnprintf(outMessage, length, _message, args);
  va_end(args);

  PrintToConsole(outMessage, _type);
}

} // namespace Ytools

#ifdef _DEBUG
#define LogInfo(message, ...)                                      \
{                                                                  \
  Ytools::LoggerAssembleMessage(Ytools::Log_Type_Info, message, __VA_ARGS__); \
  Ytools::LoggerAssembleMessage(Ytools::Log_Type_Info, "\n");                 \
}
   
#define LogDebug(message, ...)                                      \
{                                                                   \
  Ytools::LoggerAssembleMessage(Ytools::Log_Type_Debug, message, __VA_ARGS__); \
  Ytools::LoggerAssembleMessage(Ytools::Log_Type_Debug, "\n");                 \
}

#define LogWarning(message, ...)                                   \
{                                                                     \
  Ytools::LoggerAssembleMessage(Ytools::Log_Type_Warning, message, __VA_ARGS__); \
  Ytools::LoggerAssembleMessage(Ytools::Log_Type_Warning, "\n");                 \
}
#else
#define LogInfo(message, ...)
#define LogDebug(message, ...)
#define LogWarning(message, ...)
#endif // ICE_DEBUG

#define LogError(message, ...)                                      \
{                                                                   \
  Ytools::LoggerAssembleMessage(Ytools::Log_Type_Error, message, __VA_ARGS__); \
  Ytools::LoggerAssembleMessage(Ytools::Log_Type_Error, "\n");                 \
}

#define LogFatal(message, ...)                                      \
{                                                                   \
  Ytools::LoggerAssembleMessage(Ytools::Log_Type_Fatal, message, __VA_ARGS__); \
  Ytools::LoggerAssembleMessage(Ytools::Log_Type_Fatal, "\n");                 \
}

#endif // YTOOLS_LOGGER_H_

