/*
 * File:   RepClassifier.h
 * Author: Bas
 *
 * Created on 27 juni 2014, 9:43
 */

#ifndef REPCLASSIFIER_H
#define	REPCLASSIFIER_H

#include <vector>
#include "DtwPsClassifier.h"
#include "AbstractGestureClassifier.h"
#include "../YIN/YIN.h"


class RepClassifier : public AbstractGestureClassifier {

    YIN* yin;
    DtwPsClassifier* dtw;

    int learning = -1;
    bool stayLearning = false;
    double recognitionThreshold = 10.;
    bool recording = false;
    bool learnGate = false;
    
    int minimumTemplateSize = 30;

public:
    RepClassifier();
    RepClassifier(int ndim);
    RepClassifier(const RepClassifier& orig);
    virtual ~RepClassifier();

    // wrapped to DtwPs
    void infer(std::vector<float>&);
    void addTemplate();
    void fillTemplate(int, std::vector<float>&);
    void clearTemplate(int);
    void clear();
    int mostLikelyGesture();
    double getPhase();
    int templateSize(int i);
    int size();
    double getDistance();
    
    void setMinimumTemplateSize( int i );
    int getMinimumTemplateSize();

    // wrapped YIN
    bool isSync();
    int repetitionInterval();
    
    void setAverageThreshold(float f);
    void setDipThreshold(float f);
    void setMaxLength(int l);
    void setMaxDelay(int d);
    void setMinDips(int md);


    // RepeatClassifier
    void learn();
    void learn(int i);
    void stopLearning();
    void setRecognitionThreshold(double r);
    void setResolution(int r);
    bool isLearning();
    bool isRecording();
    
    void setLearningGate( bool onoff );


private:
};

#endif	/* REPCLASSIFIER_H */

