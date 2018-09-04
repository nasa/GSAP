// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_COMPOSITESAVEPOINTPROVIDER_H
#define PCOE_COMPOSITESAVEPOINTPROVIDER_H

#include <algorithm>

#include "ISavePointProvider.h"

namespace PCOE {
    /**
     * A container for objects that implement ISavePointProvider that acts as a single
     * Save Point Provider.
     *
     * @author Christopher Teubert
     * @since 1.2
     **/
    class CompositeSavePointProvider : public ISavePointProvider {
    public:
        const std::set<Message::time_point>& getSavePts() override {
            for (auto&& provider : providers) {
                std::set<Message::time_point> tmp = provider->getSavePts();
                for (auto&& elem : tmp) {
                    savePts.insert(elem);
                }
            }
            return savePts;
        }
        
        /**
         *  Add a new save point provider to the composite save point provider
         *  @param provider The save point provider to add
         */
        void add(ISavePointProvider* provider) {
            providers.push_back(provider);
        }
        
        bool hasChangedSinceSavePtsCall() const override {
            return !std::all_of(providers.begin(),
                                providers.end(),
                                [](ISavePointProvider* p){return !p->hasChangedSinceSavePtsCall();});
        }
        
    private:
        std::vector<ISavePointProvider *> providers;
        std::set<Message::time_point> savePts; // Cached version of savePts (so we can return a ref)
    };
}
#endif
