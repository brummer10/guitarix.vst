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

#include "PluginEditor.h"
#include "GuitarixEditor.h"

#include "JuceUiBuilder.h"

using namespace juce;

void cat2color(const char* cat, juce::Colour &col)
{
	if(strcmp(cat, "Tone Control")==0)
		{col = juce::Colour::fromRGBA(  0, 255,   0, 255); return;}
	else if(strcmp(cat, "Distortion")==0)
		{col = juce::Colour::fromRGBA(255,   0,   0, 255); return;}
	else if(strcmp(cat, "Fuzz")==0)
		{col = juce::Colour::fromRGBA(255, 255,   0, 255); return;}
	else if(strcmp(cat, "Reverb")==0)
		{col = juce::Colour::fromRGBA(  0, 255, 255, 255); return;}
	else if(strcmp(cat, "Echo / Delay")==0)
		{col = juce::Colour::fromRGBA(255,   0, 255, 255); return;}
	else if(strcmp(cat, "Modulation")==0)
		{col = juce::Colour::fromRGBA(255, 128,   0, 255); return;}
	else if(strcmp(cat, "Guitar Effects")==0)
		{col = juce::Colour::fromRGBA(128, 128, 255, 255); return;}
	else if(strcmp(cat, "Misc")==0)
		{col = juce::Colour::fromRGBA(  0,   0,   0, 255); return;}
	else if(strcmp(cat, "External")==0)
		{col = juce::Colour::fromRGBA( 128, 128, 255, 255); return;}
	col=juce::Colour::fromRGBA(255, 255, 255, 255);
}

//==============================================================================
PluginEditor::PluginEditor(MachineEditor* ed, const char* id, const char* cat, PluginSelector *ps) :
	ed(ed), pid(id), ps(ps), cat(cat)
{
	cat2color(cat, col);
	col = col.withAlpha((uint8)30);

	if (ps) ps->setEditor(this); 
}

void PluginEditor::recreate(const char *id, const char *cat, int edx, int edy, int &w, int &h)
{
	clear();

	pid = id;
	this->cat = cat;
	cat2color(cat, col);
	col = col.withAlpha((uint8)30);
	create(edx, edy, w, h);
	repaint();
}

void PluginEditor::create(int edx, int edy, int &w, int &h)
{
	if (pid.length() == 0)
	{
		w = edtw;
		h = 0;
		setBounds(edx, edy, w, h);
		return;
	}
	//ed->registerParListener(this);
	const char *id = pid.c_str();

	PluginDef *pd = 0;
	if (strncmp(id, "COMMON", 6) != 0)
		pd = ed->get_pdef(id);
	juce::Rectangle<int> rect/*(0, 0, 100, 100);*/ = ed->getLocalBounds();
	//rect.setTop(edy);
	JuceUiBuilder b(this, pd, &rect);
	if (pid == "COMMON-IN")
	{
		b.openHorizontalTableBox("");
		{
			//Noise Gate
			b.openHorizontalBox("Noise Gate");
			b.create_switch_no_caption(sw_led, "noise_gate.on_off");
			b.create_small_rackknob("noise_gate.threshold", N_("Noise Gate"));
			b.closeBox();

			//Compressor
			b.openHorizontalBox("Compressor");
			b.create_switch_no_caption(sw_led, "shaper.on_off");
			b.create_small_rackknob("shaper.sharper", N_("Compressor"));
			b.closeBox();

			//Mono Level Out
			b.openHorizontalBox("Mono Level Out");
			b.create_switch_no_caption(sw_led, "amp.on_off");
			b.create_small_rackknob("amp.out_amp", N_("Mono Level Out"));
			b.closeBox();

			//Clip
			b.openHorizontalBox("Clip");
			b.create_switch_no_caption(sw_led, "amp.clip.on_off");
			b.create_small_rackknob("amp.fuzz", N_("Clip"));
			b.closeBox();
		}
		b.closeBox();
	}
	else if (pid == "ampstack")
	{
		b.openHorizontalTableBox("");
		{
			b.openVerticalBox("");
			{
				//Pre Gain
				b.create_small_rackknob("amp2.stage1.Pregain", N_("Pre Gain"));
				//b.lastslider->setRange(-9, 0, 0.1);

				//Clean / Dist
				b.create_small_rackknob("gxdistortion.wet_dry", N_("Clean/Dist"));
				//b.lastslider->setRange(0, 13, 0.1);
			}
			b.closeBox();

			b.openVerticalBox("");
			{
				//Master Gain
				b.create_small_rackknob("amp2.stage2.gain1", N_("Mstr Gain"));

				//Drive
				b.create_small_rackknob("gxdistortion.drive", N_("Drive"));
				//b.lastslider->setRange(0.6, 1.0, 0.01);
			}
			b.closeBox();

			b.insertSpacer();

			b.openVerticalBox("");
			{
				//Tube
				b.create_selector_no_caption("tube.select");

				b.openHorizontalTableBox("");
				{
					//Bass Boost
					b.openVerticalBox("Bass Boost");
					b.create_switch_no_caption(sw_led, "amp.bass_boost.on_off");
					b.create_small_rackknob("bassbooster.Level", N_("Bass Boost"));
					//b.lastslider->setRange(0, 10, 0.1);
					b.closeBox();

					//Presence
					b.openVerticalBox("Presence");
					b.create_switch_no_caption(sw_led, "con.on_off");
					b.create_small_rackknob("con.Level", N_("Presence"));
					b.closeBox();

					//Reverb
					b.openVerticalBox("Reverb");
					b.create_switch_no_caption(sw_led, "amp.feed_on_off");
					b.create_small_rackknob("amp.wet_dry", N_("Reverb"));
					b.closeBox();
				}
				b.closeBox();
			}
			b.closeBox();

			//Master Volume
			b.create_big_rackknob("amp.out_master", N_("Master Volume"));
        b.openVerticalBox("");
        {
            b.create_switch(sw_led, "amp.highgain", "LowEnd");
            b.openVerticalBox("");
            b.insertSpacer();
            b.insertSpacer();
            b.insertSpacer();
            b.insertSpacer();
            b.closeBox();
            b.create_switch(sw_led, "antyfizz.on_off", "AntiFizz");
        }
        b.closeBox();
		}
		b.closeBox();
	}
	else if (pid == "COMMON-AMP")
	{
		b.openVerticalBox("");
		{
			//Tube
			b.create_selector_no_caption("tube.select");
			b.openHorizontalTableBox("");
			{
				//Pre Gain
				b.create_small_rackknob("amp2.stage1.Pregain", N_("Pre Gain"));
				//b.lastslider->setRange(-9, 0, 0.1);

				//Master Gain
				b.create_small_rackknob("amp2.stage2.gain1", N_("Mstr Gain"));
			}
			b.closeBox();

			b.openHorizontalTableBox("");
			{
				//Clean / Dist
				b.create_small_rackknob("gxdistortion.wet_dry", N_("Clean/Dist"));
				//b.lastslider->setRange(0, 13, 0.1);

				//Drive
				b.create_small_rackknob("gxdistortion.drive", N_("Drive"));
				//b.lastslider->setRange(0.6, 1.0, 0.01);
			}
			b.closeBox();
		}
		b.closeBox();
	}
	else if (pid == "COMMON-GEN")
	{
		b.openHorizontalTableBox("");
		{
			//Bass Boost
			b.openVerticalBox("Bass Boost");
			b.create_switch_no_caption(sw_led, "amp.bass_boost.on_off");
			b.create_small_rackknob("bassbooster.Level", N_("Bass Boost"));
			//b.lastslider->setRange(0, 10, 0.1);
			b.closeBox();

			//Presence
			b.openVerticalBox("Presence");
			b.create_switch_no_caption(sw_led, "con.on_off");
			b.create_small_rackknob("con.Level", N_("Presence"));
			b.closeBox();

			//Reverb
			b.openVerticalBox("Reverb");
			b.create_switch_no_caption(sw_led, "amp.feed_on_off");
			b.create_small_rackknob("amp.wet_dry", N_("Reverb"));
			b.closeBox();

			//Master Volume
			b.create_mid_rackknob("amp.out_master", N_("Master Volume"));
		}
		b.closeBox();
	}
	else if (pid == "bossds1")
	{
#define PARAM(p) ("bossds1" "." p)//DS1
		b.openHorizontalBox("");
		b.create_small_rackknobr(PARAM("drive"), "Distortion");
		//b.lastslider->setRange(0.3, 1.0, 0.01);
		b.create_small_rackknobr(PARAM("Tone"), "Tone");
		b.create_small_rackknobr(PARAM("Level"), "Level");
		b.closeBox();
#undef PARAM
	}
	else if (pid == "aclipper")//RAT
	{
#define PARAM(p) ("aclipper" "." p)
		b.openHorizontalBox("");
		b.insertSpacer();
		b.create_small_rackknobr(PARAM("drive"), "Distortion");
		b.create_small_rackknob(PARAM("tone"), "Tone");
		b.create_small_rackknob(PARAM("level"), "Volume");
		//b.lastslider->setRange(-20.0, 1.3, 0.1);
		b.insertSpacer();
		b.closeBox();
#undef PARAM
	}
	else if (pid == "mxrdis")//MXR Distortion Plus
	{
#define PARAM(p) ("mxrdis" "." p)
		b.openHorizontalBox("");
		b.create_small_rackknobr(PARAM("drive"), "Distortion");
		//b.lastslider->setRange(0.8, 1.0, 0.01);
		b.create_small_rackknobr(PARAM("Volume"), "Volume");
		//b.lastslider->setRange(0.0, 0.6, 0.01);
		b.closeBox();
#undef PARAM
	}
	else if (pid == "jconv_mono")//Convolver Mono
	{
#define PARAM(p) ("jconv_mono" "." p)
		b.openHorizontalBox("");
		b.create_mid_rackknob(PARAM("gain"), _("Gain"));
		b.create_small_rackknobr(PARAM("wet_dry"), _("Dry/Wet"));
		b.create_ir_combo(PARAM("convolver"), _("IR File"));
		b.closeBox();
#undef PARAM
	}
	else if (pid == "jconv")//Convolver Stereo
	{
#define PARAM(p) ("jconv" "." p)
		b.openHorizontalBox("");
		b.create_mid_rackknob(PARAM("gain"), _("Gain"));
		b.create_small_rackknobr(PARAM("diff_delay"), _("Delta Delay"));
		b.create_small_rackknobr(PARAM("balance"), _("Balance"));
		b.create_small_rackknobr(PARAM("wet_dry"), _("Dry/Wet"));
		b.create_ir_combo(PARAM("convolver"), _("IR File"));
		b.closeBox();
#undef PARAM
	}
	else if (!pd->load_ui) {
		rect.setWidth(0);
		rect.setHeight(0);
	}
	else {
		pd->load_ui(b, UI_FORM_STACK);
	}

	w = rect.getWidth(); 
	h = rect.getHeight()+2;
	setBounds(edx, edy, w, h);
	//ed->addAndMakeVisible(this);
}

gx_engine::Parameter* PluginEditor::get_parameter(const char* parid)
{
	return ed->get_parameter(parid);
}

void PluginEditor::addControl(juce::Component *c, juce::Component* parent)
{
	if (parent) parent->addAndMakeVisible(c); else addAndMakeVisible(c);
	edlist.push_back(c);
}

void PluginEditor::clear()
{
	//ed->unregisterParListener(this);

	for (auto i = edlist.begin(); i != edlist.end(); i++)
	{
		(*i)->getParentComponent()->removeChildComponent(*i);
		delete (*i);
	}
	edlist.clear();
}

void PluginEditor::subscribe_timer(std::string id)
{
    ed->clist.push_back(id);
}

void PluginEditor::getinfo(std::string &text)
{
	std::list<gx_engine::Parameter*> pars;
	ed->list(pid.c_str(), pars);
	text = "Plugin: ";
	text += pid;
	text += "\n";
	for (auto i = pars.begin(); i != pars.end(); i++)
	{
		std::ostringstream ss;
		gx_system::JsonWriter jw(&ss);
		(*i)->serializeJSON(jw);

		text += "\n";
		if ((*i)->name() != "") text += "\"" + (*i)->name() + "\" ";
		/*else */text += (*i)->id();
		text += ": ";
		if ((*i)->isFloat())
			text += std::to_string((*i)->getFloat().get_value());
		else if ((*i)->isInt())
			text += std::to_string((*i)->getInt().get_value());
		else if ((*i)->isBool())
			text += std::to_string((*i)->getBool().get_value());
		else if ((*i)->isString())
			text += (*i)->getString().get_value().raw();
		else if ((*i)->isFile())
			text += (*i)->getFile().get_path();
		else if (dynamic_cast<gx_engine::JConvParameter*>(*i) != 0)
			text += "<JConv> " + dynamic_cast<gx_engine::JConvParameter*>(*i)->get_value().getIRFile();
		else if (dynamic_cast<gx_engine::SeqParameter*>(*i) != 0)
			text += "<SeqParameter> ";
		else
			text += "UNKNOWN PARAMETER TYPE";// (*i)->get_typename();
	}
}

void PluginEditor::sliderValueChanged(juce::Slider* slider)
{
	gx_engine::ParamMap& param = ed->get_param();
	const Glib::ustring& attr = slider->getComponentID().toStdString();
	if (param.hasId(attr)) {
		gx_engine::Parameter& p = param[attr];
		ed->SetAlternateDouble(ModifierKeys::getCurrentModifiers().testFlags(ModifierKeys::shiftModifier));
		p.set_blocked(true);
		if (p.isFloat()) {
			p.getFloat().set(slider->getValue());
		}
		else if (p.isInt()) {
			p.getInt().set(slider->getValue());
		}
		p.set_blocked(false);
		ed->SetAlternateDouble(false);
		//save_state();
	}
}

void PluginEditor::buttonClicked(juce::Button* button)
{
	gx_engine::ParamMap& param = ed->get_param();
	const Glib::ustring& attr = button->getComponentID().toStdString();
	if (param.hasId(attr)) {
		gx_engine::Parameter& p = param[attr];
		ed->SetAlternateDouble(ModifierKeys::getCurrentModifiers().testFlags(ModifierKeys::shiftModifier));
		p.set_blocked(true);
		if (p.isFloat()) {
			p.getFloat().set(button->getToggleState() ? 1 : 0);
		}
		else if (p.isInt()) {
			p.getInt().set(button->getToggleState() ? 1 : 0);
		}
		else if (p.isBool()) {
			p.getBool().set(button->getToggleState());
		}
		p.set_blocked(false);
		ed->SetAlternateDouble(false);
		//save_state();
	}
}

void PluginEditor::comboBoxChanged(juce::ComboBox* combo)
{
	gx_engine::ParamMap& param = ed->get_param();
	const Glib::ustring& attr = combo->getComponentID().toStdString();
	if (param.hasId(attr)) {
		gx_engine::Parameter& p = param[attr];
		ed->SetAlternateDouble(ModifierKeys::getCurrentModifiers().testFlags(ModifierKeys::shiftModifier));
		p.set_blocked(true);
		
		if (dynamic_cast<gx_engine::JConvParameter*>(&p))
		{
			gx_engine::JConvParameter& e = *dynamic_cast<gx_engine::JConvParameter*>(&p);
			gx_engine::GxJConvSettings j = e.get_value();

			int f = combo->getSelectedId() / 1000;
			std::string path = ed->machine->get_options().get_IR_prefixmap().replace_symbol(ir_combo_folders[f]);

			juce::String fname = combo->getItemText(combo->getSelectedItemIndex());
			j.setFullIRPath(Glib::build_filename(path, fname.toStdString()));
			e.set(j);
		}
		else if (p.isInt())
			p.getInt().set(combo->getSelectedId() - 1);
        else if (p.isFloat())
            p.getFloat().set(combo->getSelectedId() - 1 + floor(p.getFloat().getLowerAsFloat()));

		/*
		if (!dynamic_cast<gx_engine::EnumParameter*>(&p)) return;
		gx_engine::EnumParameter& e = p.getEnum();
		const value_pair *vn = e.getValueNames();
		if (!vn) return;
		idx_from_id
		*/

		p.set_blocked(false);
		ed->SetAlternateDouble(false);
		//save_state();
	}
}

// replace findChildWithID() to find Components recursively when needed (tapbox).
juce::Component* PluginEditor::findChildByID(juce::Component* parent, const std::string parid)
{
    juce::Component *c = parent->findChildWithID(parid.c_str());
    if (c) return c;
    for (int i = 0; i < parent->getNumChildComponents(); ++i)
    {
        juce::Component* childComp = parent->getChildComponent(i);
        if (childComp->getNumChildComponents()) {
            c = findChildByID (childComp, parid);
            if (c) return c;
        }
    }
    return nullptr;
}

void PluginEditor::on_param_value_changed(gx_engine::Parameter *p)
{
	const std::string parid = p->id();
	juce::Component *c = findChildByID(this, parid.c_str());
    if (!c) return;
	if (dynamic_cast<gx_engine::JConvParameter*>(p) && dynamic_cast<juce::ComboBox*>(c))
	{
		gx_engine::JConvParameter& e = *dynamic_cast<gx_engine::JConvParameter*>(p);
		const gx_engine::GxJConvSettings &j = e.get_value();

		std::string spath = j.getIRDir();
		std::string sname = j.getIRFile();

		juce::ComboBox* combo = dynamic_cast<juce::ComboBox*>(c);

		int sel = 0;
		for (int f = 0; f < sizeof(ir_combo_folders) / sizeof(ir_combo_folders[0]); f++)
		{
			std::string path = ed->machine->get_options().get_IR_prefixmap().replace_symbol(ir_combo_folders[f]);
			if (path == spath)
			{
				int id = f * 1000+1;
				for(int idx=combo->indexOfItemId(id); idx<combo->getNumItems() && combo->getItemId(idx)<id+1000; idx++)
					if (combo->getItemText(idx).toStdString() == sname)
					{
						sel = combo->getItemId(idx);
						break;
					}
				break;
			}
		}

		if (sel)
			combo->setSelectedId(sel, juce::dontSendNotification);
	}
	else if (dynamic_cast<juce::Slider*>(c))
	{
		juce::Slider* s = dynamic_cast<juce::Slider*>(c);
		if (p->isFloat()) {
            if (parid.find(".v") != std::string::npos) { // slider is fastmeter
                s->setValue(20.*log10(p->getFloat().get_value()), juce::dontSendNotification);
            } else {
                s->setValue(p->getFloat().get_value(), juce::dontSendNotification);
            }
		}
		else if (p->isInt()) {
			s->setValue(p->getFloat().get_value(), juce::dontSendNotification);
		}
	}
	else if (dynamic_cast<juce::Button*>(c))
	{
		//juce::MessageManagerLock mmlock;
		juce::Button* b = dynamic_cast<juce::Button*>(c);
		if (p->isBool()) b->setToggleState(p->getBool().get_value(), juce::dontSendNotification);
		else if (p->isFloat()) b->setToggleState(p->getFloat().get_value() != 0, juce::dontSendNotification);
		else if (p->isInt()) b->setToggleState(p->getInt().get_value() != 0, juce::dontSendNotification);
	}
	else if (dynamic_cast<juce::ComboBox*>(c))
	{
		juce::ComboBox* cb = dynamic_cast<juce::ComboBox*>(c);
		if (p->isInt())
            cb->setSelectedId(p->getInt().get_value() + 1, juce::dontSendNotification);
        else if (p->isFloat())
            cb->setSelectedId(floor(p->getFloat().get_value() - p->getFloat().getLowerAsFloat() +0.5) + 1, juce::dontSendNotification);
	}
}

void PluginEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(col);
	
	/*
	g.setColour(juce::Colours::white);
	g.setFont(15.0f);
	juce::Rectangle<int> rect = getLocalBounds();
	g.drawFittedText(getName(), rect, Justification::bottomRight, 1);*/
}

gx_system::CmdlineOptions& PluginEditor::get_options() {
    return ed->machine->get_options();
}

//==================================================================================================
PluginSelector::PluginSelector(MachineEditor* ed, bool stereo, const char* id, const char* cat) :
	ed(ed),
	ped(0),
	stereo(stereo),
	pid(id),
	cat(cat),
	add("+"),
	remove("-")
{
	//ed->registerParListener(this);
	cat2color(cat, col);
	col=col.withAlpha((uint8)70);

	setBounds(0, 0, edtw, texth + 8);

	bool minus = false;
	if (strncmp(id, "COMMON", 6) != 0 && strcmp(id, "ampstack") != 0)
	{
		ed->fillPluginCombo(&combo, stereo, id);
		combo.onChange = [this] { pluginMenuChanged(); };
		combo.setBounds(texth + 8, 4, 250, texth);
		addAndMakeVisible(&combo);
		minus = true;
	}

	if (strncmp(id, "COMMON", 6) != 0)
	{
		mute.setBounds(4, 4, texth, texth);
		mute.setClickingTogglesState(true);
		ed->updateMuteButton(&mute, id);
		mute.onClick = [this] { muteButtonClicked(); };
		//mute.setComponentID(juce::String(id) + ".on_off");
		addAndMakeVisible(&mute);
	}

	add.setBounds(edtw - (minus ? 2 : 1)*(texth + 4), 4, texth, texth);
	add.onClick = [this] { addButtonClicked(); };
	addAndMakeVisible(&add);

	if (minus)
	{
		remove.setBounds(edtw - (texth + 4), 4, texth, texth);
		remove.onClick = [this] { removeButtonClicked(); };
		addAndMakeVisible(&remove);
	}
}

PluginSelector::~PluginSelector()
{
	//ed->unregisterParListener(this);
}

void PluginSelector::setID(const char* id, const char* cat)
{
	pid = id;
	this->cat = cat;
	cat2color(cat, col);
	col = col.withAlpha((uint8)70);
	ed->fillPluginCombo(&combo, stereo, id);
	ed->updateMuteButton(&mute, id);
	repaint();
}

void PluginSelector::pluginMenuChanged()
{
	ed->SetAlternateDouble(ModifierKeys::getCurrentModifiers().testFlags(ModifierKeys::shiftModifier));
	ed->pluginMenuChanged(ped, &combo, stereo);
	ed->SetAlternateDouble(false);
}

void PluginSelector::muteButtonClicked()
{
	ed->SetAlternateDouble(ModifierKeys::getCurrentModifiers().testFlags(ModifierKeys::shiftModifier));
	ed->muteButtonClicked(&mute, pid.c_str());
	ed->SetAlternateDouble(false);
}

void PluginSelector::addButtonClicked()
{
	ed->addButtonClicked(ped, stereo);
}

void PluginSelector::removeButtonClicked()
{
	ed->removeButtonClicked(ped, stereo);
}

void PluginSelector::on_param_value_changed(gx_engine::Parameter *p)
{
	const std::string parid = p->id();
	if(parid==pid+".on_off")
	{
		if (p->isBool()) mute.setToggleState(p->getBool().get_value(), juce::dontSendNotification);
		else if (p->isFloat()) mute.setToggleState(p->getFloat().get_value() != 0, juce::dontSendNotification);
		else if (p->isInt()) mute.setToggleState(p->getInt().get_value() != 0, juce::dontSendNotification);
	}
}

void PluginSelector::paint(juce::Graphics& g)
{
	g.fillAll(col);

	g.setColour(juce::Colours::white);
	g.setFont(15.0f);
	if (stereo)
	{
		juce::Rectangle<int> rect = getLocalBounds();
		rect.setX(texth + 8 + 250 + 4);
		g.drawFittedText("STEREO", rect, Justification::verticallyCentred | Justification::left, 1);
	}

	juce::Rectangle<int> rect = getLocalBounds();
	rect.setX(texth + 8);
	if (strncmp(pid.c_str(), "COMMON", 6) == 0)
		g.drawFittedText("INPUT", rect, Justification::verticallyCentred | Justification::left, 1);
	else if(pid == "ampstack")
		g.drawFittedText("AMP STACK", rect, Justification::verticallyCentred | Justification::left, 1);

    g.setColour(juce::Colour(0x7fffffff));
    rect = getLocalBounds();
    g.drawLine(0,1,rect.getWidth(),1);
    g.drawLine(0,rect.getHeight()-1,rect.getWidth(),getHeight()-1);
}
