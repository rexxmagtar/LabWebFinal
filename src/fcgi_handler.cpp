#include "fcgi_handler.h"
#include "json.h"
#include "json_handler.h"

bool
fcgi_handler::no_content_response()
{
    out << "Status: 204 No Content\r\n\r\n";
    return true;
}

bool
fcgi_handler::bad_request_response()
{
    out << "Status: 400 Bad Request\r\n\r\n";
    return true;
}

bool
fcgi_handler::not_found_response()
{
    out << "Status: 404 Not Found\r\n"
           "Content-Type: text/html\r\n\r\n"
           "<html>\r\n<body>\r\n"
           "<h1>404 Not Found</h1>\r\n"
           "The JSON document you requested can't be found.\r\n"
           "</body>\r\n</html>";
    return true;
}

bool 
fcgi_handler::not_allowed_response(
    const std::vector<Fastcgipp::Http::RequestMethod> &methods
)
{
    out << "Status: 405 Method Not Allowed\r\n"
           "Allow: ";
    for (std::size_t i = 0; i < methods.size(); ++i) {
        out << methods[i];
        if (i != methods.size() - 1)
            out << ", ";
    }
    out << "\r\n\r\n";
    return true;
}

bool
fcgi_handler::internal_server_error_response(const std::string &msg)
{
    out << "Status: 500 Internal Server Error\r\n"
           "Content-Type: text/html\r\n\r\n"
           "<html>\r\n<body>\r\n"
           "<h1>500 Internal Server Error</h1>\r\n"
           "Can't respond due to the following reason:\r\n"
        << msg << "\r\n"
        << "</body>\r\n</html>";
    return true;
}

bool
fcgi_handler::create_response()
{
    auto body_pos = environment().posts.find("body");
    if (body_pos == environment().posts.end())
        return bad_request_response();
    std::string &body = body_pos->second;
    long long id;
    bool ok = json_handler::insert_new_doc(body, id);
    if (!ok)
        return internal_server_error_response(
            "The document can't be inserted right now. Try again later."
        );
    out << "Status: 201 Created\r\n"
           "Content-Type: text/plain\r\n\r\n"
           "Document-Id=" << id;
    return true;
}

bool
fcgi_handler::get_one_response(long long id)
{
    json::value val;
    bool exists = json_handler::get_doc_if_exists(val, id);
    if (!exists)
        return not_found_response();
    out << "Content-Type: application/json\r\n\r\n"
        << json::to_string(val);
    return true;
}

bool
fcgi_handler::update_response(long long id)
{
    auto body_pos = environment().posts.find("body");
    if (body_pos == environment().posts.end())
        return bad_request_response();
    std::string &body = body_pos->second;
    bool exists = json_handler::update_doc_if_exists(body, id);
    if (!exists)
        return not_found_response();
    out << "\r\n\r\n";
    return true;
}

bool
fcgi_handler::delete_response(long long id)
{
    bool exists = json_handler::delete_doc_if_exists(id);
    if (!exists)
        return not_found_response();
    out << "\r\n\r\n";
    return true;
}

bool
fcgi_handler::get_all_response()
{
    auto docs = json_handler::get_by_predicate(
        [](const json::value &val) { return true; }
    );
    if (docs.empty())
        return no_content_response();
    out << "Content-Type: application/json\r\n\r\n"
        << json::to_string(docs);
    return true;
}

bool
fcgi_handler::get_k_response(const std::string &key)
{
    auto docs = json_handler::get_by_predicate(
        [&](const json::value &val) { 
            if (!json::inside<json::object>(val))
                return false;
            const json::object &obj = json::get<json::object>(val);
            return obj.find(key) != obj.end();
        }
    );
    if (docs.empty())
        return no_content_response();
    out << "Content-Type: application/json\r\n\r\n"
        << json::to_string(docs);
    return true;
}

bool
fcgi_handler::get_kv_response(const std::string &key, const std::string &value)
{
    json::value json_value;
    try {
        json_value = json::parse(value);
    } catch (json::parse_exception &ex) {
        json_value = json::string(value);
    }
    auto docs = json_handler::get_by_predicate(
        [&](const json::value &val) { 
            if (!json::inside<json::object>(val))
                return false;
            const json::object &obj = json::get<json::object>(val);
            auto key_pos = obj.find(key);
            return key_pos != obj.end() && key_pos->second == json_value;
        }
    );
    if (docs.empty())
        return no_content_response();
    out << "Content-Type: application/json\r\n\r\n"
        << json::to_string(docs);
    return true;
}

bool
fcgi_handler::id_response(long long id)
{
    switch (environment().requestMethod) {
    case Fastcgipp::Http::RequestMethod::GET:
        return get_one_response(id);
    case Fastcgipp::Http::RequestMethod::DELETE:
        return delete_response(id);
    case Fastcgipp::Http::RequestMethod::PUT:
        return update_response(id);
    default:
        return not_allowed_response( {
            Fastcgipp::Http::RequestMethod::GET,
            Fastcgipp::Http::RequestMethod::DELETE,
            Fastcgipp::Http::RequestMethod::PUT
        } );
    }
}

bool 
fcgi_handler::no_id_response()
{
    auto &gets = environment().gets;
    switch (environment().requestMethod) {
    case Fastcgipp::Http::RequestMethod::GET: {
            auto key_pos = gets.find("key");
            auto value_pos = gets.find("value");
            if (key_pos != gets.end() && value_pos != gets.end())
                return get_kv_response(key_pos->second, value_pos->second);
            else if (key_pos != gets.end())
                return get_k_response(key_pos->second);
            else 
                return get_all_response();
        }
    case Fastcgipp::Http::RequestMethod::POST:
        return create_response();
    default:
        return not_allowed_response( {
            Fastcgipp::Http::RequestMethod::GET,
            Fastcgipp::Http::RequestMethod::POST
        } );
    }
}

bool
fcgi_handler::response()
{
    auto id = environment().gets.find("id");
    if (id != environment().gets.end())
        return id_response(std::stoll(id->second));
    else
        return no_id_response(); 
}

bool
fcgi_handler::inProcessor()
{
    static const std::string textPlain = "text/plain";
    static const std::string appJson = "application/json";
    if (environment().contentType != textPlain &&
        environment().contentType != appJson) {
        return false;
    }
    
    std::string body_contents(
        environment().postBuffer().begin(),
        environment().postBuffer().end()
    );
    environment().posts.insert({ std::string("body"), body_contents });
    return true;
}

