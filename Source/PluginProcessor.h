/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CompressorBand {
public:
    CompressorBand() 
		: threshold(nullptr), attack(nullptr), release(nullptr), ratio(nullptr), bypass(nullptr)
	{
    }

	void prepare(const juce::dsp::ProcessSpec& spec) {
		compressor.prepare(spec);
	}
	void updateSettings() {
		if (threshold)
			compressor.setThreshold(threshold->get());
		if (attack)
			compressor.setAttack(attack->get());
		if (release)
			compressor.setRelease(release->get());
		if (ratio)
			compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
	}
	void process(juce::AudioBuffer<float>& buffer) {
		juce::dsp::AudioBlock<float> audioBlock(buffer);
		juce::dsp::ProcessContextReplacing<float> context(audioBlock);
		context.isBypassed = bypass->get();
		compressor.process(context);
	}
	void setParameters(juce::AudioParameterFloat* thresholdParam,
		juce::AudioParameterFloat* attackParam,
		juce::AudioParameterFloat* releaseParam,
		juce::AudioParameterChoice* ratioParam,
		juce::AudioParameterBool* bypassParam)
	{
		threshold = thresholdParam;
		attack = attackParam;
		release = releaseParam;
		ratio = ratioParam;
		bypass = bypassParam;
	}
	void setThreshold(float threshold) {
		compressor.setThreshold(threshold);
	}
	void setRatio(float ratio) {
		compressor.setRatio(ratio);
	}
	void setAttack(float attack) {
		compressor.setAttack(attack);
	}
	void setRelease(float release) {
		compressor.setRelease(release);
	}
	
private:
	juce::dsp::Compressor<float> compressor;

	juce::AudioParameterFloat* attack;
	juce::AudioParameterFloat* release;
	juce::AudioParameterFloat* threshold;
	juce::AudioParameterChoice* ratio;
	juce::AudioParameterBool* bypass;
};

//==============================================================================
/**
*/
class _3BandCompressorAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    _3BandCompressorAudioProcessor();
    ~_3BandCompressorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
	juce::AudioProcessorValueTreeState apvts;

	CompressorBand lowBandCompressor;
	//CompressorBand midBandCompressor;
	//CompressorBand highBandCompressor;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (_3BandCompressorAudioProcessor)
};
