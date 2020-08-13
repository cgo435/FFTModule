
#pragma once

#include <JuceHeader.h>

class FFTModule {
public:
    // =============================================================================
    // public member functions
    // =============================================================================
    FFTModule();
    ~FFTModule();
    
    void process(const float* inputData, int numSamples);
    
    // set functions
    void setFftSize(int size);
    void setUpdateSize(int size);
    void setRmsThreshold(float threshold);
    
    // returns a pointer to the outputBuffer
    float* getFft();
    float getFftSize();
    
    // sets the sizes of the buffers and initializes the FFT object
    void reset();
    
private:
    // =============================================================================
    // private member functions
    // =============================================================================
    // updates the inputBuffer with a new block of audio
    void fillBuffer(const float* inputData, int numSamples);
    
    // called when the updateCounter is done
    void calculateFft();
    
    // calculates the RMS of a signal
    float rms(const float* x, int numSamples) const;
    
    // =============================================================================
    // private member variables
    // =============================================================================
    // number of samples in each FFT
    int fftSize = 4096;
    
    // number of samples to wait between FFT's
    //   usually fftSize is an integer multiple of updateSize
    int updateSize = fftSize/2;
    int updateCounter = 0;
    
    // the output from this class will be slightly normalized/compressed..
    //   any input buffer with an RMS below this value will not have
    //   any normalization applied to it
    float rmsThreshold = 1e-3f;
    
    // does the FFT
    std::unique_ptr<juce::dsp::FFT> fft;
    
    // inputBuffer stores incoming blocks of audio
    // fftBuffer is where the in-place FFT occurs
    // outputBuffer is where the finished product gets copied to (post normalization)
    juce::AudioBuffer<float> inputBuffer, outputBuffer, fftBuffer;
};
