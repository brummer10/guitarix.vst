/*
 * Copyright (C) 2022 Maxim Alexanian
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include <JuceHeader.h>
#include <sigc++/sigc++.h>
namespace gx_jack { class GxJack; }
namespace gx_engine { class GxMachine; class Parameter; }
namespace gx_system { class CmdlineOptions; }

//==============================================================================
/**
*/
class GuitarixEditor;

class GuitarixStart 
{
public:
    GuitarixStart(int argc, char *argv[]);
    ~GuitarixStart();
    gx_jack::GxJack *get_jack() { return jack;}
    gx_jack::GxJack *get_jack_r() { return jack_r;}
    gx_engine::GxMachine *get_machine() { return machine;}
    gx_engine::GxMachine *get_machine_r() { return machine_r;}
    gx_system::CmdlineOptions *get_options() { return options;}

    void gx_load_preset(gx_engine::GxMachine* machine, const char* bank, const char* name);
    void gx_save_preset(gx_engine::GxMachine* machine, const char* bank, const char* name);
    void check_config_dir();

private:
    bool need_new_preset;
    gx_engine::GxMachine *machine, *machine_r;
    gx_jack::GxJack *jack, *jack_r;
    static gx_system::CmdlineOptions *options;
};

class PluginUpdateTimer : public juce::Timer
{
public:
	PluginUpdateTimer() : machine(0), editor(0), mUpdateMode(false) {}
	void set_machine(gx_engine::GxMachine *m, gx_engine::GxMachine *m_r) { machine = m; machine_r = m_r; }
	void set_editor(GuitarixEditor* ed) { editor = ed; }
	void update_mode() { mUpdateMode = true; }
	void timerCallback() override;
    juce::CriticalSection timer_cs;

private:
	gx_engine::GxMachine *machine, *machine_r;
	GuitarixEditor* editor;
	bool mUpdateMode;
};

class GuitarixProcessor : public juce::AudioProcessor, private juce::AudioProcessorParameter::Listener
{
public:
	//==============================================================================
	GuitarixProcessor();
	~GuitarixProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
	void process_midi(juce::MidiBuffer& midiMessages);
	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	juce::AudioProcessorEditor* editorIn;
	juce::AudioProcessorEditor* getEditor() ;
	bool hasEditor() const override;

	void set_editor(GuitarixEditor* ed) { editor = ed; timer.set_editor(ed); compareParameters(); }
	double scale;
	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	void get_machine_jack(gx_jack::GxJack *&j, gx_engine::GxMachine *&m, bool right) { if (right) { j = jack_r; m = machine_r; } else { j = jack; m = machine; } }

	void SetStereoMode(bool on);
	bool GetStereoMode() const { return mStereoMode; }
	void SetMultiMode(bool on) { mMultiMode = on; }
	bool GetMultiMode() const { return mMultiMode; }
	void SetMonoMute(bool m1, bool m2) { mMono1Mute = m1; mMono2Mute = m2; }
	void GetMonoMute(bool &m1, bool &m2) const { m1 = mMono1Mute; m2 = mMono2Mute; }

	void SetPresetsVisible(bool vis) { mPresetsVisible = vis; }
	bool GetPresetsVisible() const { return mPresetsVisible; }

	const juce::String& GetCurrentFile() const { return currentFile; }
	void SetCurrentFile(const juce::String& f) { currentFile=f; }

    const std::array<juce::LinearSmoothedValue<float>, 4>& getRMSValues() const {return rms;}
    void load_preset(std::string _bank, std::string _preset);
    void save_preset(std::string _bank, std::string _preset);
    void update_plugin_list(bool add);
    gx_system::CmdlineOptions *get_options() { return options; }
    juce::RangedAudioParameter* findParamForID(const char *id);
private:
	bool mStereoMode, mMultiMode;
	bool mMono1Mute, mMono2Mute;

	GuitarixStart *gx;
	gx_system::CmdlineOptions *options;
	gx_jack::GxJack *jack, *jack_r;
	gx_engine::GxMachine *machine, *machine_r;
	gx_engine::GxMachine *get_machine(bool right = false) { return right ? machine_r: machine; }
	GuitarixEditor *editor;

	void saveState(std::ostream &os, bool right);
	void loadState(std::istream &is, bool right);
    void do_program_change(int pgm);
	void do_bank_change(int pgm);
	void cloneSettingsToMachineR();

	void refreshPrograms();
	std::vector<std::pair<std::string, std::string>> presets;
	int currentPreset;

	void connect_value_changed_signal(gx_engine::Parameter *p, bool right);
	void on_param_value_changed(gx_engine::Parameter *p, bool right);
	void on_param_insert_remove(gx_engine::Parameter *p, bool inserted, bool right);
	void on_rack_unit_changed(bool stereo, bool right);
	sigc::signal<void,int> pgm_chg;
	sigc::signal<void,int> bank_chg;
	std::string switch_bank;
	bool mLoading;

    int buffersize, quantum, delay, tdelay;
    float *out[2];
    int olen, wpos, rpos, ppos;
    
    void process(float *out[2], int n);

	PluginUpdateTimer timer;

	juce::AudioParameterBool* par_stereo;
    std::map<int, juce::RangedAudioParameter*> parameterMap;
    void forwardParameters();
    void compareParameters();
	void parameterValueChanged(int parameterIndex, float newValue) override;
	void parameterGestureChanged(int, bool) override {}

	juce::String currentFile;
	juce::File defaultPath;

	bool mPresetsVisible;

    float getRMSLevel(float* data, int len) const;
    std::array<juce::LinearSmoothedValue<float>, 4> rms;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarixProcessor)
};
