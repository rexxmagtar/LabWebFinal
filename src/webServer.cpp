#include "webServer.h"
#include "json.h"
#include "json_handler.h"


bool
webServer::create_response()
{
    auto body_pos = environment().posts.find("body");

    std::string &body = body_pos->second;
    long long id;
    json_handler::insert_new_doc(body, id);

    out << "File created";
    return true;
}

bool
webServer::get_one_response(long long id)
{
    json::value val;
    bool exists = json_handler::get_doc_if_exists(val, id);

    out  << json::to_string(val);
    return true;
}


bool
webServer::+get_all_response()
{
    auto docs = json_handler::get_by_predicate(
        [](const json::value &val) { return true; }
    );
	
    out << "Content-Type: application/json\r\n\r\n"
        << json::to_string(docs);
    return true;
}

bool
webServer::id_response(long long id)
{
        return get_one_response(id);
}

bool 
webServer::no_id_response()
{
    auto &gets = environment().gets;
    switch (environment().requestMethod) {
    case Fastcgipp::Http::RequestMethod::GET: {
                return get_all_response();
        }
    case Fastcgipp::Http::RequestMethod::POST:
        return create_response();
    }
}

bool
webServer::response()
{
    auto id = environment().gets.find("id");
    if (id != environment().gets.end())
        return id_response(std::stoll(id->second));
    else
        return no_id_response(); 
}

bool
webServer::inProcessor()
{
    
    std::string body_contents(
        environment().postBuffer().begin(),
        environment().postBuffer().end()
    );
    environment().posts.insert({ std::string("body"), body_contents });
    return true;
}

