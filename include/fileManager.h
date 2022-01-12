

#include "json.h"
#include <bits/stdc++.h>

namespace fileManager 
{
    static const std::string server_root = "/var/www/json-docs";

    static const std::string json_root = server_root + "/jsons";
    
    bool insert_new_doc(const std::string &json, long long &id);
    bool get_doc_if_exists(json::value &val, long long id);
     json::array get_by_predicate(std::function<bool (const json::value &)> pred);
} 
 
