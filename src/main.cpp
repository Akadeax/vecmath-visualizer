#include "VecmathVisualizer.h"

#include <cstdlib>
#include <iostream>

int main()
{
    vmv::VecmathVisualizer app{};

    try
    {
        app.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
