#include <cmath>
#include "EnvelopeDetector.h"

EnvelopeDetector::EnvelopeDetector()
{
    setAttackTime(attackTime);
    setReleaseTime(releaseTime);
}

float EnvelopeDetector::getEnvelope(float sample)
{
    float envelope = currentEnvelope;
    sample = std::abs(sample);
    if (rms)
        sample *= sample;
    if (sample > currentEnvelope)
        envelope = attackTimeCoefficient * (currentEnvelope - sample) + sample;
    if (sample < currentEnvelope)
        envelope = releaseTimeCoefficient * (currentEnvelope - sample) + sample;
    envelope = fmin(envelope, 1.0);
    envelope = fmax(envelope, 0.0);
    currentEnvelope = envelope;
    if (rms)
        envelope = sqrt(envelope);
    return envelope;
}

void EnvelopeDetector::prepare(double sampleRate)
{
    this->sampleRate = sampleRate;
}

void EnvelopeDetector::reset()
{
    currentEnvelope = 0;
}

float EnvelopeDetector::getAttackTime()
{
    return attackTime;
}

void EnvelopeDetector::setAttackTime(float attackTime)
{
    if (attackTime != this->attackTime)
    {
        this->attackTime = attackTime;
        this->attackTimeCoefficient = std::exp(std::log10(0.368) / (attackTime * sampleRate));
    }
}

float EnvelopeDetector::getReleaseTime()
{
    return releaseTime;
}

void EnvelopeDetector::setReleaseTime(float releaseTime)
{
    if (releaseTime != this->releaseTime)
    {
        this->releaseTime = releaseTime;
        this->releaseTimeCoefficient = std::exp(std::log10(0.368) / (releaseTime * sampleRate));
    }
}

bool EnvelopeDetector::getRMS()
{
    return rms;
}

void EnvelopeDetector::setRMS(bool rms)
{
    this->rms = rms;
}
