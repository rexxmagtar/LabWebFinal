#include "json_handler.h"
#include "file_lock.h"
#include "system_error.h"
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

namespace json_handler
{

namespace
{

std::vector<std::string>
get_all_files_in_jsons_dir()
{
    std::vector<std::string> result;
    DIR *dir_str = opendir(json_root.c_str());
    if (!dir_str)
       throw system_error(errno);
    struct dirent *file_info;
    errno = 0;
    while (NULL != (file_info = readdir(dir_str))) {
        if (file_info->d_type == DT_REG)
            result.emplace_back(file_info->d_name);
        errno = 0;
    }
    if (0 != errno)
        throw system_error(errno);
    closedir(dir_str);
    return result;
}

std::string
file_name_from_id(long long id)
{
    return json_root + "/" + std::to_string(id);
}

} // namespace

bool
insert_new_doc(const std::string &json, long long &id)
{
    try {
        json::value val = json::parse(json);

        file_lock_guard flg(json_root, file_lock::lock_mode::write);
        std::vector<std::string> files = get_all_files_in_jsons_dir();
        id = 0;
        for (std::size_t i = 0; i < files.size(); ++i) {
            long long new_id = std::stoll(files[i]);
            if (id < new_id)
                id = new_id;
        }
        ++id;
        std::ofstream str(file_name_from_id(id));
        str << json;
    } catch (std::exception &ex) {
        return false;
    }

    return true;
}

bool
get_doc_if_exists(json::value &val, long long id)
{
    std::string file_name = file_name_from_id(id);
    
    file_lock_guard dflg(json_root, file_lock::lock_mode::read);
    int ret = access(file_name.c_str(), F_OK);
    if (0 != ret)
        return false;
    
    file_lock_guard fflg(file_name, file_lock::lock_mode::read);
    val = json::parse_file(file_name);
    return true;
}

bool
update_doc_if_exists(const std::string &json, long long id)
{
    std::string file_name = file_name_from_id(id);
    
    file_lock_guard dflg(json_root, file_lock::lock_mode::write);
    int ret = access(file_name.c_str(), F_OK);
    if (0 != ret)
        return false;
    json::value val = json::parse(json);
    std::ofstream str(
        file_name, std::ios_base::out | std::ios_base::trunc
    );
    str << json;
    return true;
}

bool
delete_doc_if_exists(long long id)
{
    std::string file_name = file_name_from_id(id);
    
    file_lock_guard dflg(json_root, file_lock::lock_mode::write);
    int ret = access(file_name.c_str(), F_OK);
    if (0 != ret)
        return false;
    ret = unlink(file_name.c_str());
    if (0 != ret)
        throw system_error(errno);
    return true;
}

json::array
get_by_predicate(std::function<bool (const json::value &)> pred)
{
    json::array arr;
    file_lock_guard dflg(json_root, file_lock::lock_mode::read);
    std::vector<std::string> files = get_all_files_in_jsons_dir();
    for (const std::string &file: files) {
        std::string abs_name = json_root + "/" + file;
        json::value val = json::parse_file(abs_name);
        if (pred(val)) {
            arr.push_back(json::object {
                { "id", json::integer(std::stoll(file)) },
                { "document", val }
            } );
        }
    }
    return arr;
}

} // namespace json_handler
