#include "webServer.h"
#include "JSON.h"
#include "fileManager.h"


bool
webServer::create_response()
{
    auto body_pos = environment().posts.find("body");

    std::string &body = body_pos->second;
    long long id;
    fileManager::insert_new_doc(body, id);

    out << "File created";
    return true;
}

bool
webServer::get_one_response(long long id)
{
    JSON::value val;
    bool exists = fileManager::get_doc_if_exists(val, id);

    out  << JSON::to_string(val);
    return true;
}


bool
webServer::get_all_response()
{
    auto docs = fileManager::get_by_predicate(
        [](const JSON::value &val) { return true; }
    );
	
    out << "Content-Type: application/JSON\r\n\r\n"
        << JSON::to_string(docs);
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

