/* 
 * File:   FeatureTracking.cpp
 * Author: Bas
 * 
 * Created on 20 augustus 2014, 12:36
 */

#include "FeatureTracking.h"

#include <vector>

FeatureTracking::FeatureTracking() {
}

FeatureTracking::FeatureTracking(const FeatureTracking& orig) {
}

FeatureTracking::~FeatureTracking() {
}

float FeatureTracking::getEnergy(){
    return energy;
}

std::vector<float> FeatureTracking::getOrientation(){
    return orientation;
}