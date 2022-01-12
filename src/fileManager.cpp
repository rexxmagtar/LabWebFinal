#include "fileManager.h"

#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <functional>

namespace fileManager
{

namespace
{

std::vector<std::string>
get_all_files_in_JSONs_dir()
{
    std::vector<std::string> result;
    DIR *dir_str = opendir(JSON_root.c_str());

    struct dirent *file_info;
    errno = 0;
    while (NULL != (file_info = readdir(dir_str))) {
        if (file_info->d_type == DT_REG)
            result.emplace_back(file_info->d_name);
        errno = 0;
    }

    closedir(dir_str);
    return result;
}

std::string
file_name_from_id(long long id)
{
    return JSON_root + "/" + std::to_string(id);
}

} 

bool
insert_new_doc(const std::string &JSON, long long &id)
{
    try {
        JSON::value val = JSON::parse(JSON);

     
        std::vector<std::string> files = get_all_files_in_JSONs_dir();
        id = 0;
        for (std::size_t i = 0; i < files.size(); ++i) {
            long long new_id = std::stoll(files[i]);
            if (id < new_id)
                id = new_id;
        }
        ++id;
        std::ofstream str(file_name_from_id(id));
        str << JSON;
    } catch (std::exception &ex) {
        return false;
    }

    return true;
}

bool
get_doc_if_exists(JSON::value &val, long long id)
{
    std::string file_name = file_name_from_id(id);
    
  
    int ret = access(file_name.c_str(), F_OK);
    if (0 != ret)
        return false;

    val = JSON::parse_file(file_name);
    return true;
}

JSON::array
get_by_predicate(std::function<bool (const JSON::value &)> pred)
{
    JSON::array arr;
    
    std::vector<std::string> files = get_all_files_in_JSONs_dir();
    for (const std::string &file: files) {
        std::string abs_name = JSON_root + "/" + file;
        JSON::value val = JSON::parse_file(abs_name);
        if (pred(val)) {
            arr.push_back(JSON::object {
                { "id", JSON::integer(std::stoll(file)) },
                { "document", val }
            } );
        }
    }
    return arr;
}

} 
