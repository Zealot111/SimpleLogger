#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <map>

#include "SimpleLogger.h"

using namespace s_logger;
using namespace std;

void f1() {
    for (int i = 0; i < 100; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        LOG(TRACE) << i;
        LOGF(TRACE, "%i", i);
    }
}


int main()
{
    
    create_logger("default", true, "test.%datetime.log");

    std::thread t2(f1);
    
    LOG(DEBUG) << "Test Test Test";
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    LOG(FATAL) << "Test2 Test2 Test2";

    for (int i = 0; i < 100; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        LOG(TRACE) << i;
    }
    t2.join();



}

