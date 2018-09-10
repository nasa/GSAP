//
//  progMain.cpp
//  Example
//
//  Created by Teubert, Christopher (ARC-TI)[SGT, INC] on 5/4/16.
//  Copyright © 2018 United States Government as represented by the Administrator of the National
//  Aeronautics and Space Administration.  All Rights Reserved.
//

#include "ProgManager.h"

#include "BenchmarkPrognoser.h"
#include "PrognoserFactory.h"

#include "ConfigMap.h"
#include <ctime>
#include <fstream>
#include <iostream>
//#include "BenchmarkTest.cpp"

using namespace PCOE;

int main() {
    ConfigMap::addSearchPath("../example/cfg/");

    // Specify Prognosers
    PrognoserFactory& prognoserFactory = PrognoserFactory::instance();
    prognoserFactory.Register<BenchmarkPrognoser>("BenchmarkPrognoser");

    ProgManager PM = ProgManager("bench.cfg");

    printf("Benchmarking\n");
    PM.enable();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    PM.start();
    std::this_thread::sleep_for(std::chrono::minutes(1));
    PM.stop();

    return 0;
}
