#include <binancebeast/BinanceBeast.h>
#include "testcommon.h"
#include <future>
#include <chrono>
#include <condition_variable>


using namespace bblib;
using namespace bblib_test;

using WsFunction = void (BinanceBeast::*)(RestCallback, string);


bool showResponseData = false, dataError = false;
std::condition_variable cvHaveReply;
string_view test;


void onWsResponse(WsResult result)
{
    if (showResponseData)
        std::cout << "\n" << result.json << "\n";

    dataError = bblib_test::hasError(test, result);

    cvHaveReply.notify_all();
}


void runTest(BinanceBeast& bb, string currentTest, string stream, const bool showData = false, const bool alwaysExpectResponse = true)
{
    showResponseData = showData;
    test = currentTest;

    bb.startWebSocket(onWsResponse, stream);

    auto haveReply = waitReply(cvHaveReply, test);
    
    if (alwaysExpectResponse)
        std::cout << "Test: " << currentTest << " : " << (!dataError && haveReply ? "PASS" : "FAIL") << "\n";
    else
        std::cout << "Test: " << currentTest << " : " << (!dataError ? "PASS" : "FAIL") << "\n";
}


int main (int argc, char ** argv)
{
    std::cout << "\n\nTest WebSockets API\n\n";

    auto config = ConnectionConfig::MakeTestNetConfig(argc == 2 ? argv[1] : "");

    BinanceBeast bb;
    bb.start(config);

    runTest(bb, "aggregrateTrade", "btcusdt@aggTrade");
    runTest(bb, "markPrice", "btcusdt@markPrice@1s");
    runTest(bb, "markPriceForAll", "!markPrice@arr@1s");
    runTest(bb, "klines", "btcusdt@kline_15m");
    runTest(bb, "continuousContractKline", "btcusdt_perpetual@continuousKline_1m");
    runTest(bb, "individualSymbolMiniTicker", "btcusdt@miniTicker");
    runTest(bb, "allMarketMiniTicker", "!miniTicker@arr");
    runTest(bb, "individualSymbolTicker", "btcusdt@ticker");
    runTest(bb, "allMarketTicker", "!ticker@arr");
    runTest(bb, "individualSymbolBookTicker", "btcusdt@bookTicker");
    runTest(bb, "allBookTicker", "!bookTicker");
    runTest(bb, "liquidationOrder", "btcusdt@forceOrder", false, false); 
    runTest(bb, "allMarketLiquidationOrder", "!forceOrder@arr", false, false); 
    runTest(bb, "partialBookDepth", "btcusdt@depth5@100ms"); 
    runTest(bb, "diffBookDepth", "btcusdt@depth@100ms"); 
    runTest(bb, "compositeIndexSymbolInfo", "defiusdt@compositeIndex");     
}
