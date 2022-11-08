#pragma once

class EnvelopeDetector
{
public:
    EnvelopeDetector();
    
    float getEnvelope(float sample);

    void prepare(double sampleRate);
    
    void reset();
    
    float getAttackTime();
    
    void setAttackTime(float attackTime);
    
    float getReleaseTime();
    
    void setReleaseTime(float releaseTime);
    
    bool getRMS();
    
    void setRMS(bool rms);
    
private:
    
    double sampleRate;

    float currentEnvelope = 0;

    float attackTime = 0.05f;
    float releaseTime = 0.5f;
    
    bool rms = true;

    float attackTimeCoefficient;
    float releaseTimeCoefficient;
    
};
