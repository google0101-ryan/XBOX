#include <app/Application.h>
#include <cstdlib>

bool Application::initialized = false;
System* _sys = nullptr;

bool Application::Init(int, char**)
{
    std::atexit(Exit);

    _sys = new System();

    if (!_sys)
    {
        printf("ERR: Could not initialize system\n");
        return false;
    }

    return true;
}

void Application::Run()
{
    while (1)
        _sys->Clock();
}

void Application::Exit()
{
    _sys->Dump();
    printf("Exiting\n");
}

void Application::Exit(int code)
{
    exit(code);
}