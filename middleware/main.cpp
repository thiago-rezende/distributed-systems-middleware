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

        /* Middleware Instance */
        Middleware middleware;

        /* Start middleware on given port */
        middleware.run(port);

        /* Interaction */
        bool done = false;
        std::string input;

        std::string header = "Distributed Systems Middleware [v1.0.0]\n"
                             "type 'help' to see the commands list\n";

        std::string help = "\n[command]    - [description]\n"
                           "quit         - close all connections and quit\n"
                           "help         - show this help message\n";

        std::cout << header << std::endl;

        while (!done)
        {
            std::cout << ">>> ";
            std::getline(std::cin, input);

            if (input == "quit")
                done = true;
            else if (input == "help")
                std::cout << help << std::endl;
            else
                std::cout << "\n!> unrecognized command\ntype 'help' to see the commands list\n " << std::endl;
        }

        /* Terminate middleware */
        middleware.stop();
    }

    /* Ends Profile Session */
    H_PROFILE_END_SESSION();

    return 0;
}
