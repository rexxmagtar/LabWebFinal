#ifndef _LAB_FINAL_webServer_H_
#define _LAB_FINAL_webServer_H_

#include <fastcgi++/request.hpp>
#include <bits/stdc++.h>

class webServer: public Fastcgipp::Request<char>
{
private:

    static const std::size_t post_length = 
        static_cast<std::size_t>(-1);

    bool create_response();
    bool get_one_response(long long id);
    bool get_all_response();
    bool id_response(long long id);
    bool no_id_response();
    bool response();
    bool inProcessor();

public:
    webServer()
        : Fastcgipp::Request<char>(post_length) {}
};

#endif 
