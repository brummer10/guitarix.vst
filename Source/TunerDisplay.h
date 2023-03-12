/*
 * Copyright (C) 2023 Hermann Meyer
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


static const char *note_sharp[] = {"A","A#","B","C","C#","D","D#","E","F","F#","G","G#"};
static const char *note_flat[] = {"A","Bb","B","C","Db","D","Eb","E","F","Gb","G","Ab"};
static const char* note_19[19] = {"A","A♯","B♭","B","B♯","C","C♯","D♭","D","D♯","E♭","E","E♯","F","F♯","G♭","G","G♯","A♭"};
static const char* note_24[24] = {"A","A¼♯","A♯","A¾♯","B","B¼♯","C","C¼♯","C♯","C¾♯","D","D¼♯","D♯","D¾♯","E","E¼♯","F","F¼♯","F♯","F¾♯","G","G¼♯","G♯","G¾♯"};
static const char* note_31[31] = {"A","B♭♭","A♯","B♭","A♯♯","B","C♭","B♯","C ","D♭♭","C♯","D♭","C♯♯","D","E♭♭","D♯","E♭","D♯♯","E","F♭","E♯","F","G♭♭","F♯","G♭","F♯♯","G","A♭♭","G♯","A♭","G♯♯"};
static const char* note_41[41] = {"A","^A","vB♭","B♭","^B♭","vvB","vB","B","^B","vC","C","^C","^^C","vC♯","C♯","^C♯","vD","D","^D","vE♭","E♭","^E♭","vvE","vE","E","^E","vF","F","^F","^^F","vF♯","F♯","^F♯","vG","G","^G","vA♭","A♭","^A♭","vvA","vA"};
static const char* note_53[53] = {"la","laa","lo","law","ta","teh","te","tu","tuh","ti","tih","to","taw","da","do","di","daw","ro","rih","ra","ru","ruh","reh","re ","ri","raw","ma","meh","me","mu","muh","mi","maa","mo","maw","fe","fa","fih","fu","fuh","fi","se","suh","su","sih","sol","si","saw","lo","leh","le","lu","luh"};
static const char *octave[] = {"0","1","2","3","4","5"," "};

class TunerDisplay : public juce::Component, public sigc::trackable
{
public:
    TunerDisplay(gx_engine::GxMachine *machine_) : machine(machine_), font ("FreeMono", 20 , juce::Font::bold ) {
        
        freq_conn = machine->get_jack()->get_engine().tuner.signal_freq_changed().connect(
            sigc::mem_fun(this, &TunerDisplay::on_tuner_freq_changed));
        ref_freq_conn = machine->get_parameter("ui.tuner_reference_pitch").getFloat().signal_changed().connect(
            sigc::mem_fun(this, &TunerDisplay::on_ref_freq_changed));
        tunning_conn = machine->get_parameter("racktuner.temperament").getInt().signal_changed().connect(
            sigc::mem_fun(this, &TunerDisplay::on_tunning_changed));
        use_conn = machine->get_parameter("ui.racktuner").getBool().signal_changed().connect(
            sigc::mem_fun(this, &TunerDisplay::on_use_changed));
        ref_freq = machine->get_parameter_value<float>("ui.tuner_reference_pitch");
        tunning = machine->get_parameter_value<int>("racktuner.temperament");
        use = machine->get_parameter_value<bool>("ui.racktuner");
        //getLookAndFeel().setDefaultSansSerifTypefaceName("FreeMono");
    }
    virtual ~TunerDisplay() {
        if (freq_conn.connected()) freq_conn.disconnect();
        if (ref_freq_conn.connected()) ref_freq_conn.disconnect();
        if (tunning_conn.connected()) tunning_conn.disconnect();
        if (use_conn.connected()) use_conn.disconnect();
    }

	void paint(juce::Graphics& g) override
	{
		auto bounds = getLocalBounds().toFloat();
        //juce::Font font ("FreeMono", 20 , juce::Font::bold );
        g.setFont (font);
		g.setColour(juce::Colours::white.withBrightness(0.4f));
		g.fillRoundedRectangle(bounds, 4.f);

        float value = freq;
        float c = 0.3f;
        draw_triangle(g, bounds.getWidth()/3.0, bounds.getHeight()/2 , -30, 20, c );
        draw_triangle(g, bounds.getWidth()/1.5, bounds.getHeight()/2, 30, 20, c );
        if (value < 20.0f || !use) return;
        float scale = -0.5;

        float fvis = get_tuner_temperament() * (log2f(value/ref_freq) + 4);
        float fvisr = round(fvis);
        int vis = fvisr;
        int indicate_oc = round((fvisr + temp_adjust)/get_tuner_temperament());
        const int octsz = sizeof(octave) / sizeof(octave[0]);
        if (indicate_oc < 0 || indicate_oc >= octsz) {
            // just safety, should not happen with current parameters
            // (pitch tracker output 23 .. 999 Hz)
            indicate_oc = octsz - 1;
        }

        scale = (fvis-vis) / 4;
        vis = vis % get_tuner_temperament();
        if (vis < 0) {
            vis += get_tuner_temperament();
        }
        c = std::max(0.0,1.0-(std::fabs(scale)*6.0));
        g.setColour (juce::Colours::white.withAlpha (c));
        g.setFont(36);
        g.drawSingleLineText(juce::String::fromUTF8(get_note_set()[vis]), bounds.getWidth()*0.50, bounds.getHeight()-10,  juce::Justification::Flags::right);
        g.setFont(16);
        g.drawSingleLineText(juce::String(octave[indicate_oc]), bounds.getWidth()*0.52, bounds.getHeight()-8);
        g.setColour (juce::Colours::white.withAlpha (0.9f));
        g.drawSingleLineText((juce::String(freq, 2) + juce::String("Hz")), bounds.getWidth()-20, bounds.getHeight()-5,  juce::Justification::Flags::right);
        draw_triangle(g, bounds.getWidth()/3.0, bounds.getHeight()/2 , -30, 20, c );
        draw_triangle(g, std::max(bounds.getWidth()/3.0, bounds.getWidth()/3.5-(300*scale)), bounds.getHeight()/2 , -30, 20, c );
        draw_triangle(g, std::max(bounds.getWidth()/3.0, bounds.getWidth()/3.5-(600*scale)), bounds.getHeight()/2 , -30, 20, c );
        draw_triangle(g, bounds.getWidth()/1.5, bounds.getHeight()/2, 30, 20, c );
        draw_triangle(g, std::min(bounds.getWidth()/1.5, bounds.getWidth()/1.5 -(300*scale)), bounds.getHeight()/2, 30, 20, c );
        draw_triangle(g, std::min(bounds.getWidth()/1.5, bounds.getWidth()/1.5 -(600*scale)), bounds.getHeight()/2, 30, 20, c );
	}

private:
    gx_engine::GxMachine *machine;
    sigc::connection freq_conn;
    sigc::connection ref_freq_conn;
    sigc::connection tunning_conn;
    sigc::connection use_conn;
	float freq = 0.f;
    float ref_freq = 440.0f;
    int tunning = 9;
    int temp_adjust = 3;
    bool use = false;
    juce::Font font;

    void draw_triangle(juce::Graphics& g, int x, int y, int w, int h, float c) {
        if (c < 0.99) g.setColour(juce::Colour::fromRGBA(66*c, 162*c, 200*c, 188*c));
        else  g.setColour(juce::Colour::fromRGBA(66, 162, 200, 255));
        juce::Path triangle; 
        triangle.addTriangle(x, y, x + w, y+h, x + w, y - h); 
        g.fillPath(triangle);
    }

    void on_tuner_freq_changed() {
        freq = machine->get_tuner_freq();
        repaint();
    }

    void on_ref_freq_changed(float value) {
        ref_freq = value;
    }

    void on_tunning_changed(int value) {
        tunning = value;
        tuner_set_temp_adjust();
    }

    void on_use_changed(bool value) {
        use = value;
        repaint();
    }

    int get_tuner_temperament() {
        if(tunning == 0) return 12;
        else if(tunning == 1) return 19;
        else if(tunning == 2) return 24;
        else if(tunning == 3) return 31;
        else if(tunning == 4) return 41;
        else if(tunning == 5) return 53;
        else return 12;
    }

    void tuner_set_temp_adjust() {
        if(tunning == 0) temp_adjust = 3;
        else if(tunning == 1) temp_adjust =  6;
        else if(tunning == 2) temp_adjust =  7;
        else if(tunning == 3) temp_adjust =  9;
        else if(tunning == 4) temp_adjust =  11;
        else if(tunning == 5) temp_adjust =  15;
        else temp_adjust = 3;
    }

    const char **get_note_set() {
        if(tunning == 0) return note_sharp;
        else if(tunning == 1) return note_19;
        else if(tunning == 2) return note_24;
        else if(tunning == 3) return note_31;
        else if(tunning == 4) return note_41;
        else if(tunning == 5) return note_53;
        else return note_flat;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TunerDisplay)
};
