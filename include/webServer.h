#ifndef _LAB_FINAL_webServer_H_
#define _LAB_FINAL_webServer_H_

#include <fastcgi++/request.hpp>
#include <bits/stdc++.h>

class webServer: public Fastcgipp::Request<char>
{
private:

    bool get_by_id_request(long long id);
    bool get_all_files_request();
    bool addFile_request();
    bool response();
    bool inProcessor();

    static const int post_length =
       -1;

public:
    webServer()
        : Fastcgipp::Request<char>(post_length) {}
};

#endif 
