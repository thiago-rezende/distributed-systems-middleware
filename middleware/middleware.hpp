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

/* JSON parser */
#include <nlohmann/json.hpp>

/* Server type shortcut */
typedef websocketpp::server<websocketpp::config::asio> server_t;
typedef websocketpp::connection_hdl con_hdl_t;
typedef std::set<con_hdl_t, std::owner_less<con_hdl_t>> con_set_t;
/* Connection Metadata */
class con_metadata_t
{
public:
    typedef std::shared_ptr<con_metadata_t> ptr;

    con_metadata_t(std::string status, bool state, std::string name, uint32_t guid, con_hdl_t handle)
        : status(status), state(state), name(name), guid(guid), handle(handle)
    {
    }

    std::string status;
    bool state;
    std::string name;
    con_hdl_t handle;
    uint32_t guid;
};

typedef std::map<uint32_t, con_metadata_t::ptr> con_metadata_map_t;

class Middleware
{
public:
    Middleware();
    ~Middleware();

    /* Middleware Loop */
    void run(uint16_t port = 9002);
    void stop();

    /* Validation Handler */
    bool validate(con_hdl_t handle);

    /* Connection Open Handler */
    void on_open(con_hdl_t handle);

    /* Connection Close Handler */
    void on_close(con_hdl_t handle);

    /* Message Handler */
    void on_message(con_hdl_t handle, server_t::message_ptr message);

    /* Broadcast message to clients */
    void broadcast_to_clients(std::string message);

    /* Auth Message Handler */
    void on_client_auth(con_hdl_t handle, nlohmann::json payload);
    void on_agent_auth(con_hdl_t handle, nlohmann::json payload);

    /* Ready Message Handler */
    void on_client_ready(con_hdl_t handle, nlohmann::json payload);
    void on_agent_ready(con_hdl_t handle, nlohmann::json payload);

    /* Update Message Handler */
    void on_update_by_client(con_hdl_t handle, nlohmann::json payload);
    void on_update_by_agent(con_hdl_t handle, nlohmann::json payload);

    /* Client Message Handler */
    void handle_client_message(std::string message_type, con_hdl_t handle, nlohmann::json payload);

    /* Agent Message Handler */
    void handle_agent_message(std::string message_type, con_hdl_t handle, nlohmann::json payload);

private:
    /* Server Instance */
    server_t m_server;

    /* Conection ID Counter */
    uint32_t m_connection_id_counter;

    /* Connections Buffer */
    con_set_t m_connections;
    con_set_t m_clients;
    con_set_t m_agents;
    con_metadata_map_t m_clients_metadata;
    con_metadata_map_t m_agents_metadata;

    /* Connection GUID */
    uint32_t m_next_guid = 0;

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
bool Middleware::validate(con_hdl_t handle)
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
void Middleware::on_open(con_hdl_t handle)
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
void Middleware::on_close(con_hdl_t handle)
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
void Middleware::on_message(con_hdl_t handle, server_t::message_ptr message)
{
    H_PROFILE_FUNCTION();

    server_t::connection_ptr con = m_server.get_con_from_hdl(handle);
    std::string res = con->get_resource();

    // con_set_t::iterator con_it;
    // for (con_it = m_connections.begin(); con_it != m_connections.end(); ++con_it)
    //     m_server.send(*con_it, message);

    nlohmann::json payload;

    std::string message_type = "invalid";

    try
    {
        payload = nlohmann::json::parse(message->get_payload());
        message_type = payload.at("message_type").get<std::string>();
    }
    catch (const std::exception &e)
    {
        H_ERROR("[MESSAGE] [MISSING_MESSAGE_TYPE] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
    }

    if (res.substr(1) == "clients")
        handle_client_message(message_type, handle, payload);
    else if (res.substr(1) == "agents")
        handle_agent_message(message_type, handle, payload);

    // if (res.substr(1) == "clients")
    //     for (con_it = m_agents.begin(); con_it != m_agents.end(); ++con_it)
    //         m_server.send(*con_it, message);
    // else if (res.substr(1) == "agents")
    //     for (con_it = m_clients.begin(); con_it != m_clients.end(); ++con_it)
    //         m_server.send(*con_it, message);

    // H_DEBUG("[MESSAGE] host => [{}] channel => [{}] message => [{}]", con->get_host(), res.substr(1), message->get_payload());
}

void Middleware::broadcast_to_clients(std::string message)
{
    con_metadata_map_t::iterator con_it;
    for (con_it = m_clients_metadata.begin(); con_it != m_clients_metadata.end(); ++con_it)
    {
        if (con_it->second->status != "ready")
        {
            H_DEBUG("[BROADCAST] [SKIP] [{}] [{}]", con_it->second->name, con_it->second->status);
            continue;
        }

        // con_hdl_t handle = con_it->second->handle;
        H_DEBUG("[BROADCAST] [SENT] [{}] [{}]", con_it->second->name, con_it->second->status);
        m_server.send(con_it->second->handle, message, websocketpp::frame::opcode::text);
    }
}

void Middleware::on_client_auth(con_hdl_t handle, nlohmann::json payload)
{
    uint32_t guid = m_next_guid++;

    server_t::connection_ptr con = m_server.get_con_from_hdl(handle);
    std::string res = con->get_resource();

    con_metadata_t::ptr metadata(new con_metadata_t("open", false, "no_name", guid, handle));
    m_clients_metadata[guid] = metadata;
    H_DEBUG("[CLIENT] [AUTH] host => [{}] channel => [{}] => [{}]", con->get_host(), res.substr(1), nlohmann::json({{"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump());
    m_server.send(handle, nlohmann::json({{"message_type", "ready"}, {"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump(), websocketpp::frame::opcode::text);
}

void Middleware::on_agent_auth(con_hdl_t handle, nlohmann::json payload)
{
    uint32_t guid = m_next_guid++;

    server_t::connection_ptr con = m_server.get_con_from_hdl(handle);
    std::string res = con->get_resource();

    con_metadata_t::ptr metadata(new con_metadata_t("open", false, "no_name", guid, handle));
    m_agents_metadata[guid] = metadata;
    H_DEBUG("[AGENT] [AUTH] host => [{}] channel => [{}] => [{}]", con->get_host(), res.substr(1), nlohmann::json({{"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump());
    m_server.send(handle, nlohmann::json({{"message_type", "ready"}, {"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump(), websocketpp::frame::opcode::text);
}

void Middleware::on_client_ready(con_hdl_t handle, nlohmann::json payload)
{
    server_t::connection_ptr con = m_server.get_con_from_hdl(handle);
    std::string res = con->get_resource();

    uint32_t guid = 0;
    std::string name = "invalid";
    bool state = false;

    try
    {
        ;
        guid = payload.at("guid").get<uint32_t>();
        name = payload.at("name").get<std::string>();
        state = payload.at("state").get<bool>();
    }
    catch (const std::exception &e)
    {
        H_ERROR("[CLIENT] [READY] [MISSING_GUID] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
        return;
    }

    con_metadata_t::ptr metadata = m_clients_metadata[guid];

    if (!metadata || metadata->status != "open")
    {
        H_ERROR("[AGENT] [READY] [NOT_AUTHORIZED] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
        return;
    }

    metadata->status = "ready";
    metadata->state = state;
    metadata->name = name;
    m_clients_metadata[guid] = metadata;
    H_DEBUG("[CLIENT] [READY] host => [{}] channel => [{}] => [{}]", con->get_host(), res.substr(1), nlohmann::json({{"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump());
    m_server.send(handle, nlohmann::json({{"message_type", "ready"}, {"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump(), websocketpp::frame::opcode::text);

    /* Notify all clients */
    broadcast_to_clients(nlohmann::json({{"message_type", "new_client"}, {"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump());
}

void Middleware::on_agent_ready(con_hdl_t handle, nlohmann::json payload)
{
    server_t::connection_ptr con = m_server.get_con_from_hdl(handle);
    std::string res = con->get_resource();

    uint32_t guid = 0;
    std::string name = "invalid";
    bool state = false;

    try
    {
        guid = payload.at("guid").get<uint32_t>();
        name = payload.at("name").get<std::string>();
        state = payload.at("state").get<bool>();
    }
    catch (const std::exception &e)
    {
        H_ERROR("[AGENT] [READY] [MISSING_GUID|MISSING_NAME|MISSING_STATE] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
        return;
    }

    con_metadata_t::ptr metadata = m_agents_metadata[guid];

    if (!metadata)
    {
        H_ERROR("[AGENT] [READY] [NOT_AUTHORIZED] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
        return;
    }

    if (metadata->status != "open")
        return;

    metadata->status = "ready";
    metadata->state = state;
    metadata->name = name;
    m_clients_metadata[guid] = metadata;
    H_DEBUG("[AGENT] [READY] host => [{}] channel => [{}] => [{}]", con->get_host(), res.substr(1), nlohmann::json({{"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump());
    m_server.send(handle, nlohmann::json({{"message_type", "ready"}, {"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump(), websocketpp::frame::opcode::text);

    /* Notify all clients */
    broadcast_to_clients(nlohmann::json({{"message_type", "new_agent"}, {"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump());
}

void Middleware::on_update_by_client(con_hdl_t handle, nlohmann::json payload)
{
    server_t::connection_ptr con = m_server.get_con_from_hdl(handle);
    std::string res = con->get_resource();

    uint32_t guid = 0;
    std::string name = "invalid";
    std::string status = "open";
    bool state = false;

    try
    {
        ;
        guid = payload.at("guid").get<uint32_t>();
        name = payload.at("name").get<std::string>();
        status = payload.at("status").get<std::string>();
        state = payload.at("state").get<bool>();
    }
    catch (const std::exception &e)
    {
        H_ERROR("[CLIENT] [READY] [MISSING_GUID] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
        return;
    }

    con_metadata_t::ptr metadata = m_clients_metadata[guid];

    if (!metadata)
    {
        H_ERROR("[CLIENT] [UPDATE] [NOT_AUTHORIZED] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
        return;
    }

    metadata->status = status;
    metadata->state = state;
    metadata->name = name;
    m_clients_metadata[guid] = metadata;
    H_DEBUG("[CLIENT] [UPDATE] host => [{}] channel => [{}] => [{}]", con->get_host(), res.substr(1), nlohmann::json({{"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump());
    m_server.send(metadata->handle, nlohmann::json({{"message_type", "update_agent"}, {"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump(), websocketpp::frame::opcode::text);

    /* Notify all clients */
    // broadcast_to_clients(nlohmann::json({{"message_type", "new_client"}, {"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump());
}

void Middleware::on_update_by_agent(con_hdl_t handle, nlohmann::json payload)
{
    server_t::connection_ptr con = m_server.get_con_from_hdl(handle);
    std::string res = con->get_resource();

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
        H_ERROR("[AGENT] [UPDATE] [MISSING_STATE_AGENT_VALUES] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
        return;
    }

    con_metadata_t::ptr metadata = m_agents_metadata[guid];

    if (!metadata)
    {
        H_ERROR("[AGENT] [UPDATE] [NOT_AUTHORIZED] host => [{}] channel => [{}]", con->get_host(), res.substr(1));
        return;
    }

    metadata->status = status;
    metadata->state = state;
    metadata->name = name;
    m_clients_metadata[guid] = metadata;
    H_DEBUG("[AGENT] [UPDATE] host => [{}] channel => [{}] => [{}]", con->get_host(), res.substr(1), nlohmann::json({{"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump());
    // m_server.send(handle, nlohmann::json({{"message_type", "update_agent"}, {"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump(), websocketpp::frame::opcode::text);

    /* Notify all clients */
    broadcast_to_clients(nlohmann::json({{"message_type", "update_agent"}, {"status", metadata->status}, {"state", metadata->state}, {"name", metadata->name}, {"guid", guid}}).dump());
}

void Middleware::handle_client_message(std::string message_type, con_hdl_t handle, nlohmann::json payload)
{
    if (message_type == "auth")
        on_client_auth(handle, payload);
    else if (message_type == "ready")
        on_client_ready(handle, payload);
    else if (message_type == "update_agent")
        on_update_by_client(handle, payload);
}

void Middleware::handle_agent_message(std::string message_type, con_hdl_t handle, nlohmann::json payload)
{
    if (message_type == "auth")
        on_agent_auth(handle, payload);
    else if (message_type == "ready")
        on_agent_ready(handle, payload);
    else if (message_type == "update_agent")
        on_update_by_agent(handle, payload);
}
