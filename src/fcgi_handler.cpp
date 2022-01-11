#include "fcgi_handler.h"
#include "json.h"
#include "json_handler.h"


bool
fcgi_handler::create_response()
{
    auto body_pos = environment().posts.find("body");
    if (body_pos == environment().posts.end())
        return bad_request_response();
    std::string &body = body_pos->second;
    long long id;
    bool ok = json_handler::insert_new_doc(body, id);

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
 
    out << "Content-Type: application/json\r\n\r\n"
        << json::to_string(val);
    return true;
}


bool
fcgi_handler::get_all_response()
{
    auto docs = json_handler::get_by_predicate(
        [](const json::value &val) { return true; }
    );

    out << "Content-Type: application/json\r\n\r\n"
        << json::to_string(docs);
    return true;
}


bool
fcgi_handler::id_response(long long id)
{
        return get_one_response(id);
   
}

bool
fcgi_handler::response()
{
    auto id = environment().gets.find("id");
        return id_response(std::stoll(id->second));

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

