#pragma once
#include <string>

class Console
{
public:
    // Output
    static void Print(const std::string& text);
    static void PrintLine(const std::string& text);

    // Control
    static void Clear();
};