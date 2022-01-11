#include <bits/stdc++.h>
#include "json.h"

int
main()
{
    json::value root = json::object {
        { "hello", "world" },
        { "null", json::null },
        { "arr", json::array { 10, 20 } },
        { "e", 2.71828 } 
    };
    std::cout << json::to_string(root) << '\n';
}
