#include <gtest/gtest.h>
#include <bits/stdc++.h>
#include "json.h"

TEST(json_test, empty_object)
{
    static const std::string empty_obj_str = "{}";

    json::value val = json::object {};
    EXPECT_EQ(json::to_string(val), empty_obj_str);
}

TEST(json_test, obj_init_list)
{
    static const std::string sentinel_obj_str = "{ \"a\": 10 }";

    json::value val = json::object {
        { "a", 10 }
    };
    EXPECT_EQ(json::to_string(val), sentinel_obj_str);
}

TEST(json_test, arr_init_list)
{
    static const std::string arr_str = "[ 10, \"-1.5\", null, true, {} ]";

    json::value val = json::array 
        { 10, "-1.5", json::null, true, json::object{} };
    EXPECT_EQ(json::to_string(val), arr_str);
}

TEST(json_test, copy_obj)
{
    static const json::value sentinel_obj = json::object { { "a", "b" } };

    json::value new_obj = json::object {};
    new_obj = sentinel_obj;
    EXPECT_EQ(new_obj, sentinel_obj);
}

TEST(json_test, move_obj)
{
    static const json::value sentinel_obj = json::object { { "a", "b" } };

    json::value first_obj = sentinel_obj;
    json::value second_obj = std::move(first_obj);
    EXPECT_EQ(second_obj, sentinel_obj);
}

TEST(json_test, get_from_obj_by_name)
{
    static json::object test_obj {
        { "aleph", "bet" }, { "gamma", json::null }, { "e", 3 }
    };

    auto res = json::get<json::integer>(test_obj["e"]);
    EXPECT_EQ(res, 3);
}

TEST(json_test, add_elems_to_obj)
{
    static const json::object start_obj {
        { "aleph", "bet" }, 
        { "gimel", 10 },
        { "omega", json::object { { "x", "y" } } }
    };
    static const json::object end_obj { 
        { "aleph", "bet" }, 
        { "gimel", 10 },
        { "omega", json::object { { "x", "y" } } }, 
        { "dummy", false }
    };

    json::object obj = start_obj;
    obj["dummy"] = false;
    EXPECT_EQ(obj, end_obj);
}

TEST(json_test, modify_elems_in_obj)
{
    static const json::object start_obj {
        { "aleph", "bet" }, { "gimel", 10 }
    };
    static const json::object end_obj { 
        { "aleph", "bet" }, { "gimel", json::null }
    };

    json::object obj = start_obj;
    obj["gimel"] = json::null;
    EXPECT_EQ(obj, end_obj);
}

TEST(json_test, add_elems_to_arr)
{
    static const json::array start_arr { 10, json::null, false };
    static const json::array end_arr { 10, json::null, false, 1.0 };

    json::array arr = start_arr;
    arr.push_back(1.0);
    EXPECT_EQ(arr, end_arr);
}

TEST(json_test, modify_elems_in_arr)
{
    static const json::array start_arr { 10, json::null, false };
    static const json::array end_arr { 10, "aleph", false };

    json::array arr = start_arr;
    arr[1] = "aleph";
    EXPECT_EQ(arr, end_arr);
}

TEST(json_test, iterate_thru_obj)
{
    static const json::object obj {
        { "at", 10 },
        { "bt", 15 }, 
        { "ct", 16 }
    };

    json::integer sum = 0;
    for (const auto &pair: obj)
        sum += json::get<json::integer>(pair.second);
    EXPECT_EQ(sum, 41);
}

TEST(json_test, iterate_thru_arr)
{
    static const json::array arr = { 1, 2, 3, 4, 5 };

    json::integer sum = 0;
    for (const auto &arr_elem: arr)
        sum += json::get<json::integer>(arr_elem);
    EXPECT_EQ(sum, 15);
}

TEST(json_test, stdlib_obj)
{
    static const json::object obj {
        { "at", 10 },
        { "bt", 15 }, 
        { "ct", 16 }
    };

    json::integer sum = std::accumulate(
        obj.cbegin(), obj.cend(), json::integer(0),
        [](json::integer acc, const json::object::value_type &p) { 
            return acc + json::get<json::integer>(p.second); 
        }
    );
    EXPECT_EQ(sum, 41);
}

TEST(json_test, stdlib_arr)
{
    static const json::array arr = { 1, 2, 3, 4, 5 };

    json::integer sum = std::accumulate(
        arr.cbegin(), arr.cend(), json::integer(0),
        [](json::integer acc, const json::array::value_type &v) { 
            return acc + json::get<json::integer>(v);
        }
    );
    EXPECT_EQ(sum, 15);
}

TEST(parse_test, true_val)
{
    static const json::value res = true;
    static const std::string str_repr = "  true ";

    json::value val = json::parse(str_repr);
    EXPECT_EQ(val, res);
}

TEST(parse_test, int_val)
{
    static const json::value res = 10;
    static const std::string str_repr = "10";

    json::value val = json::parse(str_repr);
    EXPECT_EQ(val, res);
}

TEST(parse_test, double_val)
{
    static const json::value res = -15.6e4;
    static const std::string str_repr = "-15.6e4";

    json::value val = json::parse(str_repr);
    EXPECT_EQ(val, res);
}

TEST(parse_test, string_val)
{
    static const json::value res = "abra\nschwabra\"kadabra";
    static const std::string str_repr = "\"abra\\nschwabra\\\"kadabra\"";

    json::value val = json::parse(str_repr);
    EXPECT_EQ(val, res);
}

TEST(parse_test, one_lvl_object_val)
{
    static const json::value res = json::object { 
        { "hello", "hell" }, { "alpha", 10 } 
    };
    static const std::string str_repr = R"-( {
        "hello": "hell", "alpha": 10
    } )-";

    json::value val = json::parse(str_repr);
    EXPECT_EQ(val, res);
}

TEST(parse_test, one_lvl_array_val)
{
    static const json::value res = json::array { 10, "-1.5", json::null, true };
    static const std::string str_repr = "[ 10, \"-1.5\", null, true ]";

    json::value val = json::parse(str_repr);
    EXPECT_EQ(val, res);
}

TEST(parse_test, array_in_obj_val)
{
    static const json::value res = json::object {
        { "fibs", json::array { 1, 1, 2, 3, 5, 8, 13 } }
    };
    static const std::string str_repr = R"-( {
        "fibs": [ 1, 1, 2, 3, 5, 8, 13 ]
    } )-";

    json::value val = json::parse(str_repr);
    EXPECT_EQ(val, res);
}
