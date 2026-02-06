#include "console.h"
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#endif

void Console::Print(const std::string& text)
{
    std::cout << text;
}

void Console::PrintLine(const std::string& text)
{
    std::cout << text << std::endl;
}

void Console::Clear()
{
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE)
        return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;
    COORD homeCoords = { 0, 0 };

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
        return;

    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(
        hConsole,
        ' ',
        cellCount,
        homeCoords,
        &count
    );

    FillConsoleOutputAttribute(
        hConsole,
        csbi.wAttributes,
        cellCount,
        homeCoords,
        &count
    );

    SetConsoleCursorPosition(hConsole, homeCoords);
#else
    // ANSI escape fallback
    std::cout << "\033[2J\033[H";
#endif
}