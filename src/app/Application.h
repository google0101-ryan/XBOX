#pragma once

#include <cstdint>
#include <cstdio>

#include <emu/System.h>

class Application
{
private:
    static bool initialized;
    static void Exit();

    static System* _sys;
public:
    static bool Init(int, char**);

    static void Run();
    static void Exit(int code);
};