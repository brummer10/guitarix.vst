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
#include "gx_jack_wrapper.h"
#include "guitarix.h"       // NOLINT
#include "GuitarixEditor.h"

#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif

using namespace juce;

static volatile int opt_counter=0;

gx_system::CmdlineOptions *GuitarixStart::options = 0;

GuitarixStart::GuitarixStart(int argc, char *argv[])
{
    Glib::init();
    Gio::init();
    std::locale::global(std::locale("C"));
    if (!opt_counter)
        options=new gx_system::CmdlineOptions(argc>=1?argv[0]:"");
    if (options == 0) {
        options=new gx_system::CmdlineOptions(argc>=1?argv[0]:"");
        opt_counter = 0;
    }
    options->parse(argc, argv);
    options->process(argc, argv);
    need_new_preset = false;
    gx_preset::GxSettings::check_settings_dir(*options, &need_new_preset);
    machine=new gx_engine::GxMachine(*options);
    jack = machine->get_jack();
    machine_r=new gx_engine::GxMachine(*options);
    jack_r = machine_r->get_jack();
    gx_preset::GxSettings *settings = &(machine->get_settings());
    gx_engine::ParamMap& pmap = settings->get_param();
    gx_engine::ParamRegImpl preg(&pmap);
    opt_counter++;
}

GuitarixStart::~GuitarixStart()
{
    opt_counter--;
    if (opt_counter){
        // @brummer Hack around static ParamMap in ParamRegImpl.
        // we need to reanimate it when a instance was destroyed.
        // before we could use it again in a remaining instance.
        gx_preset::GxSettings *settings = &(machine->get_settings());
        gx_engine::ParamMap& pmap = settings->get_param();
        gx_engine::ParamRegImpl preg(&pmap);
    }
    delete machine;
    delete machine_r;
    // delete CmdlineOptions with the last instance.
    if (!opt_counter)
        delete options;
}

void GuitarixStart::check_config_dir() {
    if (need_new_preset) machine->create_default_scratch_preset();
}

void GuitarixStart::gx_load_preset(gx_engine::GxMachine* machine, const char* bank_, const char* name_)
{
	Glib::ustring bank(bank_);
	Glib::ustring name(name_);
	gx_system::PresetFileGui *cpf = machine->get_bank_file(bank);
	machine->load_preset(cpf, name);
}

void GuitarixStart::gx_save_preset(gx_engine::GxMachine* machine, const char* bank_, const char* name_)
{
	Glib::ustring bank(bank_);
	Glib::ustring name(name_);
	gx_system::PresetFileGui& cpf = *machine->get_bank_file(bank);
	machine->pf_save(cpf, name);
}

extern void gx_inited();

//==============================================================================
GuitarixProcessor::GuitarixProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
	, scale(1.0)
	, editor(0)
	, mStereoMode(false)
	, mMultiMode(false)
	, mMono1Mute(false)
	, mMono2Mute(false)
    , buffersize(0)
	, mLoading(false)
	, mPresetsVisible(false)
	, currentPreset(-1)
{
    out[0]=out[1]=0;
    
#ifdef _WINDOWS
	static CHAR sModulePath[2048];
	::GetModuleFileName((HMODULE)Process::getCurrentModuleInstanceHandle(), sModulePath, sizeof(sModulePath) / sizeof(sModulePath[0]));

	defaultPath = File(sModulePath).getSiblingFile("Presets\\default.gxpreset");
	currentFile = WindowsRegistry::getValue("HKEY_CURRENT_USER\\Software\\Guitarix\\LastPreset", defaultPath.getFullPathName());
#else
    //TODO use on Windows??
    File app = File::getSpecialLocation (File::SpecialLocationType::currentApplicationFile);
    static string path(app.getFullPathName().toRawUTF8());
    static char *sModulePath=const_cast<char*>(path.c_str());

	File data = File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory);
	defaultPath = data.getChildFile("Application Support/Guitarix/Presets/default.gxpreset");
	PropertiesFile::Options o;
	o.applicationName = JucePlugin_Name;
	o.commonToAllUsers = false;
	o.osxLibrarySubFolder = "Preferences";
	o.filenameSuffix = "xml";
	PropertiesFile f(o);
	currentFile = f.getValue("LastPreset", defaultPath.getFullPathName());
#endif

	char* argv[1] = { sModulePath };
    gx = new GuitarixStart(sizeof(argv) / sizeof(argv[0]), argv);
    gx->check_config_dir();
    jack = gx->get_jack();
    machine = gx->get_machine();
    jack_r = gx->get_jack_r();
    machine_r = gx->get_machine_r();
	//jack = gx_start(sizeof(argv) / sizeof(argv[0]), argv, machine);
	//jack_r = gx_start(sizeof(argv) / sizeof(argv[0]), argv, machine_r);

	options = gx->get_options();
	jack->gx_jack_connection(true, true, 0, *options);
	jack_r->gx_jack_connection(true, true, 0, *options);

	//for resetting parameters in Dsp::init()
	jack->buffersize_callback(512);
	jack->srate_callback((int)22050);
	jack_r->buffersize_callback(512);
	jack_r->srate_callback((int)22050);

	par_stereo = new AudioParameterBool({"stereo",1}, "Stereo In", false);
	par_stereo->addListener(this);
	addParameter(par_stereo);
	gx_preset::GxSettings *settings = &(machine->get_settings());
	gx_engine::ParamMap& pmap = settings->get_param();
    gx_engine::BoolParameter& mStereo = pmap.reg_par(
      "engine.set_stereo", N_("switch stereo input on/off"), &mStereoMode, false, true)->getBool();
	pmap.signal_insert_remove().connect(
		sigc::bind(sigc::mem_fun(*this, &GuitarixProcessor::on_param_insert_remove), false));
    mStereo.signal_changed().connect(
        sigc::mem_fun(this, &GuitarixProcessor::SetStereoMode));
	for (gx_engine::ParamMap::iterator i = pmap.begin(); i != pmap.end(); ++i) {
		connect_value_changed_signal(i->second, false);
	}
	settings->signal_rack_unit_order_changed().connect(
		sigc::bind(sigc::mem_fun(*this, &GuitarixProcessor::on_rack_unit_changed), false));
	
	/*
	gx_preset::GxSettings *settings_r = &(machine_r->get_settings());
	gx_engine::ParamMap& pmap_r = settings_r->get_param();
	pmap_r.signal_insert_remove().connect(
		sigc::bind(sigc::mem_fun(*this, &GuitarixProcessor::on_param_insert_remove), true));
	for (gx_engine::ParamMap::iterator i = pmap_r.begin(); i != pmap_r.end(); ++i) {
		connect_value_changed_signal(i->second, true);
	}
	settings_r->signal_rack_unit_order_changed().connect(
		sigc::bind(sigc::mem_fun(*this, &GuitarixProcessor::on_rack_unit_changed), true));
	*/

	refreshPrograms();

	timer.set_machine(machine, machine_r);
	timer.startTimer(100);
}

void PluginUpdateTimer::timerCallback()
{
    const ScopedLock lock (timer_cs);
	if (machine)
		machine->timerUpdate();
	if (machine_r)
		machine_r->timerUpdate();
	if (mUpdateMode)
	{
		mUpdateMode = false;
		if (editor) editor->updateModeButtons();
	}
}

GuitarixProcessor::~GuitarixProcessor()
{
#ifdef _WINDOWS
	WindowsRegistry::setValue("HKEY_CURRENT_USER\\Software\\Guitarix\\LastPreset", currentFile);
#else
	PropertiesFile::Options o;
	o.applicationName = JucePlugin_Name;
	o.commonToAllUsers = false;
	o.osxLibrarySubFolder = "Preferences";
	o.filenameSuffix = "xml";
	PropertiesFile f(o);
	f.setValue("LastPreset", currentFile);
#endif
	
	{
    const ScopedLock lock (timer.timer_cs);
    timer.stopTimer();
    }
    delete out[0]; out[0]=0;
    delete out[1]; out[1]=0;
    delete gx;
}

void GuitarixProcessor::parameterValueChanged(int parameterIndex, float newValue)
{
	mStereoMode = newValue > 0.5;
	timer.update_mode(); 
}

void GuitarixProcessor::SetStereoMode(bool on)
{
	mStereoMode = on;
	*par_stereo = on;
}

//==============================================================================

void GuitarixProcessor::update_plugin_list(bool add)
{
    machine->save_ladspalist(editor->ml);
    jack->get_engine().ladspaloader_update_plugins();
    if (add) {
       machine_r->save_ladspalist(editor->ml);
        jack_r->get_engine().ladspaloader_update_plugins();
    }
}

void GuitarixProcessor::on_param_insert_remove(gx_engine::Parameter *p, bool inserted, bool right)
{
	if (inserted) {
		connect_value_changed_signal(p, right);
	}
}

void GuitarixProcessor::on_param_value_changed(gx_engine::Parameter *p, bool right)
{
	bool multi = mMultiMode;
	if (editor && editor->GetAlternateDouble() && mMultiMode) multi = false;
	
	if (mLoading) return;

	juce::MessageManager::callAsync(
		[this, p, right, multi]
	{
		if (multi) return;
		gx_preset::GxSettings *settings = &((right?machine:machine_r)->get_settings());
		gx_engine::ParamMap& param = settings->get_param();
		gx_engine::Parameter& p1 = param[p->id()];
		p1.set_blocked(true);
		if (p1.isFloat())
			p1.getFloat().set(p->getFloat().get_value());
		else if (p1.isInt())
			p1.getInt().set(p->getInt().get_value());
		else if (p1.isBool())
		{
			p1.getBool().set(p->getBool().get_value());
			if (p->id().substr(0, 3) == "ui.")
			{
				std::stringstream ss;
				saveState(ss, right);
				loadState(ss, !right);

				//if (editor) editor->createPluginEditors(right, !right, false);
			}
		}
		else if (p1.isString())
			p1.getString().set(p->getString().get_value());
		else if (dynamic_cast<gx_engine::JConvParameter*>(&p1) != 0)
		{
			gx_engine::JConvParameter *pp = dynamic_cast<gx_engine::JConvParameter*>(p);
			gx_engine::JConvParameter *pp1 = dynamic_cast<gx_engine::JConvParameter*>(&p1);
			pp1->set(pp->get_value());
		}
		else if (dynamic_cast<gx_engine::SeqParameter*>(&p1) != 0)
		{
			gx_engine::SeqParameter *pp = dynamic_cast<gx_engine::SeqParameter*>(p);
			gx_engine::SeqParameter *pp1 = dynamic_cast<gx_engine::SeqParameter*>(&p1);
			pp1->set(pp->get_value());
		}
		p1.set_blocked(false);
	}
	);
}

/*
void PresetIO::read_parameters(gx_system::JsonParser &jp, bool preset) {
	UnitsCollector u;
	jp.next(gx_system::JsonParser::begin_object);
	do {
		jp.next(gx_system::JsonParser::value_key);
		gx_engine::Parameter *p;
		if (!param.hasId(jp.current_value())) {
			if (convert_old(jp)) {
				continue;
			}
			std::string s = replaced_id(jp.current_value());
			if (s.empty()) {
				gx_print_warning(
					_("recall settings"),
					_("unknown parameter: ") + jp.current_value());
				jp.skip_object();
				continue;
			}
			p = &param[s];
		}
		else {
			p = &param[jp.current_value()];
		}
		if (!preset and p->isInPreset()) {
			gx_print_warning(
				_("recall settings"),
				_("preset-parameter ") + p->id() + _(" in settings"));
			jp.skip_object();
			continue;
		}
		else if (preset and !p->isInPreset()) {
			gx_print_warning(
				_("recall settings"),
				_("non preset-parameter ") + p->id() + _(" in preset"));
			jp.skip_object();
			continue;
		}
		else if (!p->isSavable()) {
			gx_print_warning(
				_("recall settings"),
				_("non saveable parameter ") + p->id() + _(" in settings"));
			jp.skip_object();
			continue;
		}
		p->readJSON_value(jp);
		collectRackOrder(p, jp, u);
	} while (jp.peek() == gx_system::JsonParser::value_key);
	jp.next(gx_system::JsonParser::end_object);
	u.get_list(rack_units.mono, false, param);
	u.get_list(rack_units.stereo, true, param);
}

void PresetIO::collectRackOrder(gx_engine::Parameter *p, gx_system::JsonParser &jp, UnitsCollector& u) {
	const std::string& s = p->id();
	if (startswith(s, 3, "ui.")) {
		if (jp.current_value_int()) {
			std::string ss = s.substr(3);
			u.set_visible(ss, true);
			u.set_show(ss, true);
		}
	}
	else if (endswith(s, 7, ".on_off")) {
		if (jp.current_value_int()) {
			u.set_show(s.substr(0, s.size() - 7), true);
		}
	}
	else if (endswith(s, 9, ".position")) {
		u.set_position(s.substr(0, s.size() - 9), jp.current_value_int());
	}
	else if (endswith(s, 3, ".pp")) {
		u.set_pp(s.substr(0, s.size() - 3), (jp.current_value() == "pre" ? 1 : 0));
	}
}
*/

void GuitarixProcessor::load_preset(std::string _bank, std::string _preset) {
    bool stereo = mStereoMode;
    SetStereoMode(false);
    gx->gx_load_preset(machine, _bank.c_str(), _preset.c_str());
	if(editor)
		editor->createPluginEditors();
    SetStereoMode(stereo);
}

void GuitarixProcessor::save_preset(std::string _bank, std::string _preset) {
    gx->gx_save_preset(machine, _bank.c_str(), _preset.c_str());
}

void GuitarixProcessor::connect_value_changed_signal(gx_engine::Parameter *p, bool right)
{
	if (p->isInt()) {
		p->getInt().signal_changed().connect(
			sigc::hide(
				sigc::bind(
					sigc::bind(
						sigc::mem_fun(*this, &GuitarixProcessor::on_param_value_changed), right), p)));
	}
	else if (p->isBool()) {
		p->getBool().signal_changed().connect(
			sigc::hide(
				sigc::bind(
					sigc::bind(
						sigc::mem_fun(*this, &GuitarixProcessor::on_param_value_changed), right), p)));
	}
	else if (p->isFloat()) {
		p->getFloat().signal_changed().connect(
			sigc::hide(
				sigc::bind(
					sigc::bind(
						sigc::mem_fun(*this, &GuitarixProcessor::on_param_value_changed), right), p)));
	}
	else if (p->isString()) {
		p->getString().signal_changed().connect(
			sigc::hide(
				sigc::bind(
					sigc::bind(
						sigc::mem_fun(*this, &GuitarixProcessor::on_param_value_changed), right), p)));
	}
	else if (dynamic_cast<gx_engine::JConvParameter*>(p) != 0) {
		dynamic_cast<gx_engine::JConvParameter*>(p)->signal_changed().connect(
			sigc::hide(
				sigc::bind(
					sigc::bind(
						sigc::mem_fun(*this, &GuitarixProcessor::on_param_value_changed), right), p)));
	}
	else if (dynamic_cast<gx_engine::SeqParameter*>(p) != 0) {
		dynamic_cast<gx_engine::SeqParameter*>(p)->signal_changed().connect(
			sigc::hide(
				sigc::bind(
					sigc::bind(
						sigc::mem_fun(*this, &GuitarixProcessor::on_param_value_changed), right), p)));
	}
}

void GuitarixProcessor::on_rack_unit_changed(bool stereo, bool right)
{
	//if (editor && !mMultiMode) editor->createPluginEditors(right ? true : false, right ? false : true);
}

//==============================================================================
const juce::String GuitarixProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuitarixProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GuitarixProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GuitarixProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GuitarixProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

void GuitarixProcessor::refreshPrograms()
{
	gx_preset::GxSettings* settings = &(machine->get_settings());

	std::string bank;
	std::string preset;
	if (settings->setting_is_preset()) {
		bank = settings->get_current_bank();
		preset = settings->get_current_name();
	}
	else
	{
		bank = "";
		preset = "";
	}

	std::string bp;
	gx_system::PresetBanks* bb = &settings->banks;
	int bi = 0, sel = 0;
	if (bb)
		for (auto b = bb->begin(); b != bb->end(); ++b)
		{
			gx_system::PresetFile* pp = settings->banks.get_file(b->get_name());
			
			int pi = 0;
			if (pp)
				for (auto p = pp->begin(); p != pp->end(); ++p)
				{
					int idx = bi * 1000 + (pi++) + 1;
					presets.push_back(make_pair(b->get_name().raw(),p->name.raw()));
					if (b->get_name().raw() == bank && p->name.raw() == preset)
						currentPreset = presets.size() - 1;//sel = idx;
				}
			bi++;
		}

//	if (sel > 0)
//	presetFileMenu.setSelectedId(sel, true);
}

int GuitarixProcessor::getNumPrograms()
{
	return 1;// max(1, presets.size());
				// NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GuitarixProcessor::getCurrentProgram()
{
	return 0;// currentPreset;
}

const juce::String GuitarixProcessor::getProgramName(int index)
{
	/*if (0 <= index && index < presets.size())
		return presets[index].first + ":" + presets[index].second;
	else*/
		return {};
}

void GuitarixProcessor::changeProgramName(int index, const juce::String& newName)
{
	//TODO: implement
}

void GuitarixProcessor::setCurrentProgram (int index)
{/*
	if (index < 0 || index >= presets.size()) return;

	gx_preset::GxSettings* settings = &(machine->get_settings());
	gx_system::PresetBanks* bb = &settings->banks;
	if (!bb) return;
	gx_system::PresetFile* pf = bb->get_file(presets[index].first);
	if (pf)
		settings->load_preset(pf, presets[index].second);

	if(editor)
		editor->createPluginEditors();*/
}

//==============================================================================
void GuitarixProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
//	gx_system::CmdlineOptions *options=0;
//	jack->gx_jack_connection(true, true, 0, *options);
    jack->get_engine().set_rack_changed();

    for(auto &r: rms)
    {
        r.reset(sampleRate, 0.5);
        r.setCurrentAndTargetValue(-100.f);
    }

    if (buffersize!=samplesPerBlock)
    {
        delete out[0]; out[0]=0;
        delete out[1]; out[1]=0;
        quantum=buffersize=samplesPerBlock;
        
        if(samplesPerBlock & (samplesPerBlock-1))
        {
            int k;
            for (k=6; (1<<k) < buffersize; k++)
                if(buffersize-(1<<k)<=(1<<k)) break;
            quantum=(1<<k);
            int q=buffersize-quantum;
            int l=quantum/q; if(l*q<quantum) l++;
            
            //delay=(l-1)*q;
			delay=quantum-1;
        }
        else
        {
            if(buffersize>=1024) quantum=buffersize/4;
            else if(buffersize>=512) quantum=buffersize/2;
            delay=0;
        }
        wpos=0;
        rpos=0;
        ppos=0;
        olen=((buffersize+quantum-1)/quantum+1)*quantum;
		tdelay=0;

        DBG("***PREPARE buffersize:"<<buffersize<<" delay:"<<delay<<" quantum:"<<quantum<<" olen:"<<olen);

        out[0]=new float[olen];
        out[1]=new float[olen];
    }

	std::ostringstream os;
	saveState(os, false);

	jack->buffersize_callback(quantum);
	jack->srate_callback((int)sampleRate);
	jack_r->buffersize_callback(quantum);
	jack_r->srate_callback((int)sampleRate);

	//Restore state - workaround to override parameters reset during Dsp::init() on sample rate change
	mLoading = true;
	std::istringstream is(os.str());
	loadState(is, false);
	mLoading = false;
	cloneSettingsToMachineR();
  
	gx_inited();
	//gx_load_preset(machine, "Scratchpad", "Putilin");
}

void GuitarixProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GuitarixProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainInputChannelSet()!=juce::AudioChannelSet::mono() && layouts.getMainInputChannelSet()!=juce::AudioChannelSet::stereo())
        return false;
   #endif

    return true;
  #endif
}
#endif

float GuitarixProcessor::getRMSLevel(float* data, int len) const
{
    double sum=0.0;
    
    for(int i=0; i<len; i++)
        sum+=data[i]*data[i];
    return (float)std::sqrt(sum/len);
}

#define DBGRT(x)

void GuitarixProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	gx_inited();
	juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
/*    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }*/
	if (buffer.getNumChannels()>=2)
	{
		float *buf[2];
		int n = buffer.getNumSamples();
		buf[0] = buffer.getWritePointer(0);
        buf[1] = buffer.getWritePointer(1);

        {
        for(auto &r: rms) r.skip(n);

        const auto l=Decibels::gainToDecibels(getRMSLevel(buf[0],n));
        if(l<rms[0].getCurrentValue()) rms[0].setTargetValue(l); else rms[0].setCurrentAndTargetValue(l);
        const auto r=Decibels::gainToDecibels(getRMSLevel(buf[1],n));
        if(r<rms[1].getCurrentValue()) rms[1].setTargetValue(r); else rms[1].setCurrentAndTargetValue(r);
        }
        
        if(out[0]==0 || out[1]==0)
            process(buf, n);
        else
        {
            DBGRT("BUF len:"<<n<<" delay:"<<delay);
            //rpos - reading index in out[], wpos - writing index, ppos - processing index
            //rpos<=ppos<=wpos (with respect to olen wrapping)
            
            {//read input buf[]
            //read input buf[] up to out[] buffer end
            int l=n;
            if(olen-wpos<l) l=olen-wpos;
            memcpy(out[0]+wpos, buf[0], l*sizeof(float));
            memcpy(out[1]+wpos, buf[1], l*sizeof(float));
            wpos+=l;
            DBGRT("    WPOS:"<<wpos<<" after reading1 "<<l);
            if(wpos>=olen) wpos-=olen; //wpos=0;
            
            //read the rest of input buf[] if nessesary
            if(n>l)
            {
                memcpy(out[0]+wpos, buf[0]+l, (n-l)*sizeof(float));
                memcpy(out[1]+wpos, buf[1]+l, (n-l)*sizeof(float));
                wpos+=(n-l);
                DBGRT("    WPOS:"<<wpos<<" after reading2 "<<(n-l));
                assert(wpos<=rpos); //buffer should not overflow
            }
            }
            
            //process complete quantum-sized chunks
            while(!(ppos<=wpos && wpos-ppos<quantum))
            {
                float *p[2];
                p[0]=out[0]+ppos;
                p[1]=out[1]+ppos;
                process(p, quantum);
                ppos+=quantum;
                DBGRT("    PPOS:"<<ppos<<" after processing "<<quantum<<" unprocessed:"<<(wpos>=ppos?wpos-ppos:olen-ppos+wpos));
                if(ppos>=olen) ppos-=olen; //ppos=0;
            }

            {//write processed
            //enlarge delay if nessesary
            int p=olen-rpos;
            if(ppos>=rpos) p=ppos-rpos;
            else p+=ppos;
            if(p+delay<n)
            {
                delay=(n-p);
                DBG("***DELAY INCREASED:"<<delay<<" processed:"<<p<<" required:"<<n);
            }
                
            //clear delay part
            int o=0;
            if(delay>0)
            {
                int z=delay;
                if(z>n) z=n;
                memset(buf[0], 0, z*sizeof(float));
                memset(buf[1], 0, z*sizeof(float));
                delay-=z;
				tdelay += z;
                DBGRT("    DELAY:"<<delay<<" after cleaning "<<z);
				DBGRT("    TOTAL DELAY:" << tdelay);
				o+=z;
            }
            
            //write processed up to out[] buffer end
            int l=olen-rpos;
            if(n-o<l) l=n-o;//limit to host buffer length
            if(ppos>=rpos && ppos-rpos<l)//limit to processed part
            {
                l=ppos-rpos;
                assert(n-o>=l);//there should be processed data to fill host buffer
            }
            if(l)
            {
                memcpy(buf[0]+o, out[0]+rpos, l*sizeof(float));
                memcpy(buf[1]+o, out[1]+rpos, l*sizeof(float));
                rpos+=l;
                DBGRT("    RPOS:"<<rpos<<" after writing1 "<<l);
                if(rpos>=olen) rpos-=olen; //rpos=0;
                o+=l;
            }
                
            //write the rest of the buffer if nessesary
            if(n-o>0)
            {
                assert(rpos==0 && ppos-rpos>=n-o);//second copy can occure only after warap around
                int l=ppos-rpos;
                if(n-o<l) l=n-o;//limit to host buffer length
                memcpy(buf[0]+o, out[0]+rpos, l*sizeof(float));
                memcpy(buf[1]+o, out[1]+rpos, l*sizeof(float));
                rpos+=l;
                DBGRT("    RPOS:"<<rpos<<" after writing2 "<<l);
                if(rpos>=olen) rpos-=olen; //rpos=0;
            }
            }
        }
        {
        const auto l=Decibels::gainToDecibels(getRMSLevel(buf[0],n));
        if(l<rms[2].getCurrentValue()) rms[2].setTargetValue(l); else rms[2].setCurrentAndTargetValue(l);
        const auto r=Decibels::gainToDecibels(getRMSLevel(buf[1],n));
        if(r<rms[3].getCurrentValue()) rms[3].setTargetValue(r); else rms[3].setCurrentAndTargetValue(r);
        }

		jack->finish_process();
		jack_r->finish_process();
	}
}

void GuitarixProcessor::process(float *out[2], int n)
{
	if (!mStereoMode && !mMultiMode)
	{
		jack->process(n, out[0], out);
		jack_r->process_ramp(n);
	}
    else if(!mStereoMode && mMultiMode)
    {
		if (mMono2Mute)
		{
			memset(out[1], 0, sizeof(float) * n);
			jack_r->process_ramp_mono(n);
		}
        else
            jack_r->process_mono(n, out[0], out[1]);
		if (mMono1Mute)
		{
			memset(out[0], 0, sizeof(float) * n);
			jack->process_ramp_mono(n);
		}
        else
            jack->process_mono(n, out[0], out[0]);
        jack->process_stereo(n, out, out);
		jack_r->process_ramp_stereo(n);
    }
    else //if (mStereoMode)
    {
		if (mMono2Mute)
		{
			memset(out[1], 0, sizeof(float) * n);
			jack_r->process_ramp_mono(n);
		}
        else
            jack_r->process_mono(n, out[1], out[1]);
        if (mMono1Mute)
		{
            memset(out[0], 0, sizeof(float)*n);
			jack->process_ramp_mono(n);
		}
		else
            jack->process_mono(n, out[0], out[0]);
        jack->process_stereo(n, out, out);
		jack_r->process_ramp_stereo(n);
	}

}

//==============================================================================

void GuitarixProcessor::loadState(std::istream& is, bool right)
{
	gx_system::AbstractStateIO* io = get_machine(right)->get_settings().get_state_io();
	gx_system::JsonParser jp(&is);
	gx_system::SettingsFileHeader header;
	jp.next(gx_system::JsonParser::begin_array);
	header.read(jp);
	io->read_state(jp, header);
	io->commit_state();
}

void GuitarixProcessor::saveState(std::ostream& os, bool right)
{
	gx_system::AbstractStateIO* io = get_machine(right)->get_settings().get_state_io();
	gx_system::JsonWriter jw(&os);
	jw.begin_array();
	gx_system::SettingsFileHeader::write(jw);
	io->write_state(jw, false);
	jw.end_array();
}

void GuitarixProcessor::cloneSettingsToMachineR()
{
	std::ostringstream os;
	saveState(os, false);

	std::istringstream is(os.str());
	loadState(is, true);

	/*	gx_engine::ParamMap &p = machine->get_settings().get_param();
		gx_engine::ParamMap &p_r = machine_r->get_settings().get_param();

		for (auto i = p.begin(); i != p.end(); i++)
			p_r[i->first] = *i->second;*/
}

const int8 kVersion = 1;
const int8 kZeroVersion = 91;// '['

void GuitarixProcessor::getStateInformation (juce::MemoryBlock& destData)
{
	auto settings = &(machine->get_settings());

/*	int8 version = kVersion;
	destData.append(&version, sizeof(version));

	int8 stereo = mStereoMode;
	destData.append(&stereo, sizeof(stereo));

	int32 slen=currentFile.length();
	destData.append(&slen, sizeof(slen));

	destData.append(currentFile.toStdString().c_str(), slen);
*/
	std::ostringstream os;
	saveState(os, false);
	//::OutputDebugString(os.str().c_str());

	destData.append(os.str().c_str(), os.str().length());

	//auto xml = juce::parseXML(os.str().c_str());
	//copyXmlToBinary()
}

void GuitarixProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	auto settings = &(machine->get_settings());

	int offset = 0;
/*
	if (sizeInBytes < 1) return;
	int8 version=*(const int8*)data;
	offset += 1;
	if (version == kZeroVersion) {version = 0; offset = 0;}

	if (version >= 1)
	{
		if (sizeInBytes < offset + 1) return;
		int8 stereo = *((const int8*)data + offset);
		SetStereoMode(stereo != 0);
		offset += 1;

		if (sizeInBytes < offset+4) return;
		int32 slen=*(int32*)((const int8*)data+offset);
		offset += 4;

		if (sizeInBytes < offset+slen) return;
		currentFile = String(CharPointer_UTF8((const char*)data + offset),slen);
		offset += slen;
	}
	else
		currentFile = defaultPath.getParentDirectory().getChildFile("---").getFullPathName();
		*/
	std::istringstream is;
	is.str(std::string((const char*)data + offset, sizeInBytes - offset));

	machine->start_ramp_down();
	machine_r->start_ramp_down();
	machine->wait_ramp_down_finished();
	machine_r->wait_ramp_down_finished();
	mLoading = true;
	loadState(is, false);
	mLoading = false;
	cloneSettingsToMachineR();

	machine->start_ramp_up();
	machine_r->start_ramp_up();

	if (editor)
	{
		editor->createPluginEditors();
		//editor->updateModeButtons();
	}
}

//==============================================================================
bool GuitarixProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GuitarixProcessor::createEditor()
{
	return new GuitarixEditor(*this);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarixProcessor();
}
