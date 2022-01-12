#ifndef _LAB_FINAL_fileManager_H_
#define _LAB_FINAL_fileManager_H_

#include "json.h"
#include <bits/stdc++.h>

namespace fileManager 
{
#ifndef LAB_FINAL_SERVER_ROOT
    static const std::string server_root = "/var/www/json-docs";
#else
    static const std::string server_root = LAB_FINAL_SERVER_ROOT;
#endif
    static const std::string json_root = server_root + "/jsons";
    
    bool insert_new_doc(const std::string &json, long long &id);
    bool get_doc_if_exists(json::value &val, long long id);
     json::array get_by_predicate(std::function<bool (const json::value &)> pred);
} 
#endif 
