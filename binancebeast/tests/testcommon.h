#ifndef BB_TESTCOMMON_H
#define BB_TESTCOMMON_H

#include <binancebeast/BinanceBeast.h>
#include <boost/json.hpp>
#include <chrono>


namespace bblib_test
{
    namespace json = boost::json;
    using namespace std::chrono_literals;

    void testFail (std::string_view test, std::string_view reason = "timeout")
    {
        std::cout << test << " FAIL: " << reason << "\n";
    }


    bool hasError(std::string_view test, bblib::RestResult& result)
    {
        if (result.hasErrorCode())
        {
            testFail(test, std::to_string(json::value_to<std::int32_t>(result.json.as_object()["code"])) + std::string{" : "} + json::value_to<std::string>(result.json.as_object()["msg"]));
            return true;
        }
        return false;
    }


    bool hasError(std::string_view test, bblib::WsResult& result)
    {
        if (result.hasErrorCode())
        {
            testFail(test, std::to_string(json::value_to<std::int32_t>(result.json.as_object()["code"])) + std::string{" : "} + json::value_to<std::string>(result.json.as_object()["msg"]));
            return true;
        }
        return false;
    }

        
    bool waitReply (std::condition_variable& cvHaveReply, const std::string_view test, const std::chrono::milliseconds timeout = 3s)
    {
        std::mutex mux;

        std::unique_lock lck(mux);
        if (cvHaveReply.wait_for(lck, timeout) == std::cv_status::timeout)
        {
            bblib_test::testFail(test);
            return false;
        }
        return true;

    }
}
#endif
