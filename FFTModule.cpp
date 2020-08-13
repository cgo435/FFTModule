

#include "FFTModule.h"

FFTModule::FFTModule() {
    reset();
}

FFTModule::~FFTModule() {}

void FFTModule::process(const float* inputData, int numSamples) {
    // fill the input audio buffer
    fillBuffer(inputData, numSamples);
    
    // count the new samples
    updateCounter += numSamples;
    
    // if its time for an update, calculate the new FFT
    // and reset the update counter
    if (updateCounter >= updateSize) {
        updateCounter -= updateSize;
        calculateFft();
    }
}

void FFTModule::fillBuffer(const float* inputData, int numSamples) {
    int numOldSamples = fftSize - numSamples;
    auto inputPtr = inputBuffer.getWritePointer(0);
    for (int i = 0; i < numOldSamples; i++) {
        inputPtr[i] = inputPtr[numSamples+i];
    }
    inputBuffer.copyFrom(0, numOldSamples, inputData, numSamples);
}

void FFTModule::calculateFft() {
    // copy the current audio in the input buffer to the fft buffer
    //   b/c 'fft' does an in-place FFT
    fftBuffer.copyFrom(0, 0, inputBuffer, 0, 0, fftSize);
    auto fftPtr = fftBuffer.getWritePointer(0);
    float inputRms = rms(fftPtr, fftSize);
    
    // normalize the frequency magnitudes using RMS of FFT block
    float gain;
    if (std::abs(inputRms) >= rmsThreshold) // prevents division by 0
        gain = 1.0f/(std::pow(inputRms,1.0f/2.0f)*fftSize);
    else
        gain = 1.0f/fftSize;
    
    fft->performFrequencyOnlyForwardTransform(fftPtr);
    fftBuffer.applyGain(gain);
    outputBuffer.copyFrom(0, 0, fftBuffer, 0, 0, fftSize);
}

void FFTModule::reset() {
    // set the sizes of all buffers and the FFT
    int fftOrder = std::log2(fftSize);
    fft.reset(new juce::dsp::FFT(fftOrder));
    inputBuffer.setSize(1, fftSize);
    inputBuffer.clear();
    outputBuffer.makeCopyOf(inputBuffer);
    fftBuffer.setSize(1, fftSize*2);
    fftBuffer.clear();
}

void FFTModule::setFftSize(int size) {
    fftSize = size;
    reset();
}

void FFTModule::setUpdateSize(int size) {
    updateSize = size;
}

void FFTModule::setRmsThreshold(float threshold) {
    rmsThreshold = threshold;
}

float* FFTModule::getFft() {
    return outputBuffer.getWritePointer(0);
}

float FFTModule::getFftSize() {
    return fftSize;
}

float FFTModule::rms(const float* x, int numSamples) const {
    float squaredSum = 0;
    for (int i = 0; i < numSamples; i++) squaredSum += x[i]*x[i];
    return std::sqrt(squaredSum/numSamples);
}
