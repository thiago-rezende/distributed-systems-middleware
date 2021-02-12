/**
 * @file main.cpp
 * @brief Application Entry Point
 *
 */

/* Args parser */
#include <clipp.h>

/* WebSocketpp stuff */
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

/* Server type shortcut */
typedef websocketpp::server<websocketpp::config::asio> server_t;

/* Handlers */
void on_open(std::shared_ptr<server_t> server, websocketpp::connection_hdl handle)
{
    H_PROFILE_FUNCTION();

    server_t::connection_ptr con = server->get_con_from_hdl(handle);
    std::string res = con->get_resource();

    std::regex channel_regex("(/\\w+)", std::regex_constants::ECMAScript);

    if (!std::regex_search(res, channel_regex))
    {
        con->close(websocketpp::close::status::internal_endpoint_error, "invalid channel string");
        return;
    }

    H_DEBUG("[CONNECTION] [OPEN] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
}

void on_close(std::shared_ptr<server_t> server, websocketpp::connection_hdl handle)
{
    H_PROFILE_FUNCTION();

    server_t::connection_ptr con = server->get_con_from_hdl(handle);
    std::string res = con->get_resource();

    std::regex channel_regex("(/\\w+)", std::regex_constants::ECMAScript);

    H_DEBUG("[CONNECTION] [CLOSE] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
}

void on_message(std::shared_ptr<server_t> server, websocketpp::connection_hdl handle, server_t::message_ptr message)
{
    H_PROFILE_FUNCTION();

    server_t::connection_ptr con = server->get_con_from_hdl(handle);
    std::string res = con->get_resource();

    std::regex channel_regex("(/\\w+)", std::regex_constants::ECMAScript);

    H_DEBUG("[MESSAGE] host => [{}] channel => [{}] message => [{}]", con->get_host(), res.substr(1), message->get_payload());
}

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

        /* Sync */
        std::atomic<bool> runing = true;

        std::shared_ptr<server_t> server = std::make_shared<server_t>();
        server->clear_access_channels(websocketpp::log::alevel::all);
        server->set_open_handler(std::bind(&on_open, server, std::placeholders::_1));
        server->set_close_handler(std::bind(&on_close, server, std::placeholders::_1));
        server->set_message_handler(std::bind(&on_message, server, std::placeholders::_1, std::placeholders::_2));

        H_DEBUG("[SERVER] Initializing");
        server->init_asio();
        server->listen(port);
        H_DEBUG("[SERVER] Listening on port {}", port);
        server->start_accept();
        H_DEBUG("[SERVER] Ready to accept connections");

        std::thread server_thread([&]() { server->run(); });
        H_DEBUG("[SERVER] Running");

        std::cin.get();

        server->stop();
        H_DEBUG("[SERVER] Terminating");

        server_thread.join();
        H_DEBUG("[SERVER] Stopped");
    }

    /* Ends Profile Session */
    H_PROFILE_END_SESSION();

    return 0;
}
