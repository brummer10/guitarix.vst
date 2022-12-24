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
#include "GuitarixProcessor.h"
#include <glibmm.h>
#include "PluginEditor.h"
#include "guitarix.h"

namespace gx_jack { class GxJack; }
namespace gx_engine { class GxMachine; class Parameter; class Plugin; class ParamMap;  }
namespace gx_preset { class GxSettings; }
namespace gx_system { class PresetFile; class PresetBanks; class PresetFile; }
struct PluginDef;

//==============================================================================
class MachineEditor : public juce::Component, public sigc::trackable
{
public:
	enum MonoT{ mn_Mono, mn_Stereo, mn_Both };
	MachineEditor(GuitarixProcessor& p, bool right, MonoT mono);
    ~MachineEditor() override;
    std::vector<std::pair<juce::Component*, std::string> >clist;

    //==============================================================================

	void createPluginEditors();
	bool GetAlternateDouble() const { return false;/* mAlternateDouble; */}

	//PluginEditor callbacks =======================================================
	void registerParListener(ParListener *ed);
	void unregisterParListener(ParListener *ed);
	PluginDef* get_pdef(const char *id);
	gx_engine::ParamMap& get_param();
	gx_engine::Parameter* get_parameter(const char* pid);
	void list(const char* id, std::list<gx_engine::Parameter*> &pars);
	void fillPluginCombo(juce::ComboBox *c, bool stereo, const char* id);
	void pluginMenuChanged(PluginEditor *ped, juce::ComboBox *c, bool stereo);
	void updateMuteButton(juce::ToggleButton *b, const char* id);
	void muteButtonClicked(juce::ToggleButton *b, const char* id);
	void addButtonClicked(PluginEditor *ped, bool stereo);
	void removeButtonClicked(PluginEditor *ped, bool stereo);
	void SetAlternateDouble(bool alternateDouble) {mAlternateDouble = alternateDouble;}
	void on_param_value_changed(gx_engine::Parameter *p);
	void on_rack_unit_changed(bool stereo);
    bool plugin_in_use(const char* id);
	gx_engine::GxMachine *machine;
private:
	gx_jack::GxJack *jack;
	gx_preset::GxSettings *settings;
	bool mRight;
	bool mAlternateDouble;
	MonoT mMono;

	// signal handler
	void on_param_insert_remove(gx_engine::Parameter *p, bool insert);

	bool mIgnoreRackUnitChange;

	void connect_value_changed_signal(gx_engine::Parameter *p);
	//
	bool insert_rack_unit(const char* id, const char* before, bool stereo);
	bool remove_rack_unit(const char* id, bool stereo);

	//calls
	void get_visible_mono(std::list<gx_engine::Plugin*> &l);
	void get_visible_stereo(std::list<gx_engine::Plugin*> &l);

	//======================================================

	void buildPluginCombo(juce::ComboBox *c, std::list<gx_engine::Plugin*> &lv, const char* selid);
	
	juce::ConcertinaPanel cp;

	void addEditor(int idx, PluginSelector *ps, PluginEditor *pe, const char* name);
	std::list<ParListener*> editors;
	PluginEditor inputEditor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MachineEditor)
};

class HorizontalMeter: public juce::Component
{
public:
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        g.setColour(juce::Colours::white.withBrightness(0.4f));
        g.fillRoundedRectangle(bounds, 4.f);
        
        const auto scaledX=juce::jmap(level,-60.f, +6.f, 0.f, bounds.getWidth());
        const auto scaledCol=juce::jmap(level,-60.f, 0.f, 0.5f, 1.0f);
        g.setColour(juce::Colours::white.withBrightness(scaledCol));
        g.fillRoundedRectangle(bounds.removeFromLeft(scaledX), 4.f);
    }
    
    void setLevel(float value) {level=value;}

private:
    float level = -60.f;
};

//==============================================================================
class GuitarixEditor : public juce::AudioProcessorEditor, public juce::Button::Listener, public juce::Timer
{
public:
	GuitarixEditor(GuitarixProcessor&);
	~GuitarixEditor() override;
    ladspa::LadspaPluginList ml;

    void timerCallback() override;
    
    void paint(juce::Graphics&) override;
	void resized() override;

	void createPluginEditors(bool l=true, bool r=true, bool s=true);
	void updateModeButtons();

	bool GetAlternateDouble() const { return ed.GetAlternateDouble()/* || ed_r.GetAlternateDouble()*/; }

private:
	GuitarixProcessor& audioProcessor;

	MachineEditor ed, /*ed_r, */ed_s;
    
    gx_jack::GxJack *jack;
    gx_engine::GxMachine *machine;
    gx_preset::GxSettings *settings;

	juce::TextButton monoButton, stereoButton, aboutButton, pluginButton /*, singleButton, multiButton, mute1Button, mute2Button*/;
	void buttonClicked(juce::Button* b) override;

	juce::ComboBox presetFileMenu;
    HorizontalMeter meters[4];

    gx_system::PresetFile* get_bank(const std::string& id);
    gx_system::PresetBanks* banks();
    gx_system::PresetFile* presets(const std::string& id);
    
    std::string new_bank;
    std::string new_preset;
    void on_preset_select();
    void load_preset_list();
    static void loadLV2PlugCallback(int i, GuitarixEditor* ge);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuitarixEditor)
};
