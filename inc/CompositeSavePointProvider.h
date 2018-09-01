// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_COMPOSITESAVEPOINTPROVIDER_h
#define PCOE_COMPOSITESAVEPOINTPROVIDER_h

#include "ISavePointProvider.h"

namespace PCOE {
    /**
     * A container for objects that implement ISavePointProvider that acts as a single 
     * Save Point Provider
     *
     * @author Christopher Teubert
     * @since 1.2
     **/
    class CompositeSavePointProvider : public ISavePointProvider {
    public:
        std::vector<ISavePointProvider *> providers;
        
        std::vector<double> getSavePts() override {
            std::vector<double> savePts;
            for (auto && provider : providers) {
                std::vector<double> tmp = provider->getSavePts();
                savePts.insert(savePts.end(), tmp.begin(), tmp.end());
            }
            return savePts;
        }
        
        void add(ISavePointProvider * provider) {
            providers.push_back(provider);
        }
    };
}
#endif
