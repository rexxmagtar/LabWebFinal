#ifndef _LAB_FINAL_fileManager_H_
#define _LAB_FINAL_fileManager_H_

#include "JSON.h"
#include <bits/stdc++.h>

namespace fileManager 
{
#ifndef LAB_FINAL_SERVER_ROOT
    static const std::string server_root = "/var/www/JSON-docs";
#else
    static const std::string server_root = LAB_FINAL_SERVER_ROOT;
#endif
    static const std::string JSON_root = server_root + "/JSONs";
    
    bool insert_new_doc(const std::string &JSON, long long &id);
    bool get_doc_if_exists(JSON::value &val, long long id);
     JSON::array get_by_predicate(std::function<bool (const JSON::value &)> pred);
} 
#endif 
