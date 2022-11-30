#pragma once

#include <cstdint>
#include <cstdio>

#include <emu/System.h>

extern System* _sys;

class Application
{
private:
    static bool initialized;
    static void Exit();
public:
    static bool Init(int, char**);

    static void Run();
    static void Exit(int code);
};