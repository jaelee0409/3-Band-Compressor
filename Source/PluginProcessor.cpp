/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
_3BandCompressorAudioProcessor::_3BandCompressorAudioProcessor()
    : apvts(*this, nullptr, "Parameters", createParameterLayout()),
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    lowBandCompressor.setParameters(dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Threshold")),
                                    dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Attack")),
                                    dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Release")),
                                    dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("Ratio")),
                                    dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Bypass")));

    jassert(apvts.getParameter("Threshold") != nullptr);
    jassert(apvts.getParameter("Attack") != nullptr);
    jassert(apvts.getParameter("Release") != nullptr);
    jassert(apvts.getParameter("Ratio") != nullptr);
    jassert(apvts.getParameter("Bypass") != nullptr);
}

_3BandCompressorAudioProcessor::~_3BandCompressorAudioProcessor() {
}

//==============================================================================
const juce::String _3BandCompressorAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool _3BandCompressorAudioProcessor::acceptsMidi() const {
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool _3BandCompressorAudioProcessor::producesMidi() const {
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool _3BandCompressorAudioProcessor::isMidiEffect() const {
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double _3BandCompressorAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int _3BandCompressorAudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int _3BandCompressorAudioProcessor::getCurrentProgram() {
    return 0;
}

void _3BandCompressorAudioProcessor::setCurrentProgram (int index) {
}

const juce::String _3BandCompressorAudioProcessor::getProgramName (int index) {
    return {};
}

void _3BandCompressorAudioProcessor::changeProgramName (int index, const juce::String& newName) {
}

//==============================================================================
void _3BandCompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getTotalNumOutputChannels();
	lowBandCompressor.prepare(spec);
}

void _3BandCompressorAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool _3BandCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void _3BandCompressorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    //for (int channel = 0; channel < totalNumInputChannels; ++channel) {
    //    auto* channelData = buffer.getWritePointer (channel);

    //    // ..do something to the data...
    //}

	lowBandCompressor.updateSettings();
	//midBandCompressor.updateSettings();
	//highBandCompressor.updateSettings();
	lowBandCompressor.process(buffer);
	//midBandCompressor.process(buffer);
	//highBandCompressor.process(buffer);
}

//==============================================================================
bool _3BandCompressorAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* _3BandCompressorAudioProcessor::createEditor() {
    //return new _3BandCompressorAudioProcessorEditor (*this);
	return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void _3BandCompressorAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
	juce::MemoryOutputStream mos(destData, true);
	apvts.state.writeToStream(mos);
}

void _3BandCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ValueTree valueTree = juce::ValueTree::readFromData(data, sizeInBytes);
	if (valueTree.isValid()) {
		apvts.replaceState(valueTree);
	}
}

juce::AudioProcessorValueTreeState::ParameterLayout _3BandCompressorAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

	layout.add(std::make_unique<juce::AudioParameterFloat>("Threshold",
                                                  "Threshold",
                                                  juce::NormalisableRange<float>(-60.0f, 12.0f, 1.0f, 1.0f),
                                                  0.0f));

    juce::NormalisableRange<float> attackAndReleaseRange = juce::NormalisableRange<float>(5.0f, 500.0f, 1.0f, 1.0f);
	layout.add(std::make_unique<juce::AudioParameterFloat>("Attack",
		                                          "Attack",
		                                          attackAndReleaseRange,
		                                          50.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Release",
                                                  "Release",
                                                  attackAndReleaseRange,
                                                  250.0f));

	std::vector<float> ratioChoices = std::vector<float>{ 1.0f, 1.5f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f,
                                                                10.0f, 15.0f, 20.0f, 50.0f, 100.0f };
    juce::StringArray ratioStringArray;
	for (auto choice : ratioChoices) {
        ratioStringArray.add(juce::String(choice, 1));
	}
	layout.add(std::make_unique<juce::AudioParameterChoice>("Ratio",
		                                                "Ratio",
                                                        ratioStringArray,
		                                                3));

	layout.add(std::make_unique<juce::AudioParameterBool>("Bypass",
		                                                  "Bypass",
		                                                  false));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new _3BandCompressorAudioProcessor();
}
