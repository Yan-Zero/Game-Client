#ifndef YAN_UI_HPP_
#define YAN_UI_HPP_

#include <string>
#ifdef _WIN32
#include <windows.h>

void Get_Console_Cursor_Position(short &x,short &y, HANDLE hConsoleOutput = NULL)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    x = csbi.dwCursorPosition.X;
    y = csbi.dwCursorPosition.Y;
}
void Set_Console_Cursor_Position(short x,short y, HANDLE hConsoleOutput = NULL)
{
    if(hConsoleOutput == NULL)
        hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hConsoleOutput, {x, y});
}
void Move_Console_Cursor_To_Line_Begin(HANDLE hConsoleOutput = NULL)
{
  short x, y;
  Get_Console_Cursor_Position(x, y, hConsoleOutput);
  Set_Console_Cursor_Position(0, y, hConsoleOutput);
}
void Move_Console_Cursor_To_Next_Line(HANDLE hConsoleOutput = NULL)
{
  short x, y;
  Get_Console_Cursor_Position(x, y, hConsoleOutput);
  Set_Console_Cursor_Position(0, y + 1, hConsoleOutput);
}
void Move_Console_Cursor_To_Previous_Line(HANDLE hConsoleOutput = NULL)
{
  short x, y;
  Get_Console_Cursor_Position(x, y, hConsoleOutput);
  Set_Console_Cursor_Position(0, y - 1, hConsoleOutput);
}
std::string Read_Console_Line(short y, short len, HANDLE hConsoleOutput = NULL)
{
  if(hConsoleOutput == NULL)
    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  // use ReadConsoleOutputCharacterA
  char *buf = new char[len + 1];
  DWORD read;
  ReadConsoleOutputCharacterA(hConsoleOutput, buf, len, {0, y}, &read);
  buf[read] = '\0';
  std::string ret(buf);
  delete[] buf;
  return ret;
}

#endif

// class UI
// {

// }

#endif