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
#include "guitarix.h"

class PluginEditor;

class VerticalMeter : public juce::Component
{
public:
	void paint(juce::Graphics& g) override
	{
		auto bounds = getLocalBounds().toFloat();

		g.setColour(juce::Colours::white.withBrightness(0.4f));
		g.fillRoundedRectangle(bounds, 4.f);

		const auto scaledY = juce::jmap(level, -60.f, +6.f, 0.f, bounds.getHeight());
		const auto scaledCol = juce::jmap(level, -60.f, 0.f, 0.5f, 1.0f);
		g.setColour(juce::Colours::white.withBrightness(scaledCol));
		g.fillRoundedRectangle(bounds.removeFromTop(scaledY), 4.f);
	}

	void setLevel(float value) { level = value; }

private:
	float level = -60.f;
};


class JuceUiBuilder : public UiBuilder {
private:
	static void openTabBox_(const char* label);
	static void openVerticalBox_(const char* label);
	static void openVerticalBox1_(const char* label);
	static void openVerticalBox2_(const char* label);
	static void openHorizontalBox_(const char* label);
	static void openHorizontalhideBox_(const char* label);
	static void openHorizontalTableBox_(const char* label);
	static void openFrameBox_(const char* label);
	static void openFlipLabelBox_(const char* label);
	static void openpaintampBox_(const char* label);
	static void insertSpacer_();
	static void set_next_flags_(int flags);
	static void create_big_rackknob_(const char *id, const char *label);
	static void create_mid_rackknob_(const char *id, const char *label);
	static void create_small_rackknob_(const char *id, const char *label);
	static void create_small_rackknobr_(const char *id, const char *label);
	static void create_master_slider_(const char *id, const char *label);
	static void create_feedback_slider_(const char *id, const char *label);
	static void create_selector_no_caption_(const char *id);
	static void create_selector_(const char *id, const char *label);
	static void create_simple_meter_(const char *id);
	static void create_simple_c_meter_(const char *id, const char *idl, const char *label);
	static void create_spin_value_(const char *id, const char *label);
	static void create_switch_no_caption_(const char *sw_type, const char * id);
	static void create_feedback_switch_(const char *sw_type, const char * id);
	static void create_fload_switch_(const char *sw_type, const char * id, const char * idf);
	static void create_switch_(const char *sw_type, const char * id, const char *label);
	static void create_wheel_(const char * id, const char *label);
	static void create_port_display_(const char *id, const char *label);
	static void create_p_display_(const char *id, const char *idl, const char *idh);
	static void create_simple_spin_value_(const char *id);
	static void create_eq_rackslider_no_caption_(const char *id);
	static void closeBox_();
	static void load_glade_(const char *data);
	static void load_glade_file_(const char *fname);

	static void create_slider(const char *id, const char *label, juce::Slider::SliderStyle style, int w, int h);
	static void create_f_slider(const char *id, const char *label);
	static void create_combo(const char *id, const char *label);
	static void create_button(const char *id, const char *label);
	static void create_text_button(const char *id, const char *label);
    static void create_f_button(const char *id, const char *label);

	static void addbox(bool vertical, const char* label);
	static void closebox();
	static void additem(juce::Component *c);
	static void addspacer();
	static void updateparentsize(int w, int h);
	static std::list<juce::FlexBox *> boxes;
	static std::list<juce::Component*> parents;
	typedef std::pair<juce::FlexBox*, juce::TabbedComponent*> boxkey_t;
	static std::list<std::pair<boxkey_t, juce::Point<int> > > boxstack;

	static PluginEditor *ed;
	static int flags;
	static bool inHide;
	static juce::Rectangle<int> *bounds;

public:
	JuceUiBuilder(PluginEditor *ed, PluginDef *pd, juce::Rectangle<int> *rect);
	~JuceUiBuilder();

	static void create_ir_combo(const char *id, const char *label);

	static juce::Slider *lastslider;
	static juce::ToggleButton *lastbutton;
	static juce::TextButton *lasttextbutton;
	static juce::ComboBox *lastcombo;
};

enum { texth = 24, knobh = 60, edtw=500, winh = 734};
extern char* ir_combo_folders[3];
