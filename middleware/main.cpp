/**
 * @file main.cpp
 * @brief Application Entry Point
 *
 */

/* Args parser */
#include <clipp.h>

#include "middleware.hpp"

/* Application Entry Point */
int main(int argc, char **argv)
{
    /* Args variables */
    uint16_t port = 9002;

    /* Set cli options */
    clipp::group cli(clipp::required("-p", "--port").doc("port to listen on") & clipp::value("port", port));

    /* Parse the args */
    if (!clipp::parse(argc, argv, cli))
    {
        /* Show help */
        std::cout << clipp::make_man_page(cli, "middleware");

        /* End the profile session */
        H_PROFILE_END_SESSION();
        return 0;
    }

    /* Starts Profile Session */
    H_PROFILE_BEGIN_SESSION("Application Profile", "profile_results.json");

    {
        /* Profiles the Main Function */
        H_PROFILE_SCOPE("Main Scope");

        /* Initialize Logger */
        Horus::Logger::init();

        Middleware middleware;

        middleware.run(port);

        std::cin.get();

        middleware.stop();
    }

    /* Ends Profile Session */
    H_PROFILE_END_SESSION();

    return 0;
}
