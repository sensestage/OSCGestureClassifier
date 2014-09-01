/* 
 * File:   FeatureTracking.h
 * Author: Bas
 *
 * Created on 20 augustus 2014, 12:36
 */

#ifndef FEATURETRACKING_H
#define	FEATURETRACKING_H

#include <vector>

class FeatureTracking {
public:
    FeatureTracking();
    FeatureTracking(const FeatureTracking& orig);
    virtual ~FeatureTracking();
    
    float getEnergy();
    std::vector<float> getOrientation();
    void infer(std::vector<float>);
    
private:
    float energy = 0.;
    std::vector<float> orientation;

};

#endif	/* FEATURETRACKING_H */

