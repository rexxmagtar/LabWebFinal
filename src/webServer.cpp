#include "webServer.h"
#include "json.h"
#include "fileManager.h"


bool
webServer::addFile_request()
{
    auto body_pos = environment().posts.find("body");

    std::string &body = body_pos->second;
    long long id;
    fileManager::insert_new_doc(body, id);

    out << "File created";
    return true;
}

bool
webServer::get_by_id_request(long long id)
{
    json::value val;
    bool exists = fileManager::get_doc_if_exists(val, id);

    out  << json::to_string(val);
    return true;
}


bool
webServer::get_all_files_request()
{
    auto docs = fileManager::get_by_predicate(
        [](const json::value &val) { return true; }
    );
	
    out << "Content-Type: application/json\r\n\r\n"
        << json::to_string(docs);
    return true;
}

bool
webServer::id_response(long long id)
{
        return get_by_id_request(id);
}

bool 
webServer::no_id_response()
{
    auto &gets = environment().gets;
    switch (environment().requestMethod) {
    case Fastcgipp::Http::RequestMethod::GET: {
                return get_all_files_request();
        }
    case Fastcgipp::Http::RequestMethod::POST:
        return addFile_request();
    }
}

bool
webServer::response()
{
    auto id = environment().gets.find("id");
    if (id != environment().gets.end())
        return  get_by_id_request(std::stoll(id->second));
    else
        {
        auto& gets = environment().gets;
        switch (environment().requestMethod) {
        case Fastcgipp::Http::RequestMethod::GET: {
            return get_all_files_request();
        }
        case Fastcgipp::Http::RequestMethod::POST:
            return addFile_request();
        }
    }
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

