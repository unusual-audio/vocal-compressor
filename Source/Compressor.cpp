#include "Compressor.h"

Compressor::Compressor()
{
    
}

float Compressor::getGainReduction(float envelope)
{
    float gainReduction = (1 - (1 / ratio));
    float softKneeLowerBound = threshold - knee / 2;
    float softKneeUpperBound = threshold + knee / 2;
    
    if (envelope <= softKneeLowerBound)
    {
        return 0;
    }
    if (envelope >= softKneeUpperBound)
    {
        return (envelope - threshold) * gainReduction;
    }
    float position = (envelope - softKneeLowerBound) / knee;
    return (envelope - softKneeLowerBound) * gainReduction * position / 2;
}

void Compressor::prepare(double sampleRate)
{
    this->sampleRate = sampleRate;
}

void Compressor::reset()
{
}

float Compressor::getThreshold()
{
    return threshold;
}

void Compressor::setThreshold(float threshold)
{
    this->threshold = threshold;
}

float Compressor::getRatio()
{
    return ratio;
}

void Compressor::setRatio(float ratio)
{
    this->ratio = ratio;
}

float Compressor::getKnee()
{
    return knee;
}

void Compressor::setKnee(float knee)
{
    this->knee = knee;
}
