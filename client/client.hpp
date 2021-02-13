/**
 * @file middleware.hpp
 * @author Thiago Rezende (thiago-rezende.github.io)
 * @brief Client Singleton
 *
 */

#pragma once

/* WebSocketpp stuff */
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>

/* JSON parser */
#include <nlohmann/json.hpp>

/* FMT */
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/format.h>

/* Server type shortcut */
typedef websocketpp::client<websocketpp::config::asio> client_t;
typedef websocketpp::connection_hdl con_hdl_t;

class Client
{
public:
    Client();
    ~Client();

    /* Client Loop */
    void run(std::string host = "127.0.0.1", uint16_t port = 9002, std::string name = "client");
    void stop();

    /* Connection Open Handler */
    void on_open(con_hdl_t handle);

    /* Connection Close Handler */
    void on_close(con_hdl_t handle);

    /* Message Handler */
    void on_message(con_hdl_t handle, client_t::message_ptr message);

    /* Auth Message Handler */
    void on_auth(con_hdl_t handle, nlohmann::json payload);

    /* Ready Message Handler */
    void on_ready(con_hdl_t handle, nlohmann::json payload);

    /* Update Message Handler */
    void on_update(con_hdl_t handle, nlohmann::json payload);

    /* Update New Agent Handler */
    void on_new_agent(con_hdl_t handle, nlohmann::json payload);

    /* Update New Client Handler */
    void on_new_client(con_hdl_t handle, nlohmann::json payload);

    /* Update Update Agent Handler */
    void on_update_agent(con_hdl_t handle, nlohmann::json payload);

    /* Update Name Handler */
    void update_name(std::string name);

    /* Update state Handler */
    void update_state(bool state);

    /* Update Agent Name Handler */
    void update_agent_name(std::string name, uint32_t guid);

    /* Update Agetn State Handler */
    void update_agent_state(bool state, uint32_t guid);

    /* Get Runnig State */
    bool running() { return m_running; }

private:
    /* Client Instance */
    client_t m_client;

    /* Connection Metadata */
    bool m_state;
    std::string m_status;
    std::string m_name;
    con_hdl_t m_handle;
    uint32_t m_guid;

    /* Server Port */
    std::string m_host = "127.0.0.1";
    uint16_t m_port = 9002;

    /* Server Thread */
    std::thread m_client_thread;

    /* Status Utility */
    bool m_running = false;
};

Client::Client()
{
    H_PROFILE_FUNCTION();

    /* Asio Initialization */
    H_DEBUG("[CLIENT] Initializing");
    m_client.init_asio();

    /* CLeaner Log */
    m_client.clear_access_channels(websocketpp::log::alevel::all);
    m_client.clear_error_channels(websocketpp::log::elevel::all);
    m_client.set_error_channels(websocketpp::log::elevel::fatal);

    /* Handlers Binding */
    m_client.set_open_handler(std::bind(&Client::on_open, this, std::placeholders::_1));
    m_client.set_message_handler(std::bind(&Client::on_message, this, std::placeholders::_1, std::placeholders::_2));
}

Client::~Client()
{
    H_PROFILE_FUNCTION();
}

/* Client Run */
void Client::run(std::string host, uint16_t port, std::string name)
{
    H_PROFILE_FUNCTION();

    m_host = host;
    m_port = port;
    m_name = name;
    std::string uri = fmt::format("ws://{}:{}/clients", m_host, m_port);

    H_DEBUG("[CLIENT] Server uri [{}]", uri);

    /* Socket Setup */
    websocketpp::lib::error_code ec;
    client_t::connection_ptr con = m_client.get_connection(uri, ec);

    if (ec)
    {
        H_ERROR("[CLIENT] [CONNECTION] {}", ec.message());
        exit(1);
    }

    /* Store Connection Handle */
    m_handle = con->get_handle();

    /* Connect to Server */
    m_client.connect(con);
    H_DEBUG("[CLIENT] Connected");

    /* Start Client Thread */
    m_client_thread = std::thread([&]() { m_client.run(); });
    H_DEBUG("[CLIENT] Running");

    m_running = true;
}

/* Client Stop */
void Client::stop()
{
    H_PROFILE_FUNCTION();

    H_DEBUG("[CLIENT] Terminating");
    websocketpp::lib::error_code ec;

    m_client.close(m_handle, websocketpp::close::status::going_away, "going away", ec);

    if (ec)
    {
        H_ERROR("[CLIENT] [TERMINATION] {}", ec.message());
    }

    H_DEBUG("[CLIENT] [CONNECTION] [CLOSE] host => [{}:{}] channel => [clients]", m_host, m_port);

    m_client_thread.join();
    H_DEBUG("[CLIENT] Stopped");
    m_running = false;
}

/* Connection Open Handler */
void Client::on_open(con_hdl_t handle)
{
    H_PROFILE_FUNCTION();

    m_client.send(handle, nlohmann::json({{"message_type", "auth"}}).dump(), websocketpp::frame::opcode::text);

    H_DEBUG("[CLIENT] [CONNECTION] [OPEN] host => [{}:{}] channel => [clients]", m_host, m_port);
}

/* Message Handler */
void Client::on_message(con_hdl_t handle, client_t::message_ptr message)
{
    H_PROFILE_FUNCTION();

    nlohmann::json payload;

    std::string message_type = "invalid";

    try
    {
        payload = nlohmann::json::parse(message->get_payload());
        message_type = payload.at("message_type").get<std::string>();
    }
    catch (const std::exception &e)
    {
        H_ERROR("[MESSAGE] [MISSING_MESSAGE_TYPE] host => [{}:{}] channel => [clients]", m_host, m_port);
    }

    if (message_type == "ready")
        on_ready(handle, payload);
    else if (message_type == "new_client")
        on_new_client(handle, payload);
    else if (message_type == "new_agent")
        on_new_agent(handle, payload);
    else if (message_type == "update_agent")
        on_update_agent(handle, payload);

    // H_DEBUG("[CLIENT] [MESSAGE] host => [{}:{}] channel => [clients] message => [{}]", m_host, m_port, message->get_payload());
}

void Client::on_auth(con_hdl_t handle, nlohmann::json payload)
{
    H_DEBUG("[CLIENT] [AUTH] host => [{}:{}] channel => [clients] => [{}]", m_host, m_port, nlohmann::json({{"status", m_status}, {"state", m_state}, {"name", m_name}, {"guid", m_guid}}).dump());
}

void Client::on_ready(con_hdl_t handle, nlohmann::json payload)
{
    uint32_t guid = 0;
    std::string status = "invalid";
    std::string name = "invalid";
    bool state = false;

    try
    {
        guid = payload.at("guid").get<uint32_t>();
        name = payload.at("name").get<std::string>();
        status = payload.at("status").get<std::string>();
        state = payload.at("state").get<bool>();
    }
    catch (const std::exception &e)
    {
        H_ERROR("[CLIENT] [READY] [MISSING_GUID] host => [{}:{}] channel => [clients]", m_host, m_port);
        return;
    }

    m_status = status;
    m_state = state;
    m_guid = guid;

    H_DEBUG("[CLIENT] [READY] host => [{}:{}] channel => [clients] => [{}]", m_host, m_port, nlohmann::json({{"status", m_status}, {"state", m_state}, {"name", m_name}, {"guid", m_guid}}).dump());

    /* Send ready back to server */
    m_client.send(handle, nlohmann::json({{"message_type", "ready"}, {"status", m_status}, {"state", m_state}, {"name", m_name}, {"guid", m_guid}}).dump(), websocketpp::frame::opcode::text);
}

void Client::on_update(con_hdl_t handle, nlohmann::json payload)
{
    uint32_t guid = 0;
    std::string name = "invalid";
    std::string status = "open";
    bool state = false;

    try
    {
        guid = payload.at("guid").get<uint32_t>();
        name = payload.at("name").get<std::string>();
        status = payload.at("status").get<std::string>();
        state = payload.at("state").get<bool>();
    }
    catch (const std::exception &e)
    {
        H_ERROR("[CLIENT] [UPDATE] [MISSING_CLIENT_DATA] host => [{}:{}] channel => [clients]", m_host, m_port);
        return;
    }

    if (m_status != "ready")
    {
        H_ERROR("[CLIENT] [UPDATE] [NOT_AUTHORIZED] host => [{}:{}] channel => [clients]", m_host, m_port);
        return;
    }

    m_status = status;
    m_state = state;
    m_name = name;
    H_DEBUG("[CLIENT] [UPDATE] host => [{}:{}] channel => [clients] client_data => [{}]", m_host, m_port, nlohmann::json({{"status", m_status}, {"state", m_state}, {"name", m_name}, {"guid", guid}}).dump());
    m_client.send(m_handle, nlohmann::json({{"message_type", "update_client"}, {"status", m_status}, {"state", m_state}, {"name", m_name}, {"guid", guid}}).dump(), websocketpp::frame::opcode::text);
}

void Client::on_new_agent(con_hdl_t handle, nlohmann::json payload)
{
    uint32_t guid = 0;
    std::string name = "invalid";
    std::string status = "open";
    bool state = false;

    try
    {
        guid = payload.at("guid").get<uint32_t>();
        name = payload.at("name").get<std::string>();
        status = payload.at("status").get<std::string>();
        state = payload.at("state").get<bool>();
    }
    catch (const std::exception &e)
    {
        H_ERROR("[CLIENT] [NEW_AGENT] [MISSING_AGENT_DATA] host => [{}:{}] channel => [clients]", m_host, m_port);
        return;
    }

    H_DEBUG("[CLIENT] [NEW_AGENT] host => [{}:{}] channel => [clients] client_data => [{}]", m_host, m_port, nlohmann::json({{"status", status}, {"state", state}, {"name", name}, {"guid", guid}}).dump());
}

void Client::on_new_client(con_hdl_t handle, nlohmann::json payload)
{
    uint32_t guid = 0;
    std::string name = "invalid";
    std::string status = "open";
    bool state = false;

    try
    {
        guid = payload.at("guid").get<uint32_t>();
        name = payload.at("name").get<std::string>();
        status = payload.at("status").get<std::string>();
        state = payload.at("state").get<bool>();
    }
    catch (const std::exception &e)
    {
        H_ERROR("[CLIENT] [NEW_CLIENT] [MISSING_CLIENT_DATA] host => [{}:{}] channel => [clients]", m_host, m_port);
        return;
    }

    H_DEBUG("[CLIENT] [NEW_CLIENT] host => [{}:{}] channel => [clients] client_data => [{}]", m_host, m_port, nlohmann::json({{"status", status}, {"state", state}, {"name", name}, {"guid", guid}}).dump());
}

void Client::on_update_agent(con_hdl_t handle, nlohmann::json payload)
{
    uint32_t guid = 0;
    std::string name = "invalid";
    std::string status = "open";
    bool state = false;

    try
    {
        guid = payload.at("guid").get<uint32_t>();
        name = payload.at("name").get<std::string>();
        status = payload.at("status").get<std::string>();
        state = payload.at("state").get<bool>();
    }
    catch (const std::exception &e)
    {
        H_ERROR("[CLIENT] [UPDATE_AGENT] [MISSING_AGENT_DATA] host => [{}:{}] channel => [clients]", m_host, m_port);
        return;
    }

    H_DEBUG("[CLIENT] [UPDATE_AGENT] host => [{}:{}] channel => [clients] client_data => [{}]", m_host, m_port, nlohmann::json({{"status", status}, {"state", state}, {"name", name}, {"guid", guid}}).dump());
}

void Client::update_name(std::string name)
{
    on_update(m_handle, nlohmann::json({{"message_type", "update_client"}, {"status", m_status}, {"state", m_state}, {"name", name}, {"guid", m_guid}}));
}

void Client::update_state(bool state)
{
    on_update(m_handle, nlohmann::json({{"message_type", "update_client"}, {"status", m_status}, {"state", state}, {"name", m_name}, {"guid", m_guid}}));
}

void Client::update_agent_name(std::string name, uint32_t guid)
{
    m_client.send(m_handle, nlohmann::json({{"message_type", "update_agent_name"}, {"name", name}, {"guid", guid}}).dump(), websocketpp::frame::opcode::text);
}

void Client::update_agent_state(bool state, uint32_t guid)
{
    m_client.send(m_handle, nlohmann::json({{"message_type", "update_agent_state"}, {"state", state}, {"guid", guid}}).dump(), websocketpp::frame::opcode::text);
}
