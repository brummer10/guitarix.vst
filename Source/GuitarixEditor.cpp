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

#include "GuitarixProcessor.h"
#include "GuitarixEditor.h"

#include "guitarix.h"
#include "gx_jack_wrapper.h"

#include "JuceUiBuilder.h"

#ifdef _WINDOWS
	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	#include <windows.h>
#endif

using namespace juce;

//==============================================================================
GuitarixEditor::GuitarixEditor(GuitarixProcessor& p)
	: AudioProcessorEditor(&p),
    audioProcessor(p),
    ed(p, false, MachineEditor::mn_Mono),
    //ed_r(p, true, MachineEditor::mn_Mono),
    ed_s(p, false, MachineEditor::mn_Stereo),
	monoButton("MONO"), stereoButton("STEREO"),
    pluginButton("LV2 plugs"), presetFileMenu(""),
    aboutButton("i"), tunerButton("TUNER"), onlineButton("Online"),
    topBox(),
    ml(),
    new_bank(""),
    new_preset("")
	//singleButton("SINGLE"), multiButton("DOUBLE"),
	//mute1Button("MONO 1"), mute2Button("MONO 2"),
{
	audioProcessor.set_editor(this);
    
    //mIsVSTPlugin=audioProcessor.wrapperType==juce::AudioProcessor::WrapperType::wrapperType_VST3;
    p.get_machine_jack(jack_r, machine, true);
    p.get_machine_jack(jack, machine, false);
    settings = &(machine->get_settings());
    tuner_on = machine->get_parameter_value<bool>("system.show_tuner");
    
    //getConstrainer()->setFixedAspectRatio((double)(edtw*2+2)/(winh+texth+8));
    //setResizeLimits(edtw+1, (winh+texth+8)/2,edtw*4+2,winh+texth*2+8);
    setResizable(true, false);
	setSize((edtw*2+2) * audioProcessor.scale, (winh+texth+8) * audioProcessor.scale);

    topBox.setComponentID("TopBox");
    topBox.setBounds(0, 0, edtw*2+2, winh+texth+8);
    addAndMakeVisible(topBox);

	aboutButton.setComponentID("ABOUT");
	aboutButton.setBounds(2 * edtw - 4 - texth, 4, texth, texth);
	//aboutButton.changeWidthToFitText();
	aboutButton.addListener(this);
	topBox.addAndMakeVisible(aboutButton);

    int left=0;
    
    meters[0].setBounds(left+4,4+3,100,texth/2-4);
    topBox.addAndMakeVisible(meters);
    meters[1].setBounds(left+4,4+texth/2+1,100,texth/2-4);
    topBox.addAndMakeVisible(meters[1]);
    left+=100+4;
    
    meters[2].setBounds(left+4,4+3,100,texth/2-4);
    topBox.addAndMakeVisible(meters[2]);
    meters[3].setBounds(left+4,4+texth/2+1,100,texth/2-4);
    topBox.addAndMakeVisible(meters[3]);
    left+=100+4;

    monoButton.setComponentID("MONO");
    monoButton.setBounds(left+4, 4, 20, texth);
    monoButton.changeWidthToFitText();
    monoButton.addListener(this);
    topBox.addAndMakeVisible(monoButton);

    stereoButton.setComponentID("STEREO");
    stereoButton.setBounds(monoButton.getRight()+4, 4, 20, texth);
    stereoButton.changeWidthToFitText();
    stereoButton.addListener(this);
    topBox.addAndMakeVisible(stereoButton);

    tunerButton.setComponentID("TUNER");
    tunerButton.setBounds(stereoButton.getRight()+4, 4, 20, texth);
    tunerButton.changeWidthToFitText();
    tunerButton.addListener(this);
    topBox.addAndMakeVisible(tunerButton);
/*
	singleButton.setComponentID("SINGLE");
	singleButton.setBounds(stereoButton.getRight()+12, 4, 20, texth);
	singleButton.changeWidthToFitText();
	singleButton.addListener(this);
	addAndMakeVisible(singleButton);

	multiButton.setComponentID("DOUBLE");
	multiButton.setBounds(singleButton.getRight(), 4, 20, texth);
	multiButton.changeWidthToFitText();
	multiButton.addListener(this);
	addAndMakeVisible(multiButton);

	mute1Button.setComponentID("MUTE1");
	mute1Button.setBounds(stereoButton.getRight() + 12, 4, 20, texth);
	mute1Button.changeWidthToFitText();
	mute1Button.addListener(this);
	addAndMakeVisible(mute1Button);

	mute2Button.setComponentID("MUTE2");
	mute2Button.setBounds(mute1Button.getRight(), 4, 20, texth);
	mute2Button.changeWidthToFitText();
	mute2Button.addListener(this);
	addAndMakeVisible(mute2Button);
*/
	updateModeButtons();
    load_preset_list();
    presetFileMenu.onChange = [this] { on_preset_select(); };
    presetFileMenu.rightClick = [this] { ed.presetFileMenuContext(); };
    presetFileMenu.setBounds(tunerButton.getRight() + 8, 4, 250, texth);
    topBox.addAndMakeVisible(&presetFileMenu);

	onlineButton.setComponentID("Online");
	onlineButton.setBounds(presetFileMenu.getRight() + 8, 4, 20, texth);
	onlineButton.changeWidthToFitText();
	onlineButton.addListener(this);
	topBox.addAndMakeVisible(onlineButton);

	pluginButton.setComponentID("LV2PLUGS");
	pluginButton.setBounds(onlineButton.getRight() + 8, 4, 20, texth);
	pluginButton.changeWidthToFitText();
	pluginButton.addListener(this);
	topBox.addAndMakeVisible(pluginButton);

	ed.setTopLeftPosition(0, texth+8); ed.setSize(edtw, winh);
	//ed_r.setTopLeftPosition(edtw, texth); ed_r.setSize(edtw, winh);
	ed_s.setTopLeftPosition(edtw+2, texth+8); ed_s.setSize(edtw, winh);
	topBox.addAndMakeVisible(ed);
	//addAndMakeVisible(ed_r);
	topBox.addAndMakeVisible(ed_s);
    
    startTimer(1, 42);
    startTimer(2, 200);
    /*ladspa::LadspaPluginList ml;
    std::vector<std::string>  old_not_found;
    machine->load_ladspalist(old_not_found, ml);
    for (auto v = ml.begin(); v != ml.end(); ++v) {
        fprintf(stderr, "%s\n", ((*v)->Name).c_str());
    }*/
}

GuitarixEditor::~GuitarixEditor()
{
	stopTimer(1);
    stopTimer(2);
    audioProcessor.set_editor(0);
}

void GuitarixEditor::timerCallback(int id)
{
    if (!audioProcessor.HasSampleRate()) return;
    if (id == 1) {
        auto rms=audioProcessor.getRMSValues();
        for(int i=0; i<4; i++) {
            meters[i].setLevel(rms[i].getCurrentValue());
            meters[i].repaint();
        }
        // monitor mono feedback controller
        for (auto i = ed.clist.begin(); i != ed.clist.end(); ++i) {
            std::string id = (*i);
            if (machine->parameter_hasId(id)) {
                if (machine->get_parameter_value<bool>(id.substr(0,id.find_last_of(".")+1)+"on_off")) {
                    ed.on_param_value_changed(ed.get_parameter(id.c_str()));
                }
            }  
        }
        // monitor stere feedback controller
        for (auto i = ed_s.clist.begin(); i != ed_s.clist.end(); ++i) {
            std::string id = (*i);
            if (machine->parameter_hasId(id)) {
                if (machine->get_parameter_value<bool>(id.substr(0,id.find_last_of(".")+1)+"on_off")) {
                    ed_s.on_param_value_changed(ed_s.get_parameter(id.c_str()));
                }
            }  
        }
    } else {
        bool stereo=audioProcessor.GetStereoMode();
        if (machine->get_parameter_value<bool>("cab.on_off")) {
            jack->get_engine().cabinet.pl_check_update();
            if (stereo) jack_r->get_engine().cabinet.pl_check_update();
        }
        if (machine->get_parameter_value<bool>("cab_st.on_off")) {
            jack->get_engine().cabinet_st.pl_check_update();
        }
        if (machine->get_parameter_value<bool>("pre.on_off")) {
            jack->get_engine().preamp.pl_check_update();
            if (stereo) jack_r->get_engine().preamp.pl_check_update();
        }
        if (machine->get_parameter_value<bool>("pre_st.on_off")) {
            jack->get_engine().preamp_st.pl_check_update();
        }
        if (machine->get_parameter_value<bool>("con.on_off")) {
            jack->get_engine().contrast.pl_check_update();
            if (stereo) jack_r->get_engine().contrast.pl_check_update();
        }
    }
}

void GuitarixEditor::updateModeButtons()
{
	bool stereo=audioProcessor.GetStereoMode(), multi=audioProcessor.GetMultiMode();
	bool mute1, mute2; audioProcessor.GetMonoMute(mute1, mute2);
    tuner_on = machine->get_parameter_value<bool>("system.show_tuner");

	monoButton.setToggleState(!stereo, juce::dontSendNotification);
	stereoButton.setToggleState(stereo, juce::dontSendNotification);
    tunerButton.setToggleState(tuner_on, juce::dontSendNotification);
    meters[1].setVisible(stereo);
/*	singleButton.setToggleState(!multi, juce::dontSendNotification);
	multiButton.setToggleState(multi, juce::dontSendNotification);
	mute1Button.setToggleState(!mute1, juce::dontSendNotification);
	mute2Button.setToggleState(!mute2, juce::dontSendNotification);
*/
}

void GuitarixEditor::createPluginEditors(bool l, bool r, bool s)
{
	if(l) ed.createPluginEditors();
//	if(r) ed_r.createPluginEditors();
	if(s) ed_s.createPluginEditors();
}

void GuitarixEditor::buttonClicked(juce::Button * b)
{
	if (b == &monoButton)
        {audioProcessor.SetStereoMode(false); updateModeButtons();}
	else if (b == &stereoButton)
        {audioProcessor.SetStereoMode(true); updateModeButtons();}
	else if (b == &tunerButton) {
        machine->set_parameter_value("system.show_tuner",!tuner_on);
        updateModeButtons();
        ed.addTunerEditor();
    }
    else if (b == &aboutButton)
    {
        char txt[]=
        "Guitarix virtual guitar amplifier VST3 port for Linux\n"
        "Version v"
        GXV
        "\n"
        "Portions (C) 2024 Hermann Meyer\n"
        "\n"
        "Guitarix.vst virtual guitar amplifier port for Mac/PC\n"
        "Portions (C) 2022 Maxim Alexanian\n"
        "\n"
        "VST is a trademark of Steinberg Media Technologies GmbH, registered in \n"
        "Europe and other countries.\n"
        "\n"
        "Guitarix virtual guitar amplifier for Linux\n"
        "Copyright (C) Hermann Meyer, James Warden, Andreas Degert, Pete Shorthose\n"
        "\n"
        "This program is free software: you can redistribute it and/or modify \n"
        "it under the terms of the GNU General Public License as published by \n"
        "the Free Software Foundation, either version 3 of the License, or \n"
        "(at your option) any later version.\n"
        "\n"
        "This program is distributed in the hope that it will be useful, \n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of \n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \n"
        "GNU General Public License for more details.\n"
        "\n"
        "You should have received a copy of the GNU General Public License \n"
        "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
        "\n"
        "For the source code for the Mac/PC port see \n"
        "<https://github.com/maximalexanian/guitarix-vst>\n"
        "\n"
        "For the source code for the Linux port see \n"
        "<https://github.com/brummer10/guitarix.vst>\n"
        "\n \n";

        juce::AlertWindow alertWindow("About Guitarix.vst",
                                                                     txt,
                                                                     AlertWindow::InfoIcon);
        alertWindow.addButton("Ok", 0);
        alertWindow.setUsingNativeTitleBar(true);
            
        alertWindow.runModalLoop();
    }
    else if (b == &onlineButton) {
        on_online_preset();
    }
    else if (b == &pluginButton) {
        PopupMenu menu;
        static int l = 0;
        if (!l) {
            std::vector<std::string>  old_not_found;
            machine->load_ladspalist(old_not_found, ml);
            l = 1;
        }
        int i = 1;
        for (auto v = ml.begin(); v != ml.end(); ++v) {
            if ((*v)->is_lv2) {
                bool enabled = (*v)->active;
                std::string s = (*v)->Name;
                menu.addItem (i, juce::String(s), true, enabled);
            }
            i++;
            //fprintf(stderr, "%s %s\n", ((*v)->Name).c_str(),((*v)->category.c_str()));
        }
        menu.showMenuAsync (PopupMenu::Options()
            .withTargetComponent(&pluginButton)
            .withMaximumNumColumns(1),
             ModalCallbackFunction::forComponent (loadLV2PlugCallback, this));
    }
/*	else if (b == &singleButton)
		audioProcessor.SetMultiMode(false);
	else if (b == &multiButton)
		audioProcessor.SetMultiMode(true);
	else if (b == &mute1Button || b == &mute2Button)
	{
		bool mute1, mute2; audioProcessor.GetMonoMute(mute1, mute2);
		if (b == &mute1Button) mute1 = !mute1;
		else mute2 = !mute2;
		audioProcessor.SetMonoMute(mute1, mute2);
	}*/

	updateModeButtons();
}

void GuitarixEditor::loadLV2PlugCallback(int i, GuitarixEditor* ge)
{
    if (!i) return;
    std::vector<ladspa::PluginDesc*>::iterator p = std::next(ge->ml.begin(),i-1);
    if (!(*p)->active) {
        (*p)->active_set = (*p)->active = true;
    } else  {
        std::string id_str = "lv2_" + gx_system::encode_filename((*p)->path);
        if (!ge->ed.plugin_in_use(id_str.c_str())) {
            (*p)->active_set = (*p)->active = false;
        } else {
            juce::AlertWindow::showAsync (MessageBoxOptions()
                                  .withIconType (MessageBoxIconType::InfoIcon)
                                  .withTitle ("Guitarix Info")
                                  .withMessage ("Can't remove plugin while it is in use!")
                                  .withButton ("OK"),
                                nullptr);
        }
    }
    ge->audioProcessor.update_plugin_list((*p)->active);
    ge->ed.on_rack_unit_changed(false);
    ge->ed_s.on_rack_unit_changed(true);
}

void GuitarixEditor::load_preset_list()
{
    presetFileMenu.clear(dontSendNotification);
    PopupMenu* pr = presetFileMenu.getRootMenu();
    std::string bank;
    std::string preset;
    if (settings->setting_is_preset()) {
        bank = settings->get_current_bank();
        preset = settings->get_current_name();
    } else {
        bank = "";
        preset = "";
    }
    gx_system::PresetBanks* bb = banks();
    int bi = 0, sel = 0;
    if (bb)
        for (auto b = bb->begin(); b != bb->end(); ++b) {
            gx_system::PresetFile* pp = presets(b->get_name());
            int pi = 0;
            int in_factory = false;
            if (pp) {
                PopupMenu sub;
                for (auto p = pp->begin(); p != pp->end(); ++p) {
                    int idx = bi * 1000 + (pi++) + 1;
                    sub.addItem(idx, p->name.raw());
                    if (b->get_name().raw() == bank && p->name.raw() == preset) {
                        sel = idx;
                        new_bank = bank;
                        new_preset = preset;
                    }
                }
                if (!in_factory) {
                    int idx = bi * 1000 + (pi++) + 1;
                    sub.addItem(idx, "<New>");
                    bi++;
                }
                if (!in_factory && pp->get_type() == gx_system::PresetFile::PRESET_FACTORY) {
                    in_factory = true;
                    pr->addSubMenu(b->get_name().raw() + " - Factory Presets", sub);
                    //presetFileMenu.addSectionHeading(b->get_name().raw() + " - Factory Presets");
                } else {
                    pr->addSubMenu(b->get_name().raw(), sub);
                    //presetFileMenu.addSectionHeading(b->get_name().raw());
                }
            }
        }

    if (sel > 0)
        presetFileMenu.setSelectedId(sel, juce::dontSendNotification);
}

void GuitarixEditor::on_preset_save()
{
    juce::AlertWindow *w = new juce::AlertWindow("Save Preset as", "", juce::AlertWindow::NoIcon);
    w->addTextEditor("bank", new_bank, "Enter Bank Name", false);
    w->addTextEditor("preset", "", "Enter Preset Name", false);
    w->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
    w->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

    auto savePreset = ([&, w, this](int result) {
        if (result == 1) {
            auto pset = w->getTextEditorContents("preset");
            auto bank = w->getTextEditorContents("bank");
            if (bank.isNotEmpty()) {
                gx_system::PresetBanks* bb = banks();
                bool need_new = true;
                for (auto b = bb->begin(); b != bb->end(); ++b) {
                    if ((bank.toStdString().compare(b->get_name().raw()) == 0) &&
                         b->get_type() != gx_system::PresetFile::PRESET_FACTORY){
                        need_new = false;
                        break;
                    }
                }
                if (need_new) {
                    machine->bank_insert_new(bank.toStdString());
                }
            }
            if (pset.isNotEmpty() && bank.isNotEmpty()) {
                this->audioProcessor.save_preset(bank.toStdString(), pset.toStdString());
                this->load_preset_list();
            }
        }
    });

    auto callback = juce::ModalCallbackFunction::create(savePreset);
    w->enterModalState(true, callback, true);
}

void GuitarixEditor::on_preset_select()
{
    gx_system::PresetBanks* bb = banks();
    int bi = 0, sel = 0, ad = 0;
    new_bank.clear();
    new_preset.clear();
    if (!presetFileMenu.getText().compare("<New>")) {
        ad = 1;
    }
    if (bb)
        for (auto b = bb->begin(); b != bb->end(); ++b) {
            gx_system::PresetFile* pp = presets(b->get_name());
            int pi = 0;
            if (pp)
            for (auto p = pp->begin(); p != pp->end()+ad; ++p) {
                int idx = bi * 1000 + (pi++) + 1;
                if (idx == presetFileMenu.getSelectedId()) {
                    new_bank = b->get_name().raw();
                    if (ad == 0)
                        new_preset = p->name.raw();
                }
            }
        bi++;
    }
    if (!new_bank.empty() && !new_preset.empty())
        audioProcessor.load_preset(new_bank, new_preset);
    else on_preset_save();
}

void GuitarixEditor::read_online_preset_menu() {
    ifstream is(audioProcessor.get_options()->get_online_config_filename());
    olp.clear();
    gx_system::JsonParser jp(&is);
    try {
	jp.next(gx_system::JsonParser::begin_array);
	do {
	    std::string NAME_;
	    std::string FILE_;
	    std::string INFO_;
	    std::string AUTHOR_;
	    jp.next(gx_system::JsonParser::begin_object);
	    do {
		jp.next(gx_system::JsonParser::value_key);
		if (jp.current_value() == "name") {
		    jp.read_kv("name", NAME_);
		} else if (jp.current_value() == "description") {
		    jp.read_kv("description", INFO_);
		} else if (jp.current_value() == "author") {
		    jp.read_kv("author", AUTHOR_);
		} else if (jp.current_value() == "file") {
		    jp.read_kv("file", FILE_);
		} else {
		    jp.skip_object();
		}
	    } while (jp.peek() == gx_system::JsonParser::value_key);
	    jp.next(gx_system::JsonParser::end_object);
	    INFO_ += "Author : " + AUTHOR_;
	    olp.push_back(std::tuple<std::string,std::string,std::string>(NAME_,FILE_,INFO_));
	} while (jp.peek() == gx_system::JsonParser::begin_object);
    } catch (gx_system::JsonException& e) {
	cerr << "JsonException: " << e.what() << ": '" << jp.current_value() << "'" << endl;
	assert(false);
    }
}

void GuitarixEditor::downloadPreset(std::string uri) {

    std::string::size_type n = uri.find_last_of('/');
    if (n != std::string::npos) {
        std::string fn = uri.substr(n);
        std::string ff = "/tmp"+fn;

        if (download_file(uri, ff)) {
            machine->bank_insert_uri(Glib::filename_to_uri(ff, "localhost"), false, 0);
            machine->bank_check_reparse();
            load_preset_list();
        }
    }
}

void GuitarixEditor::handleOnlineMenu(int choice, GuitarixEditor* ge){
    if (choice > 0) {
        std::vector<std::tuple<std::string,std::string,std::string> >::iterator it = ge->olp.begin()+choice -1;
        //fprintf(stderr, "%i %s \n",choice, get<1>(*it).c_str());
        ge->downloadPreset(get<1>(*it));
    }
}

void GuitarixEditor::on_online_preset_select(int choice, GuitarixEditor* ge)
{
    if (choice > 0) {
        std::vector<std::tuple<std::string,std::string,std::string> >::iterator it = ge->olp.begin()+choice -1;
        juce::AlertWindow *w = new juce::AlertWindow("Download Online Preset", "", juce::AlertWindow::NoIcon);
        juce::String m = get<2>(*it);
        int a = m.indexOf("https");
        int o = m.indexOf(a, "\n");
        juce::HyperlinkButton* button = nullptr;
        if (a>0 && o>0) {
            juce::String n = m.substring(a,o);
            juce::String message = m.substring(0, a-1);
            juce::String message2 = m.substring(o+1);
            w->setMessage(message);
            if (n.isNotEmpty ()) {
                button = new juce::HyperlinkButton(n, URL(n));
                button->setBounds(0, 0, 400, 25);
                button->setName("");
                w->addCustomComponent(button);
            }
            w->addTextBlock(message2);
            
        } else {
            w->setMessage(m);
        }
        w->addButton("Download", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
        w->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

        auto checkPresets = ([&, w, button, choice, ge](int result) {
            w->removeCustomComponent(w->getNumCustomComponents()-1);
            if (button) delete button;
            if (result == 1) {
                handleOnlineMenu(choice, ge);
            }
        });

        auto callback = juce::ModalCallbackFunction::create(checkPresets);
        w->enterModalState(true, callback, true);
    }
}

void GuitarixEditor::create_online_preset_menu() {

    static bool read_new = true;
    if (read_new) {
        read_online_preset_menu();
        read_new = false;
    }

    juce::PopupMenu menu;
    int i = 1;
    for(std::vector<std::tuple<std::string,std::string,std::string> >::iterator it = olp.begin(); it != olp.end(); it++) {
        menu.addItem(i, juce::String(get<0>(*it)));
        i++;
    }

    menu.showMenuAsync (PopupMenu::Options()
        .withTargetComponent(&onlineButton)
        .withMaximumNumColumns(1),
         ModalCallbackFunction::forComponent (on_online_preset_select, this));
}

bool GuitarixEditor::download_file(std::string from_uri, std::string to_path) {

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();
    CURLcode res;
    FILE *out;
    out = fopen(to_path.c_str(), "wb");

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
    curl_easy_setopt(curl, CURLOPT_URL, from_uri.c_str());
    res = curl_easy_perform(curl);
    if(CURLE_OK == res) {
        char *ct = NULL;
        res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
        if (strstr(ct, "application/json")!= NULL ) {
            res = CURLE_OK;
        } else if (strstr(ct, "application/octet-stream")!= NULL) {
            res = CURLE_OK;
        } else {
            res = CURLE_CONV_FAILED;
        }
    }
    curl_easy_reset(curl);
    fclose(out);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    if(res != CURLE_OK) {
        remove(to_path.c_str());
        //gx_print_error( "download_file", Glib::ustring::compose("curl_easy_perform() failed: %1", curl_easy_strerror(res)));
        return false;
    }
    return true;
}

void GuitarixEditor::on_online_preset()
{
    static bool read_new = true;
    if (read_new) {
        read_new = false;
        juce::AlertWindow *w = new juce::AlertWindow("Download Online Preset List", "", juce::AlertWindow::NoIcon);
        w->setMessage("Check for new online Presets?");
        w->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
        w->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

        auto checkPresets = ([&, w, this](int result) {
            if (result == 1) {
                download_file("https://musical-artifacts.com/artifacts.json?apps=guitarix", audioProcessor.get_options()->get_online_config_filename());
            }
            create_online_preset_menu();
        });

        auto callback = juce::ModalCallbackFunction::create(checkPresets);
        w->enterModalState(true, callback, true);
    } else {
        create_online_preset_menu();
    }
}

void GuitarixEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	/*
	g.setColour (juce::Colours::white);
	g.setFont (15.0f);
	juce::Rectangle<int> rect = getLocalBounds();
	rect.expand(0, -20);
	g.drawFittedText(text, rect, Justification::bottomRight, 1);*/
}

void GuitarixEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
    auto area = getLocalBounds().toFloat();
	double scale_x = static_cast<double>(area.getWidth() / (edtw*2+2));
	double scale_y = static_cast<double>(area.getHeight() / (winh+texth+8));
    audioProcessor.scale = std::max(0.5,std::min(2.5,scale_x < scale_y ? scale_x : scale_y));
	topBox.setTransform(AffineTransform::scale(audioProcessor.scale));
}


gx_system::PresetFile* GuitarixEditor::get_bank(const std::string& id) {
	return settings->banks.get_file(id);
}

gx_system::PresetBanks* GuitarixEditor::banks() {
	return &settings->banks;
}

gx_system::PresetFile* GuitarixEditor::presets(const std::string& id) {
	return settings->banks.get_file(id);
}


//==============================================================================
MachineEditor::MachineEditor(GuitarixProcessor& p, bool right, MonoT mono) :
	inputEditor(this, "COMMON-IN", ""),
	mIgnoreRackUnitChange(false),
	mRight(right),
	mMono(mono),
	mAlternateDouble(false),
    tunerIsVisible(false),
    audioProcessor(p)
{
	p.get_machine_jack(jack, machine, right);
	settings = &(machine->get_settings());

	gx_engine::ParamMap& pmap = settings->get_param();
	pmap.signal_insert_remove().connect(
		sigc::mem_fun(*this, &MachineEditor::on_param_insert_remove));
	for (gx_engine::ParamMap::iterator i = pmap.begin(); i != pmap.end(); ++i) {
		connect_value_changed_signal(i->second);
	}

	//settings->signal_rack_unit_order_changed().connect(
	//	sigc::mem_fun(this, &MachineEditor::on_rack_unit_changed));

	createPluginEditors();
}

MachineEditor::~MachineEditor()
{
    //for (int i = cp.getNumPanels() - 1; i >= 0; i--)
    //    cp.removePanel(cp.getPanel(i));
    editors.clear();
}

bool MachineEditor::plugin_in_use(const char* id) {
    gx_engine::Plugin* pl = jack->get_engine().pluginlist.find_plugin(id);
    if (!pl) return false;
    if (!pl->get_box_visible()) return false;
    return true;
}

PluginDef* MachineEditor::get_pdef(const char *id)
{
	gx_engine::Plugin* p = jack->get_engine().pluginlist.lookup_plugin(id);
	if (p)
		return p->get_pdef();
	else
		return 0;
}

void MachineEditor::registerParListener(ParListener *ed)
{
	auto f=std::find(editors.begin(), editors.end(), ed);
	if (f == editors.end())
		editors.push_back(ed);
}

void MachineEditor::unregisterParListener(ParListener *ed)
{
	auto f = std::find(editors.begin(), editors.end(), ed);
	if (f != editors.end())
		editors.erase(f);
}

gx_engine::ParamMap& MachineEditor::get_param()
{
	return settings->get_param();
}

void MachineEditor::buildPluginCombo(juce::ComboBox *c, std::list<gx_engine::Plugin*> &lv, const char* selid)
{
	const char* categories[] = { "Tone Control","Distortion","Fuzz","Reverb","Echo / Delay","Modulation","Guitar Effects","Misc" ,"External"};
	int cl = sizeof(categories) / sizeof(categories[0]);
    PopupMenu* pl = c->getRootMenu();
	int sel = 0;
	for (int ci = 0; ci < cl; ci++)
	{
		//bool createHeading = true;
        PopupMenu sub;
		int id = 1;
		for (auto v = lv.begin(); v != lv.end(); v++, id++)
		{
			auto pd = (*v)->get_pdef();
			const char* cat = pd->category;
			if (cat && strcmp(cat, categories[ci]) == 0)
			{
				std::string s = pd->id;
				std::string uid = "ui." + s;
				gx_engine::ParamMap& pmap = settings->get_param();
				if (pmap.hasId(uid))
				{
					/*if (createHeading)
					{
						c->addSectionHeading(categories[ci]);
                        
						createHeading = false;
					}*/
					const char* n = pd->name;
					sub.addItem(id, n);
					if (strcmp(pd->id, selid) == 0)
						sel = id;
				}
				else
					;
			}
		}
        pl->addSubMenu(categories[ci], sub);
	}
	
	if (sel > 0)
		c->setSelectedId(sel,dontSendNotification );
}

static bool plugin_order(gx_engine::Plugin* p1, gx_engine::Plugin* p2) {
	return strcmp(p1->get_pdef()->name, p2->get_pdef()->name)<0;
}

void MachineEditor::fillPluginCombo(juce::ComboBox *c, bool stereo, const char* id)
{
	c->clear(NotificationType::dontSendNotification);

	std::list<gx_engine::Plugin*> lv;
	if (stereo) get_visible_stereo(lv); else get_visible_mono(lv);
	lv.sort(plugin_order);
	buildPluginCombo(c, lv, id);
}

void MachineEditor::addEditor(int idx, PluginSelector *ps, PluginEditor *pe, const char* name)
{
	int w, h;
	pe->create(0, 0, w, h);
	pe->setName(name);
	cp.addPanel(idx, pe, true);
	cp.setPanelHeaderSize(pe, texth + 8);
	cp.setCustomPanelHeader(pe, ps, true);
	cp.setMaximumPanelSize(pe, h);
    //Desktop::getInstance().getAnimator().fadeOut(pe, 1);
    //Desktop::getInstance().getAnimator().fadeIn(pe, 800);
	registerParListener(pe);
	registerParListener(ps);
}

void MachineEditor::addTunerEditor()
{
    if (machine->get_parameter_value<bool>("system.show_tuner") ) {
        if (!tunerIsVisible) {
            PluginSelector *ps = new PluginSelector(this, false, "tuner", "none");
            tunerEditor = new PluginEditor(this, "tuner", "none", ps);
            addEditor(0, ps, tunerEditor, "Tuner");
            cp.expandPanelFully(tunerEditor, true);
            tunerIsVisible = true;
        }
    } else if (tunerIsVisible) {
        mIgnoreRackUnitChange = true;
        //remove_rack_unit(ped->getID(), stereo);
        mIgnoreRackUnitChange = false;
        unregisterParListener(tunerEditor->getPluginSelector());
        unregisterParListener(tunerEditor);
        cp.removePanel(tunerEditor);
        tunerEditor = NULL;
        tunerIsVisible = false;
    }
}

bool MachineEditor::compare_pos( const std::string& o1, const std::string& o2)
{
    gx_engine::Plugin *pl = jack->get_engine().pluginlist.find_plugin(o1);
    int pos1 = pl->get_effect_post_pre();
    gx_engine::Plugin *plu = jack->get_engine().pluginlist.find_plugin(o2);
    int pos2 = plu->get_effect_post_pre();
    if (pos1 == pos2) {
        pos1 = pl->get_position();
        pos2 = plu->get_position();
        return (pos1<pos2);
    }
    return (pos1>pos2);
}

void MachineEditor::reorder_by_post_pre(std::vector<std::string> *ol)
{
    std::sort(ol->begin(), ol->end(), [this](std::string& o1, std::string& o2) 
        { return this->compare_pos(o1, o2); });
}

void MachineEditor::createPluginEditors()
{
	editors.clear();
	for (int i = cp.getNumPanels() - 1; i >= 0; i--) {
        if (cp.getPanel(i) == tunerEditor) tunerIsVisible = false;;
		cp.removePanel(cp.getPanel(i));
    }
	cp.setBounds(0, 0, edtw, winh);
	inputEditor.clear();

	int w, h;
	if (mMono == mn_Mono || mMono == mn_Both)
	{
        addTunerEditor();

		inputEditor.create(0, 0, w, h);
		inputEditor.setName("Input");
		cp.addPanel(1, &inputEditor, false);
		cp.setPanelHeaderSize(&inputEditor, texth + 8);
		cp.setCustomPanelHeader(&inputEditor, new PluginSelector(this, false, inputEditor.getID(), ""), true);
		cp.setMaximumPanelSize(&inputEditor, h);
		registerParListener(&inputEditor);
	}

	int idx = 2;
	for (int stereo = (mMono==mn_Stereo ? 1 : 0); stereo <= (mMono == mn_Mono ? 0 : 1); stereo++)
	{
		std::vector<std::string> ol;
		ol=settings->get_rack_unit_order(stereo);
        if (!stereo) reorder_by_post_pre(&ol);

		std::list<gx_engine::Plugin*> lv;
		if (stereo) get_visible_stereo(lv); else get_visible_mono(lv);
		lv.sort(plugin_order);

		for (auto oli = ol.begin(); oli != ol.end(); oli++) {
			for (auto a = lv.begin(); a != lv.end(); a++)
			{
				if (*oli == (*a)->get_pdef()->id /*&& (*oli!="ampstack")*/)
				{
					const char* id = (*a)->get_pdef()->id;
					const char* cat = (*a)->get_pdef()->category;
					PluginSelector *ps = new PluginSelector(this, stereo, id, cat);
					PluginEditor *pe = new PluginEditor(this, id, cat, ps);
					addEditor(idx, ps, pe, (*a)->get_pdef()->name);
					idx++;
					break;
				}
			}
        }
	}

	if (mMono == mn_Stereo && idx == 2)
		addButtonClicked(0, true);

	addAndMakeVisible(cp);
}

void MachineEditor::updateMuteButton(juce::ToggleButton *b, const char* id)
{
	if (id[0] == 0)
	{
		b->setVisible(false);
		return;
	}

	b->setVisible(true);
    
    if (strcmp(id, "ui.racktuner") == 0) {
        b->setToggleState(machine->get_parameter_value<bool>("ui.racktuner"),dontSendNotification );
        machine->tuner_used_for_display(b->getToggleState());
        return;
    }

	gx_engine::Plugin *pl = jack->get_engine().pluginlist.find_plugin(id);
	if (!pl) return;

	gx_engine::Parameter *p;
	p = &settings->get_param()[pl->id_on_off()];
	bool on = pl->get_on_off();

	b->setToggleState(on,dontSendNotification );
}

void MachineEditor::muteButtonClicked(juce::ToggleButton *b, const char* id)
{
    if (strcmp(id, "ui.racktuner") == 0) {
        machine->set_parameter_value("ui.racktuner", b->getToggleState());
        machine->tuner_used_for_display(b->getToggleState());
        return;
    }
	gx_engine::Plugin *pl = jack->get_engine().pluginlist.find_plugin(id);
	if (!pl) return;

	gx_engine::Parameter *p;
	p = &settings->get_param()[pl->id_on_off()];
	p->set_blocked(true);
	pl->set_on_off(b->getToggleState());
	p->set_blocked(false);
	
	updateMuteButton(b, id);
}

void MachineEditor::pluginMenuChanged(PluginEditor *ped, juce::ComboBox *c, bool stereo)
{
	std::string id(ped->getID());

	int sel = c->getSelectedId();

	std::list<gx_engine::Plugin*> lv;
	if (stereo) get_visible_stereo(lv); else get_visible_mono(lv);
	lv.sort(plugin_order);

	auto v = lv.begin();
	for (; v != lv.end(); v++) if (--sel == 0) break;

	if (sel == 0)
	{
		auto pd = (*v)->get_pdef();

		for (int i = 0; i < cp.getNumPanels(); i++) //remove the same editor
		{
			PluginEditor* eds = (PluginEditor*)cp.getPanel(i);
			if (strcmp(eds->getID(), pd->id) == 0)
			{
				removeButtonClicked(eds,stereo);
				break;
			}
		}

		mIgnoreRackUnitChange = true;
		insert_rack_unit(pd->id, "", stereo);
		if (id.length() != 0)
			remove_rack_unit(id.c_str(), stereo);
		//������ ��� ������ signal_rack_unit_order_changed //TODO
		mIgnoreRackUnitChange = false;

		juce::Rectangle<int> rect = ped->getBoundsInParent();
		int w, h;
		const char* cat = pd->category;
		ped->recreate(pd->id, cat, rect.getX(), rect.getY(), w, h);
		ped->setSize(rect.getWidth(), h);
		cp.setMaximumPanelSize(ped, h);
		cp.expandPanelFully(ped, true);
        //Desktop::getInstance().getAnimator().fadeOut(ped, 1);
        //Desktop::getInstance().getAnimator().fadeIn(ped, 800);
		PluginSelector *ps = ped->getPluginSelector();
		if (ps) ps->setID(pd->id, cat);

		int pos = 0;
		unsigned int post_pre = 1;
		for (int i = 0; i < cp.getNumPanels(); i++)
		{
			PluginEditor *pe = (PluginEditor*)cp.getPanel(i);
			if (strcmp(pe->getID(), "ampstack") == 0)
			{
				pos = 0;
				post_pre = 0;
				continue;
			}

			gx_engine::Plugin *pl = jack->get_engine().pluginlist.find_plugin(pe->getID());

			if (!pl) continue;

			gx_engine::Parameter* p = &settings->get_param()[pl->id_position()];
			p->set_blocked(true);
			pl->set_position(++pos);
			p->set_blocked(false);

			if (!stereo)
			{
				p = &settings->get_param()[pl->id_effect_post_pre()];
				p->set_blocked(true);
				pl->set_effect_post_pre(post_pre);
				p->set_blocked(false);
			}
		}
	}
}

void MachineEditor::addButtonClicked(PluginEditor *ped, bool stereo)
{
	int idx=0;
	for (int i = 0; i < cp.getNumPanels(); i++)
		if ((PluginEditor*)cp.getPanel(i) == ped) {idx = i; break;}

	if (idx == cp.getNumPanels() - 1 && mMono==mn_Both || mMono==mn_Stereo) stereo = true;
	PluginSelector *ps = new PluginSelector(this, stereo, "", "");
	PluginEditor *pe = new PluginEditor(this, "", "", ps);
    if (tunerIsVisible && !stereo && idx == 0) idx = 1;
	addEditor(idx+1, ps, pe, "");
}

void MachineEditor::removeButtonClicked(PluginEditor *ped, bool stereo)
{
	mIgnoreRackUnitChange = true;
	remove_rack_unit(ped->getID(), stereo);
	mIgnoreRackUnitChange = false;
	unregisterParListener(ped->getPluginSelector());
	unregisterParListener(ped);
	cp.removePanel(ped);
	
	if (mMono == mn_Stereo && stereo && cp.getNumPanels()==0)
		addButtonClicked(0, true);
}

//==================================================================================================
void MachineEditor::on_param_insert_remove(gx_engine::Parameter *p, bool inserted)
{
	if (inserted) {
		connect_value_changed_signal(p);
	}
}

void MachineEditor::on_param_value_changed(gx_engine::Parameter *p)
{
	juce::MessageManager::callAsync(
		[this, p]
		{
			for (auto i = editors.begin(); i != editors.end(); i++)
				(*i)->on_param_value_changed(p); 
			/*if (!mIgnoreRackUnitChange && p->id().substr(0, 3) == "ui.")
				juce::MessageManager::callAsync([this] {createPluginEditors(); });*/
		}
	);
}

void MachineEditor::connect_value_changed_signal(gx_engine::Parameter *p) {
	if (p->isInt()) {
		p->getInt().signal_changed().connect(
			sigc::hide(
				sigc::bind(
					sigc::mem_fun(*this, &MachineEditor::on_param_value_changed), p)));
	}
	else if (p->isBool()) {
		p->getBool().signal_changed().connect(
			sigc::hide(
				sigc::bind(
					sigc::mem_fun(*this, &MachineEditor::on_param_value_changed), p)));
	}
	else if (p->isFloat()) {
		p->getFloat().signal_changed().connect(
			sigc::hide(
				sigc::bind(
					sigc::mem_fun(*this, &MachineEditor::on_param_value_changed), p)));
	}
	else if (p->isString()) {
		p->getString().signal_changed().connect(
			sigc::hide(
				sigc::bind(
					sigc::mem_fun(*this, &MachineEditor::on_param_value_changed), p)));
	}
	else if (dynamic_cast<gx_engine::JConvParameter*>(p) != 0) {
		dynamic_cast<gx_engine::JConvParameter*>(p)->signal_changed().connect(
			sigc::hide(
				sigc::bind(
					sigc::mem_fun(*this, &MachineEditor::on_param_value_changed), p)));
	}
	else if (dynamic_cast<gx_engine::SeqParameter*>(p) != 0) {
		dynamic_cast<gx_engine::SeqParameter*>(p)->signal_changed().connect(
			sigc::hide(
				sigc::bind(
					sigc::mem_fun(*this, &MachineEditor::on_param_value_changed), p)));
	}
}

void MachineEditor::on_rack_unit_changed(bool stereo)
{
	//if (mIgnoreRackUnitChange) return;
	//createPluginEditors();
}

bool MachineEditor::insert_rack_unit(const char* id, const char* before, bool stereo) {
	Glib::ustring unit = id;
	gx_engine::Plugin *pl = jack->get_engine().pluginlist.find_plugin(unit);
	if (!pl) {
		return false;// throw RpcError(-32602, Glib::ustring::compose("Invalid param -- unit %1 unknown", unit));
	}
	settings->insert_rack_unit(unit, before, stereo);
	gx_engine::Parameter* p = &settings->get_param()[pl->id_box_visible()];
	p->set_blocked(true);
	pl->set_box_visible(true);
	p->set_blocked(false);

	p = &settings->get_param()[pl->id_on_off()];
	p->set_blocked(true);
	pl->set_on_off(true);
	p->set_blocked(false);

	/*
		int pp = 1;//pre
		if (strcmp(id, "cab") == 0) pp = 0;

		p = &settings->get_param()[pl->id_effect_post_pre()];
		p->set_blocked(true);
		pl->set_effect_post_pre(pp);
		p->set_blocked(false);
		*/

	settings->signal_rack_unit_order_changed()(stereo);
	return true;
}

bool MachineEditor::remove_rack_unit(const char* id, bool stereo) {
	Glib::ustring unit = id;
	gx_engine::Plugin *pl = jack->get_engine().pluginlist.find_plugin(unit);
	if (!pl) {
		return false; // throw RpcError(-32602, Glib::ustring::compose("Invalid param -- unit %1 unknown", unit));
	}
	if (settings->remove_rack_unit(id, stereo)) {
		gx_engine::Parameter *p;
		if (pl->get_box_visible())
		{
			p = &settings->get_param()[pl->id_box_visible()];
			p->set_blocked(true);
			pl->set_box_visible(false);
			p->set_blocked(false);
		}
		p = &settings->get_param()[pl->id_on_off()];
		p->set_blocked(true);
		pl->set_on_off(false);
		p->set_blocked(false);
		settings->signal_rack_unit_order_changed()(stereo);
		return true;
	}
	return false;
}

void MachineEditor::get_visible_mono(std::list<gx_engine::Plugin*> &l) {
	const int bits = (PGN_GUI | gx_engine::PGNI_DYN_POSITION);
	jack->get_engine().pluginlist.ordered_list(l, false, 0, 0);
}

void MachineEditor::get_visible_stereo(std::list<gx_engine::Plugin*> &l) {
	const int bits = (PGN_GUI | gx_engine::PGNI_DYN_POSITION);
	jack->get_engine().pluginlist.ordered_list(l, true, 0, 0);
}

void MachineEditor::list(const char* id, std::list<gx_engine::Parameter*> &pars)
{
	Glib::ustring prefix = id;
	prefix += ".";
	gx_engine::ParamMap& param = settings->get_param();
	for (gx_engine::ParamMap::iterator i = param.begin(); i != param.end(); ++i) {
		if (i->first.compare(0, prefix.size(), prefix) == 0) {
			pars.push_back(i->second);
		}
	}
}

gx_engine::Parameter* MachineEditor::get_parameter(const char* pid) {
	gx_engine::ParamMap& param = settings->get_param();
	const Glib::ustring& id = pid;
	if (param.hasId(id))
		return &param[id];
	return 0;
}

//==============Get host provided context menu =========================

void MachineEditor::get_host_menu_for_parameter(juce::AudioProcessorParameter* param) {
    if (auto* c = audioProcessor.getEditor()->getHostContext())
        if (auto menuInfo = c->getContextMenuForParameter (param))
            menuInfo->getEquivalentPopupMenu().showMenuAsync(
                juce::PopupMenu::Options{}.withTargetComponent(this).withMousePosition());
}

void MachineEditor::getParameterContext(const char* id) {
    juce::RangedAudioParameter* param = audioProcessor.findParamForID(id);
    if (param) get_host_menu_for_parameter(param);
}

void MachineEditor::muteButtonContext(juce::ToggleButton *b, const char* id)
{
	gx_engine::Plugin *pl = jack->get_engine().pluginlist.find_plugin(id);
	if (!pl) return;

	juce::RangedAudioParameter* param = audioProcessor.findParamForID(pl->id_on_off().c_str());
    if (param) get_host_menu_for_parameter(param);
}

void MachineEditor::presetFileMenuContext() {
	juce::RangedAudioParameter* param = audioProcessor.findParamForID("selPreset");
    if (param) get_host_menu_for_parameter(param);
}
