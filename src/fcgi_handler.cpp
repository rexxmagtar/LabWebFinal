#include "fcgi_handler.h"

#include <string>

#include "json.h"
#include "json_handler.h"


bool
fcgi_handler::create_response()
{
    auto body_pos = environment().posts.find("body");
    if (body_pos == environment().posts.end())
        return false;
    std::string &body = body_pos->second;
    long long id;
    bool ok = json_handler::insert_new_doc(body, id);
    return true;
}

bool
fcgi_handler::get_one_response(long long id)
{
    json::value val;
    bool exists = json_handler::get_doc_if_exists(val, id);
    return true;
}

bool
fcgi_handler::get_all_response()
{
    auto docs = json_handler::get_by_predicate(
        [](const json::value &val) { return true; }
    );
    return true;
}

bool
fcgi_handler::id_response(long long id)
{
        return get_one_response(id);
    
}


