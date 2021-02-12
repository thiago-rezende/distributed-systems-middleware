/**
 * @file main.cpp
 * @brief Application Entry Point
 *
 */

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

/* Server type shortcut */
typedef websocketpp::server<websocketpp::config::asio> server_t;

/* Message Handler */
void on_message(websocketpp::connection_hdl handle, server_t::message_ptr message)
{
    H_PROFILE_FUNCTION();
    std::cout << message->get_payload() << std::endl;
}

/* Application Entry Point */
int main()
{
    /* Starts Profile Session */
    H_PROFILE_BEGIN_SESSION("Application Profile", "profile_results.json");

    {
        /* Profiles the Main Function */
        H_PROFILE_SCOPE("Main Scope");

        /* Initialize Logger */
        Horus::Logger::init();

        /* Sync */
        std::atomic<bool> runing = true;

        server_t server;
        server.set_message_handler(&on_message);

        server.init_asio();
        server.listen(9002);
        server.start_accept();

        std::thread server_thread([&]() { server.run(); });

        std::cin.get();

        server.stop();

        server_thread.join();
    }

    /* Ends Profile Session */
    H_PROFILE_END_SESSION();

    return 0;
}
