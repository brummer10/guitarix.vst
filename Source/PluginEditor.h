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

namespace gx_engine { class Parameter; class GxMachine; }
namespace gx_system { class CmdlineOptions; }

//==============================================================================
class MachineEditor;
class PluginSelector;

class MuteButton : public juce::ToggleButton
{
public:
    MuteButton() : juce::ToggleButton() {}

    std::function<void()> rightClick;

    void mouseUp(const juce::MouseEvent& e) override {
        if (e.mods.isRightButtonDown()) {
            rightClick();
            return;
        }
        setToggleState(!getToggleState(), juce::sendNotification);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MuteButton)
};

class ParListener
{
public:
    virtual ~ParListener()=default;
	virtual void on_param_value_changed(gx_engine::Parameter *p)=0;
};

class PluginEditor: public juce::Component,
	public juce::Slider::Listener,
	public juce::Button::Listener,
	public juce::ComboBox::Listener,
	public ParListener
{
public:
	PluginEditor(MachineEditor* ed, const char* id, const char* cat, PluginSelector *ps = 0);
	virtual ~PluginEditor() { clear(); }

	//MachineEditor callbacks
	void create(int edx, int edy, int &w, int &h);
	void recreate(const char *id, const char *cat, int edx, int edy, int &w, int &h);
	void clear();
	void getinfo(std::string &text);
	PluginSelector* getPluginSelector() { return ps; }

	//JuceUiBuilder callbacks
	void addControl(juce::Component *c, juce::Component* parent = 0);
	gx_engine::Parameter* get_parameter(const char* parid);
    gx_engine::GxMachine *get_machine();

	const char* getID() const { return pid.c_str(); }
//	int edx, edy;

	//Engine callbacks
	void on_param_value_changed(gx_engine::Parameter *p) override;
    void subscribe_timer(std::string id);

    void getParameterContext(const char* id);
    gx_system::CmdlineOptions& get_options();

private:
	void sliderValueChanged(juce::Slider* slider) override;
	void buttonClicked(juce::Button* button) override;
	void comboBoxChanged(juce::ComboBox* combo) override;
    void open_file_browser(juce::Button* button, const std::string& id);
    bool is_factory_IR(const std::string& dir);
    void load_IR(const std::string& attr, juce::Button* button, juce::String fname);
    void set_ir_load_button_text(const std::string& attr, bool set);
    juce::File lastDirectory;
	void paint(juce::Graphics& g) override;
    juce::Component* findChildByID(juce::Component* parent, const std::string parid);
	std::list<juce::Component*> edlist;
	
	MachineEditor *ed;
	PluginSelector *ps;

	std::string pid;
	std::string cat;
	juce::Colour col;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

class PluginSelector : public juce::Component,
	public ParListener
{
public:
	PluginSelector(MachineEditor* ed, bool stereo, const char* id, const char* cat);
	virtual ~PluginSelector();

	void setEditor(PluginEditor *ped_) { ped = ped_; }
	void setID(const char* id, const char* cat);
	void on_param_value_changed(gx_engine::Parameter *p) override;

private:
	void pluginMenuChanged();
	void muteButtonClicked();
	void muteButtonContext();
	void addButtonClicked();
	void removeButtonClicked();

	void paint(juce::Graphics& g) override;

	MachineEditor *ed;
	PluginEditor *ped;
	juce::ComboBox combo;
	MuteButton mute;
	juce::TextButton add, remove;

	std::string pid;
	std::string cat;
	juce::Colour col;
	bool stereo;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginSelector)
};
