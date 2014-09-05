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

public:
    RepClassifier();
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

    // wrapped YIN
    bool isSync();
    int repetitionInterval();

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

