//
//  ConfigMapTests.h
//  UnitTestAll
//
//  Created by Chris Teubert on 4/5/16.
//  Copyright © 2018 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.
//

#ifndef CONFIGMAPTESTS_H
#define CONFIGMAPTESTS_H

void configMapInit();
void configMapLoadArgs();
void configMapUse();
void configMapLoad();
void configMapLoadNonexistent();
void configMapAddBadSearchPath();
void configMapTrim();
void configMapRequireKeys();

#endif // CONFIGMAPTESTS_H
