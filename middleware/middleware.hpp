/**
 * @file middleware.hpp
 * @author Thiago Rezende (thiago-rezende.github.io)
 * @brief Middleware Singleton
 *
 */

#pragma once

/* WebSocketpp stuff */
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

/* Server type shortcut */
typedef websocketpp::server<websocketpp::config::asio> server_t;
typedef std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> con_set_t;

class Middleware
{
public:
    Middleware();
    ~Middleware();

    /* Middleware Loop */
    void run(uint16_t port = 9002);
    void stop();

    /* Validation Handler */
    bool validate(websocketpp::connection_hdl handle);

    /* Connection Open Handler */
    void on_open(websocketpp::connection_hdl handle);

    /* Connection Close Handler */
    void on_close(websocketpp::connection_hdl handle);

    /* Message Handler */
    void on_message(websocketpp::connection_hdl handle, server_t::message_ptr message);

private:
    /* Server Instance */
    server_t m_server;

    /* Conection ID Counter */
    uint32_t m_connection_id_counter;

    /* Connections Buffer */
    con_set_t m_connections;
    con_set_t m_clients;
    con_set_t m_agents;

    /* Server Port */
    uint16_t m_port = 9002;

    /* Server Thread */
    std::thread m_server_thread;
};

Middleware::Middleware()
{
    H_PROFILE_FUNCTION();

    /* Asio Initialization */
    H_DEBUG("[SERVER] Initializing");
    m_server.init_asio();

    /* CLeaner Log */
    m_server.clear_access_channels(websocketpp::log::alevel::all);
    m_server.clear_error_channels(websocketpp::log::elevel::all);
    m_server.set_error_channels(websocketpp::log::elevel::fatal);

    /* Handlers Binding */
    m_server.set_validate_handler(std::bind(&Middleware::validate, this, std::placeholders::_1));
    m_server.set_open_handler(std::bind(&Middleware::on_open, this, std::placeholders::_1));
    m_server.set_close_handler(std::bind(&Middleware::on_close, this, std::placeholders::_1));
    m_server.set_message_handler(std::bind(&Middleware::on_message, this, std::placeholders::_1, std::placeholders::_2));
}

Middleware::~Middleware()
{
    H_PROFILE_FUNCTION();
}

/* Middleware Run */
void Middleware::run(uint16_t port)
{
    H_PROFILE_FUNCTION();

    m_port = port;

    /* Socket Setup */
    m_server.listen(m_port);
    H_DEBUG("[SERVER] Listening on port {}", m_port);

    /* Accept Connections */
    m_server.start_accept();
    H_DEBUG("[SERVER] Ready to accept connections");

    /* Start Middleware Thread */
    m_server_thread = std::thread([&]() { m_server.run(); });

    H_DEBUG("[SERVER] Running");
}

/* Middleware Stop */
void Middleware::stop()
{
    H_PROFILE_FUNCTION();

    H_DEBUG("[SERVER] Terminating");
    m_server.stop_listening();

    con_set_t::iterator con_it;
    for (con_it = m_connections.begin(); con_it != m_connections.end(); ++con_it)
    {
        m_server.pause_reading(*con_it);
        m_server.close(*con_it, websocketpp::close::status::normal, "server closed");
    }

    m_server_thread.join();
    H_DEBUG("[SERVER] Stopped");
}

/* Validation Handler */
bool Middleware::validate(websocketpp::connection_hdl handle)
{
    H_PROFILE_FUNCTION();

    server_t::connection_ptr con = m_server.get_con_from_hdl(handle);

    std::string res = con->get_resource();
    std::regex channel_regex("(/agents|/clients)", std::regex_constants::ECMAScript);

    if (!std::regex_search(res, channel_regex))
    {
        con->set_status(websocketpp::http::status_code::not_acceptable, "invalid channel");
        H_DEBUG("[HANDSHAKE] [REJECT] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
        return false;
    }

    H_DEBUG("[HANDSHAKE] [ACCEPT] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
    return true;
}

/* Connection Open Handler */
void Middleware::on_open(websocketpp::connection_hdl handle)
{
    H_PROFILE_FUNCTION();

    server_t::connection_ptr con = m_server.get_con_from_hdl(handle);
    std::string res = con->get_resource();

    m_connections.insert(handle);

    if (res.substr(1) == "clients")
        m_clients.insert(handle);
    else if (res.substr(1) == "agents")
        m_agents.insert(handle);

    H_DEBUG("[CONNECTION] [OPEN] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
}

/* Connection Close Handler */
void Middleware::on_close(websocketpp::connection_hdl handle)
{
    H_PROFILE_FUNCTION();

    server_t::connection_ptr con = m_server.get_con_from_hdl(handle);
    std::string res = con->get_resource();

    m_connections.erase(handle);

    if (res.substr(1) == "clients")
        m_clients.erase(handle);
    else if (res.substr(1) == "agents")
        m_agents.erase(handle);

    H_DEBUG("[CONNECTION] [CLOSE] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
}

/* Message Handler */
void Middleware::on_message(websocketpp::connection_hdl handle, server_t::message_ptr message)
{
    H_PROFILE_FUNCTION();

    server_t::connection_ptr con = m_server.get_con_from_hdl(handle);
    std::string res = con->get_resource();

    con_set_t::iterator con_it;
    // for (con_it = m_connections.begin(); con_it != m_connections.end(); ++con_it)
    //     m_server.send(*con_it, message);

    if (res.substr(1) == "clients")
        for (con_it = m_agents.begin(); con_it != m_agents.end(); ++con_it)
            m_server.send(*con_it, message);
    else if (res.substr(1) == "agents")
        for (con_it = m_clients.begin(); con_it != m_clients.end(); ++con_it)
            m_server.send(*con_it, message);

    H_DEBUG("[MESSAGE] host => [{}] channel => [{}] message => [{}]", con->get_host(), res.substr(1), message->get_payload());
}
