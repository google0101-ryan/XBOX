#include <app/Application.h>

int main(int argc, char** argv)
{
    if (!Application::Init(argc, argv))
    {
        printf("Cannot initialize application, exiting\n");
        return -1;
    }

    Application::Run();

    return 0;
}