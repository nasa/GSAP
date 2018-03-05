//
//  ParticleFilterTests.h
//  UnitTestAll
//
//  Created by Julian Vu on 2/28/17.

#ifndef GSAP_PARTICLEFILTERTESTS_H
#define GSAP_PARTICLEFILTERTESTS_H


void ctor();
void ctorWithNonemptyVectors();
void GSAPConfigMapCtor();
void setSensorCovariance();
void checkNoiseVectors();
void setModel();
void setMinEffective();
void PFinitialize();
void getStateMean();
void getOutputMean();
void normalize();
void resample();
void systematicResample();
void generateProcessNoise();
void likelihood();
void weightedMean();
void step();
void getStateEstimate();



#endif //GSAP_PARTICLEFILTERTESTS_H
