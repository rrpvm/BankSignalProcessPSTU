#pragma once

#include "..\data\AppConfig.hpp"

class CommandLineParser final
{
public:
    static AppConfig parse(int argc, char** argv);

private:
    static bool equals(const char* left, const char* right);
};