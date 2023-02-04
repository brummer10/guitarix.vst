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

#include "JuceUiBuilder.h"
#include "PluginEditor.h"

using namespace juce;

PluginEditor *JuceUiBuilder::ed = 0;
int JuceUiBuilder::flags = 0;
bool JuceUiBuilder::inHide(false);
std::list<juce::FlexBox *> JuceUiBuilder::boxes;
std::list<std::pair<JuceUiBuilder::boxkey_t, juce::Point<int> > > JuceUiBuilder::boxstack;
std::list<juce::Component*> JuceUiBuilder::parents;
juce::Rectangle<int> *JuceUiBuilder::bounds;
juce::Slider *JuceUiBuilder::lastslider = 0;
juce::ToggleButton *JuceUiBuilder::lastbutton = 0;
juce::TextButton *JuceUiBuilder::lasttextbutton = 0;
juce::ComboBox *JuceUiBuilder::lastcombo = 0;
int edx, edy;

// icon to label wrapper
const char* get_label(const char *sw_type) {
    if (std::strcmp(sw_type, "pbutton") == 0) return ">";
    else if (std::strcmp(sw_type, "rbutton") == 0) return "Rec";
    else if (std::strcmp(sw_type, "prbutton") == 0) return "<";
    else if (std::strcmp(sw_type, "fbutton") == 0) return ">>";
    else if (std::strcmp(sw_type, "frbutton") == 0) return "<<";
    else if (std::strcmp(sw_type, "button") == 0) return "X";
    else if (std::strcmp(sw_type, "overdub") == 0) return "O";
    else return "";
}

JuceUiBuilder::JuceUiBuilder(PluginEditor *ed, PluginDef *pd, juce::Rectangle<int> *rect)
	: UiBuilder() {
	JuceUiBuilder::ed = ed;
	plugin = pd;
	flags = 0;
	bounds = rect;
	edx = bounds->getX();
	edy = bounds->getY();

	openTabBox = openTabBox_;
	openVerticalBox = openVerticalBox_;
	openVerticalBox1 = openVerticalBox1_;
	openVerticalBox2 = openVerticalBox2_;
	openHorizontalBox = openHorizontalBox_;
	openHorizontalhideBox = openHorizontalhideBox_;
	openHorizontalTableBox = openHorizontalTableBox_;
	openFrameBox = openFrameBox_;
	openFlipLabelBox = openFlipLabelBox_;
	openpaintampBox = openpaintampBox_;
	closeBox = closeBox_;
	load_glade = load_glade_;
	load_glade_file = load_glade_file_;
	create_master_slider = create_master_slider_;
	create_feedback_slider = create_feedback_slider_;
	create_big_rackknob = create_big_rackknob_;
	create_mid_rackknob = create_mid_rackknob_;
	create_small_rackknob = create_small_rackknob_;
	create_small_rackknobr = create_small_rackknobr_;
	create_simple_meter = create_simple_meter_;
	create_simple_c_meter = create_simple_c_meter_;
	create_spin_value = create_spin_value_;
	create_switch = create_switch_;
	create_wheel = create_wheel_;
	create_switch_no_caption = create_switch_no_caption_;
	create_feedback_switch = create_feedback_switch_;
	create_selector = create_selector_;
	create_selector_no_caption = create_selector_no_caption_;
	create_port_display = create_port_display_;
	create_p_display = create_p_display_;
	create_simple_spin_value = create_simple_spin_value_;
	create_eq_rackslider_no_caption = create_eq_rackslider_no_caption_;
	create_fload_switch = create_fload_switch_;
	insertSpacer = insertSpacer_;
	set_next_flags = set_next_flags_;

	boxes.clear();
	boxstack.clear();
	parents.clear(); parents.push_front(ed);

	//addbox(true);
}

JuceUiBuilder::~JuceUiBuilder() {
	//closebox();
	ed = 0;
	for (auto i = boxes.begin(); i != boxes.end(); ++i)
	{	
		delete *i;
	}
	boxes.clear();
	boxstack.clear();
	parents.clear();
}
/*
#define UI_NUM_TOP           0x01
#define UI_NUM_BOTTOM        0x03
#define UI_NUM_LEFT          0x05
#define UI_NUM_RIGHT         0x07
#define UI_NUM_POSITION_MASK 0x07
#define UI_NUM_SHOW_ALWAYS   0x08
#define UI_LABEL_INVERSE     0x02
*/


void JuceUiBuilder::create_slider(const char *id, const char *label, juce::Slider::SliderStyle style, int w, int h) {
	if (inHide) return;

	gx_engine::Parameter *p = ed->get_parameter(id);
	
	if (p == 0) 
		return;

	addbox(true, label);

	juce::Label *l = new juce::Label(p->name(), label);
	l->setFont(juce::Font().withPointHeight(texth / 2));
	int ww=juce::Font().withPointHeight(texth / 2).getStringWidth(label);
	if (ww < w) ww = w;

	l->setBounds(edx, edy, ww, texth);
	l->setJustificationType(juce::Justification::centred);

	additem(l);

	juce::Slider *s = new juce::Slider(label);

	s->setComponentID(id);
	s->setSliderStyle(style);
	s->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, texth);
	//s->setPopupDisplayEnabled(true, false, ed);
	//s->setTextValueSuffix(label);
	s->setBounds(edx+(ww-w)/2, edy + texth, w, h + texth);

	lastslider = s;
	s->setRange(p->getLowerAsFloat(), p->getUpperAsFloat(), p->getStepAsFloat());
	if (p->isFloat()) s->setValue(p->getFloat().get_value(), juce::dontSendNotification);
	else if (p->isInt()) s->setValue(p->getInt().get_value(), juce::dontSendNotification);
	//s->setTooltip(p->desc());

	s->addListener(ed);
	additem(s);

	edx += h + 2 * texth;
	closebox();
}

void JuceUiBuilder::create_f_slider(const char *id, const char *label, juce::Slider::SliderStyle style, int w, int h) {
	if (inHide) return;

	gx_engine::Parameter *p = ed->get_parameter(id);
	
	if (p == 0) 
		return;

	juce::Slider *s = new juce::Slider(label);

	s->setComponentID(id);
	s->setSliderStyle(style);
    s->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    s->setColour(juce::Slider::ColourIds::trackColourId, juce::Colour::fromRGBA(66, 162, 200, 255));
    s->snapValue(-70.0, juce::Slider::DragMode::notDragging);
	s->setBounds(0, 0, w, h);

	lastslider = s;
	s->setRange(p->getLowerAsFloat(), p->getUpperAsFloat(), p->getStepAsFloat());
	if (p->isFloat()) s->setValue(p->getFloat().get_value(), juce::dontSendNotification);
	else if (p->isInt()) s->setValue(p->getInt().get_value(), juce::dontSendNotification);
    ed->subscribe_timer(id);

	//s->addListener(ed);
	additem(s);
}

void JuceUiBuilder::create_f_button(const char *id, const char *label) {
	if (inHide) return;

	gx_engine::Parameter *p = ed->get_parameter(id);
	if (p == 0) 
		return;

	addbox(true, label);

	juced::PushButton *b = new juced::PushButton(id, label);

	b->setComponentID(id);
	b->setBounds(0, 0, 60, texth);
	//b->changeWidthToFitText();

	//lastbutton = b;

	if (p->isBool()) b->setToggleState(p->getBool().get_value(), juce::dontSendNotification);
	else if (p->isFloat()) b->setToggleState(p->getFloat().get_value() != 0, juce::dontSendNotification);
	else if (p->isInt()) b->setToggleState(p->getInt().get_value() != 0, juce::dontSendNotification);
    ed->subscribe_timer(id);

	b->addListener(ed);
	additem(b);
	closebox();
}

void JuceUiBuilder::create_text_button(const char *id, const char *label) {
	if (inHide) return;
    int w = 25, h = 2;
	gx_engine::Parameter *p = ed->get_parameter(id);
	
	if (p == 0) 
		return;

	addbox(true, label);

	juce::Label *l = new juce::Label(p->name(), label);
	l->setFont(juce::Font().withPointHeight(texth / 2));
	int ww=juce::Font().withPointHeight(texth / 2).getStringWidth(label);
	if (ww < w) ww = w;

	l->setBounds(edx, edy, ww, texth);
	l->setJustificationType(juce::Justification::centred);

	additem(l);

	juce::ToggleButton *b = new juce::ToggleButton("");

	b->setComponentID(id);
	b->setBounds(edx+(ww-w)/2, edy + texth, w, h + texth);

	lastbutton = b;

	if (p->isBool()) b->setToggleState(p->getBool().get_value(), juce::dontSendNotification);
	else if (p->isFloat()) b->setToggleState(p->getFloat().get_value() != 0, juce::dontSendNotification);
	else if (p->isInt()) b->setToggleState(p->getInt().get_value() != 0, juce::dontSendNotification);

	b->addListener(ed);
	additem(b);

	edx += h + 2 * texth;
	closebox();
}

void JuceUiBuilder::create_button(const char *id, const char *label) {
	if (inHide) return;

	gx_engine::Parameter *p = ed->get_parameter(id);

	juce::ToggleButton *b = new juce::ToggleButton(label);

	b->setComponentID(id);
	b->setBounds(0, 0, 60, texth);
	b->changeWidthToFitText();

	lastbutton = b;

	if (p->isBool()) b->setToggleState(p->getBool().get_value(), juce::dontSendNotification);
	else if (p->isFloat()) b->setToggleState(p->getFloat().get_value() != 0, juce::dontSendNotification);
	else if (p->isInt()) b->setToggleState(p->getInt().get_value() != 0, juce::dontSendNotification);

	b->addListener(ed);
	additem(b);
}

void JuceUiBuilder::create_combo(const char *id, const char *label) {
	if (inHide) return;
	juce::ComboBox *c = new juce::ComboBox();
	lastcombo = c;

	gx_engine::Parameter *p = ed->get_parameter(id);

    if(dynamic_cast<gx_engine::EnumParameter*>(p))
    {
        gx_engine::EnumParameter& e = p->getEnum();
        const value_pair *vn = e.getValueNames();
        if (!vn) return;
        for (int n = 0; ; ++n)
        {
            if (!vn[n].value_id) break;
            c->addItem(vn[n].value_label ? vn[n].value_label : vn[n].value_id, n + 1);
        }
        c->setSelectedId(e.get_value() + 1, juce::dontSendNotification);
    }
    else if(dynamic_cast<gx_engine::FloatEnumParameter*>(p))
    {
        gx_engine::FloatEnumParameter *e = dynamic_cast<gx_engine::FloatEnumParameter*>(p);
        const value_pair *vn = e->getValueNames();
        if (!vn) return;
        for (int n = 0; ; ++n)
        {
            if (!vn[n].value_id) break;
            c->addItem(vn[n].value_label ? vn[n].value_label : vn[n].value_id, n + 1);
        }
        c->setSelectedId(floor(e->get_value()- e->getLowerAsFloat() +0.5) + 1, juce::dontSendNotification);
    }
    else
    {
        DBG ("Failed to create ComboBox for non-EnumParameter id:" << id << " label:" << label);
        delete c;
        lastcombo=0;
        return;
    }

    
	c->setScrollWheelEnabled(true);
	//c->setTooltip(label);
	c->setBounds(edx, edy, 150, texth);
	edy += texth;
	c->setComponentID(id);
	c->addListener(ed);

	additem(c);
}

char* ir_combo_folders[3] = { const_cast<char*>("%S"), const_cast<char*>("%S/amps"), const_cast<char*>("%S/bands") };
const char* folder_names[3] = { "Impulse Responses","Amplifiers", "Bands" };

void JuceUiBuilder::create_ir_combo(const char *id, const char *label) {
	if (inHide) return;
	juce::ComboBox *c = new juce::ComboBox();
	lastcombo = c;

	gx_engine::Parameter *p = ed->get_parameter(id);
	//dynamic_cast<gx_engine::JConvParameter*>(*i)->get_value().getIRFile();

	if (!dynamic_cast<gx_engine::JConvParameter*>(p))
    {
        DBG ("Failed to create ComboBox for non-JConvParameter" << id << " label:" << label);
        delete c;
        lastcombo=0;
        return;
    }

	gx_engine::JConvParameter& e = *dynamic_cast<gx_engine::JConvParameter*>(p);
	const gx_engine::GxJConvSettings &j = e.get_value();

	std::string spath = j.getIRDir();
	std::string sname = j.getIRFile();
	
	int sel = 0;
	for (int f = 0; f < sizeof(ir_combo_folders) / sizeof(ir_combo_folders[0]); f++)
	{
		std::string path = ed->get_options().get_IR_prefixmap().replace_symbol(ir_combo_folders[f]);

		gx_system::IRFileListing l(path);
		int n = 1000*f;
		c->addSectionHeading(folder_names[f]);
		for (std::vector<gx_system::FileName>::iterator i = l.get_listing().begin(); i != l.get_listing().end(); ++i)
		{
			c->addItem(i->filename.c_str(), ++n);
			if ((path == spath || path.empty()) && i->filename == sname)
				sel = n;
		}
	}

	if(sel)
		c->setSelectedId(sel, juce::dontSendNotification);

	//c->setTooltip(label);
	c->setBounds(edx, edy, 240, texth);
	edy += texth;
	c->setComponentID(id);
	c->addListener(ed);

	additem(c);
}

void JuceUiBuilder::additem(juce::Component *c)
{
	ed->addControl(c, *parents.begin());

	if (boxstack.size())
	{
		auto parent = boxstack.begin();
		juce::FlexBox *b = parent->first.first;
		if (b)
		{
			b->items.add(juce::FlexItem(c->getWidth(), c->getHeight(), *c).withAlignSelf(juce::FlexItem::AlignSelf::center));
			updateparentsize(c->getWidth(), c->getHeight());
		}
		else
		{
			assert(false);
		}
	}
}

void JuceUiBuilder::addspacer()
{
	if (boxstack.size())
	{
		auto parent = boxstack.begin();
		juce::FlexBox *b = parent->first.first;
		if (b)
		{
			b->items.add(juce::FlexItem().withFlex(0.5f));
			updateparentsize(20, 20);
		}
		else
		{
			assert(false);
		}
	}
}

void JuceUiBuilder::updateparentsize(int w, int h)
{
	if (boxstack.size())
	{
		auto parent = boxstack.begin();
		juce::FlexBox *b = parent->first.first;
		juce::Point<int> &sz = parent->second;

		if (b)
		{
			if (b->flexDirection == juce::FlexBox::Direction::column)
			{
				sz.x = max(sz.x, w);
				sz.y += h;
			}
			else
			{
				sz.y = max(sz.y, h);
				sz.x += w;
			}
		}
		else
		{
			sz.x = max(sz.x, w);
			sz.y = max(sz.y, h);
		}
	}
}

void JuceUiBuilder::addbox(bool vert, const char* label)
{
	juce::FlexBox *b;
	if (vert)
		b = new juce::FlexBox(FlexBox::Direction::column, FlexBox::Wrap::noWrap, FlexBox::AlignContent::flexStart,
			FlexBox::AlignItems::center, FlexBox::JustifyContent::spaceBetween);
	else
		b = new juce::FlexBox(FlexBox::Direction::row, FlexBox::Wrap::noWrap, FlexBox::AlignContent::flexStart,
			FlexBox::AlignItems::center, FlexBox::JustifyContent::spaceBetween);

	if (boxstack.size())
	{
		auto h = boxstack.begin();
		if (h->first.second != 0) //TabbedComponent
		{
			auto t = h->first.second;
			t->addTab(label, juce::Colour(0x00ffffff), new juce::Component(label), true);
			parents.push_front(t->getTabContentComponent(t->getNumTabs()-1));
		}
	}

	boxes.push_back(b);
	boxstack.push_front(decltype(boxstack)::value_type(boxkey_t(b,0), juce::Point<int>()));
}

void JuceUiBuilder::closebox()
{
	auto parent = boxstack.begin();
	juce::FlexBox *b = parent->first.first;
	juce::TabbedComponent* t = parent->first.second;
	juce::Point<int> sz = parent->second;
	boxstack.pop_front();
	if (b)
	{
		updateparentsize(sz.x, sz.y);

		if (boxstack.size())
		{
			auto pparent = boxstack.begin();
			juce::FlexBox* p = pparent->first.first;
			juce::TabbedComponent* tp = pparent->first.second;
			if(p)
				p->items.add(juce::FlexItem(sz.x, sz.y, *b));
			else if (tp)
			{
				parents.pop_front();
				int w = knobh * max(3,tp->getNumTabs());
				if (pparent->second.x < w) pparent->second.x = w;

				juce::Rectangle<int> tbounds(0, 0, pparent->second.x, pparent->second.y);
				b->performLayout(tbounds);
			}
		}

		if (boxstack.size() == 0 && !inHide)
		{
			bounds->setWidth(sz.x);
			bounds->setHeight(sz.y);
			b->performLayout(*bounds);
		}
	}
	else if (t)
	{
		t->setBounds(0, 0, sz.x, sz.y);
		t->setBounds(0, 0, sz.x, sz.y+t->getTabbedButtonBar().getThickness());
		additem(t);
	}
}

void JuceUiBuilder::openTabBox_(const char* label) {
	//addbox(true);
	juce::TabbedComponent* t = new juce::TabbedComponent(TabbedButtonBar::Orientation::TabsAtTop);
	boxstack.push_front(decltype(boxstack)::value_type(boxkey_t(0, t), juce::Point<int>()));
}

void JuceUiBuilder::openVerticalBox_(const char* label) {
	addbox(true, label);
}

void JuceUiBuilder::openVerticalBox1_(const char* label) {
	addbox(true, label);
}

void JuceUiBuilder::openVerticalBox2_(const char* label) {
	addbox(false, label);
}

void JuceUiBuilder::openHorizontalhideBox_(const char* label) {
	inHide = true;
	addbox(false, label);
}

void JuceUiBuilder::openHorizontalTableBox_(const char* label) {
	addbox(false, label);
}

void JuceUiBuilder::openFrameBox_(const char* label) {
	addbox(false, label);
}

void JuceUiBuilder::openFlipLabelBox_(const char* label) {
	addbox(false, label);
}

void JuceUiBuilder::openpaintampBox_(const char* label) {
	addbox(false, label);
}

void JuceUiBuilder::openHorizontalBox_(const char* label) {
	addbox(false, label);
}

void JuceUiBuilder::insertSpacer_() {
	addspacer();
}

void JuceUiBuilder::set_next_flags_(int flags) {
	JuceUiBuilder::flags = flags;
}

void JuceUiBuilder::create_big_rackknob_(const char *id, const char *label) {
	create_slider(id, label, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, knobh + 40, knobh + 40);
}

void JuceUiBuilder::create_mid_rackknob_(const char *id, const char *label) {
	create_slider(id, label, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, knobh + 20, knobh + 20);
}

void JuceUiBuilder::create_small_rackknob_(const char *id, const char *label) {
	create_slider(id, label, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, knobh, knobh);
}

void JuceUiBuilder::create_small_rackknobr_(const char *id, const char *label) {
	create_slider(id, label, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, knobh, knobh);
}

void JuceUiBuilder::create_feedback_slider_(const char *id, const char *label) {
	create_slider(id, label, juce::Slider::SliderStyle::LinearHorizontal, 150, 20);
}

void JuceUiBuilder::create_master_slider_(const char *id, const char *label) {
	create_slider(id, label, juce::Slider::SliderStyle::LinearHorizontal, 150, 20);
}

void JuceUiBuilder::create_selector_no_caption_(const char *id) {
	create_combo(id, "");
}

void JuceUiBuilder::create_selector_(const char *id, const char *label) {
	create_combo(id, label);
}

void JuceUiBuilder::create_simple_meter_(const char *id) {
	create_f_slider(id, "", juce::Slider::LinearBarVertical, 15, 120);
}

void JuceUiBuilder::create_simple_c_meter_(const char *id, const char *idl, const char *label) {
	create_slider(idl, label, juce::Slider::SliderStyle::LinearVertical, 40, 100);
	create_f_slider(id, label, juce::Slider::LinearBarVertical, 5, 100);
}

void JuceUiBuilder::create_spin_value_(const char *id, const char *label) {
	create_slider(id, label, juce::Slider::SliderStyle::LinearVertical, 40, 100);
}

void JuceUiBuilder::create_switch_no_caption_(const char *sw_type, const char * id) {
	create_button(id, "");
}

void JuceUiBuilder::create_feedback_switch_(const char *sw_type, const char * id) {

    const char* label = get_label(sw_type);
    create_f_button(id, label);
}

void JuceUiBuilder::create_fload_switch_(const char *sw_type, const char * id, const char * idf) {
}

void JuceUiBuilder::create_switch_(const char *sw_type, const char * id, const char *label) {
	create_text_button(id, label);
}

void JuceUiBuilder::create_wheel_(const char * id, const char *label) {
}

void JuceUiBuilder::create_port_display_(const char *id, const char *label) {
}

void JuceUiBuilder::create_p_display_(const char *id, const char *idl, const char *idh) {
}

void JuceUiBuilder::create_simple_spin_value_(const char *id) {
}

void JuceUiBuilder::create_eq_rackslider_no_caption_(const char *id) {
}

void JuceUiBuilder::closeBox_() {
	closebox();
	inHide = false;
}

void JuceUiBuilder::load_glade_(const char *data) {
}

void JuceUiBuilder::load_glade_file_(const char *fname) {
}
