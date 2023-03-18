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


class TunerDisplay : public juce::Component, public sigc::trackable
{
public:
    TunerDisplay(gx_engine::GxMachine *machine_);
    virtual ~TunerDisplay();

    void paint(juce::Graphics& g) override;

private:
    gx_engine::GxMachine *machine;
    sigc::connection freq_conn;
    sigc::connection ref_freq_conn;
    sigc::connection tunning_conn;
    sigc::connection use_conn;
    float freq;
    float ref_freq;
    int tunning;
    int temp_adjust;
    bool use;
    juce::Font font;
    int move;
    int smove;

    void draw_triangle(juce::Graphics& g, int x, int y, int w, int h, float c);
    void on_tuner_freq_changed();
    void on_ref_freq_changed(float value);
    void on_tunning_changed(int value);
    void on_use_changed(bool value);
    int get_tuner_temperament();
    void tuner_set_temp_adjust();
    const char **get_note_set();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TunerDisplay)
};
