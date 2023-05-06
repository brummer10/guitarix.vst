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
#include <curl/curl.h>

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
    std::vector<std::string> clist;

    void get_host_menu_for_parameter(juce::AudioProcessorParameter* param);
    void getParameterContext(const char* id);
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
	void muteButtonContext(juce::ToggleButton *b, const char* id);
    void presetFileMenuContext();
	void addButtonClicked(PluginEditor *ped, bool stereo);
	void removeButtonClicked(PluginEditor *ped, bool stereo);
	void SetAlternateDouble(bool alternateDouble) {mAlternateDouble = alternateDouble;}
	void on_param_value_changed(gx_engine::Parameter *p);
	void on_rack_unit_changed(bool stereo);
    bool plugin_in_use(const char* id);
    void addTunerEditor();
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
    void reorder_by_post_pre(std::vector<std::string> *ol);
    bool compare_pos( const std::string& o1, const std::string& o2);
	//calls
	void get_visible_mono(std::list<gx_engine::Plugin*> &l);
	void get_visible_stereo(std::list<gx_engine::Plugin*> &l);

	//======================================================

	void buildPluginCombo(juce::ComboBox *c, std::list<gx_engine::Plugin*> &lv, const char* selid);
	
	juce::ConcertinaPanel cp;

	void addEditor(int idx, PluginSelector *ps, PluginEditor *pe, const char* name);
    bool tunerIsVisible;
	std::list<ParListener*> editors;
	PluginEditor inputEditor;
	PluginEditor* tunerEditor;
	GuitarixProcessor& audioProcessor;
    
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

class PresetSelect: public juce::ComboBox
{
public:
    PresetSelect(const char *label) : juce::ComboBox(label) {}
    std::function<void()> rightClick;

    void mouseUp(const juce::MouseEvent& e) override {
        if (e.mods.isRightButtonDown()) {
            rightClick();
            return;
        }
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetSelect)
};

//==============================================================================
class GuitarixEditor : public juce::AudioProcessorEditor, public juce::Button::Listener, public juce::MultiTimer
{
public:
	GuitarixEditor(GuitarixProcessor&);
	~GuitarixEditor() override;
    ladspa::LadspaPluginList ml;

    void timerCallback(int id) override;
    
    void paint(juce::Graphics&) override;
	void resized() override;

	void createPluginEditors(bool l=true, bool r=true, bool s=true);
	void updateModeButtons();
    void load_preset_list();

	bool GetAlternateDouble() const { return ed.GetAlternateDouble()/* || ed_r.GetAlternateDouble()*/; }

private:
	GuitarixProcessor& audioProcessor;

	MachineEditor ed, /*ed_r, */ed_s;
    
    gx_jack::GxJack *jack;
	gx_jack::GxJack *jack_r;
    gx_engine::GxMachine *machine;
    gx_preset::GxSettings *settings;

	juce::TextButton monoButton, stereoButton, aboutButton, pluginButton, tunerButton , onlineButton /*, singleButton, multiButton, mute1Button, mute2Button*/;
	void buttonClicked(juce::Button* b) override;
    bool tuner_on;

	PresetSelect presetFileMenu;
    HorizontalMeter meters[4];
    juce::Component topBox;
    gx_system::PresetFile* get_bank(const std::string& id);
    gx_system::PresetBanks* banks();
    gx_system::PresetFile* presets(const std::string& id);
    
    std::string new_bank;
    std::string new_preset;
    void on_preset_save();
    void on_preset_select();
    void on_online_preset();
    static void loadLV2PlugCallback(int i, GuitarixEditor* ge);
    void downloadPreset(std::string uri);
    void read_online_preset_menu();
    static void handleOnlineMenu(int choice, GuitarixEditor* ge);
    static void on_online_preset_select(int choice, GuitarixEditor* ge);
    void create_online_preset_menu();
    bool download_file(std::string from_uri, std::string to_path);
    std::vector< std::tuple<std::string,std::string,std::string> > olp;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuitarixEditor)
};
