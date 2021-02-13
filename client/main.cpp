/**
 * @file main.cpp
 * @brief Application Entry Point
 *
 */

/* Args parser */
#include <clipp.h>

#include "client.hpp"

/* Application Entry Point */
int main(int argc, char **argv)
{
    /* Args variables */
    std::string host;
    uint16_t port = 9002;
    std::string name;

    /* Set cli options */
    clipp::group cli(
        clipp::required("-h", "--host").doc("middleware host") & clipp::value("host", host),
        clipp::required("-p", "--port").doc("port to listen on") & clipp::value("port", port),
        clipp::required("-n", "--name").doc("client name") & clipp::value("name", name));

    /* Parse the args */
    if (!clipp::parse(argc, argv, cli))
    {
        /* Show help */
        std::cout << clipp::make_man_page(cli, "client");

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

        /* Client Instance */
        Client client;

        /* Start client with given host:port */
        client.run(host, port, name);

        /* Interaction */
        bool done = false;
        std::string input;

        std::string header = "Distributed Systems Client [v1.0.0]\n"
                             "type 'help' to see the commands list\n";

        std::string help = "\n[command]    - [description]\n"
                           "name <text>  - update the name of a agent\n"
                           "state <0|1>  - update the state of a agent [ON|OFF]\n"
                           "quit         - close the connection and quit\n"
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
            else if (input.substr(0, 4) == "name")
            {
                std::string new_name = input.substr(5);
                if (new_name == "")
                {
                    std::cout << "\n!> invalid name\n"
                              << std::endl;
                    continue;
                }

                uint32_t guid = 0;

                std::cout << "\n#> Enter the Agent guid\n>>> ";
                std::cin >> guid;

                client.update_agent_name(new_name, guid);
            }
            else if (input.substr(0, 5) == "state")
            {
                char new_state = input.substr(6).at(0);
                if (new_state < '0' || new_state > '1')
                {
                    std::cout << "\n!> invalid state\n"
                              << std::endl;
                    continue;
                }

                uint32_t guid = 0;

                std::cout << "\n#> Enter the Agent guid\n>>> ";
                std::cin >> guid;

                client.update_agent_state(new_state == '0' ? false : true, guid);
            }
            else
                std::cout << "\n!> unrecognized command\ntype 'help' to see the commands list\n " << std::endl;
        }

        /* Terminate client */
        if (client.running())
            client.stop();
    }

    /* Ends Profile Session */
    H_PROFILE_END_SESSION();

    return 0;
}
