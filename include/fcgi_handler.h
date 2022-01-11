#ifndef _LAB_FINAL_FCGI_HANDLER_H_
#define _LAB_FINAL_FCGI_HANDLER_H_

#include <fastcgi++/request.hpp>
#include <bits/stdc++.h>

class fcgi_handler: public Fastcgipp::Request<char>
{
private:
#ifndef LAB_FINAL_POST_LENGTH
    static const std::size_t post_length = 
        static_cast<std::size_t>(-1);
#else
    static const std::size_t post_length = 
        static_cast<std::size_t>(LAB_FINAL_POST_LENGTH);
#endif

    bool no_content_response();
    bool bad_request_response();
    bool not_found_response();
    bool not_allowed_response(
        const std::vector<Fastcgipp::Http::RequestMethod> &methods
    );
    bool internal_server_error_response(const std::string &msg);

    bool create_response();
    bool get_one_response(long long id);
    bool update_response(long long id);
    bool delete_response(long long id);
    bool get_all_response();
    bool get_k_response(const std::string &key);
    bool get_kv_response(const std::string &key, const std::string &value);
    bool id_response(long long id);
    bool no_id_response();
    bool response();
    bool inProcessor();

public:
    fcgi_handler()
        : Fastcgipp::Request<char>(post_length) {}
};

#endif // _LAB_FINAL_FCGI_HANDLER_H_
