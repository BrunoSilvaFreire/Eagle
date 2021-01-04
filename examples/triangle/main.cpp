//
// Created by Ricardo on 11/28/2020.
//

#include "TriangleApp.h"

int main(){


    Eagle::Log::init(Eagle::Log::TRACE, Eagle::Log::TRACE);

    TriangleApp app;

    try {
        app.run();
    } catch(const std::exception& e) {
        EG_FATAL_F("An exception occurred: {0}", e.what());
        return 1;
    }

    return 0;
}