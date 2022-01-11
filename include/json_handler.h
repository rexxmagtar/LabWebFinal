#ifndef _LAB_FINAL_JSON_HANDLER_H_
#define _LAB_FINAL_JSON_HANDLER_H_

#include "json.h"
#include <bits/stdc++.h>

namespace json_handler 
{
#ifndef LAB_FINAL_SERVER_ROOT
    static const std::string server_root = "/var/www/json-docs";
#else
    static const std::string server_root = LAB_FINAL_SERVER_ROOT;
#endif
    static const std::string json_root = server_root + "/jsons";
    
    bool insert_new_doc(const std::string &json, long long &id);
    bool get_doc_if_exists(json::value &val, long long id);
    bool update_doc_if_exists(const std::string &json, long long id);
    bool delete_doc_if_exists(long long id);
    json::array get_by_predicate(std::function<bool (const json::value &)> pred);
} // namespace json_handler

#endif // _LAB_FINAL_JSON_HANDLER_H_
