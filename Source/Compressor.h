#pragma once

class Compressor
{
public:
    Compressor();

    float getGainReduction(float envelope);
    
    void prepare(double sampleRate);
    
    void reset();
    
    float getThreshold();
    
    void setThreshold(float threshold);
    
    float getRatio();
    
    void setRatio(float ratio);
    
    float getKnee();
    
    void setKnee(float knee);

private:
    double sampleRate;

    float threshold = 0;
    float ratio = 1;
    float knee = 0;
};
