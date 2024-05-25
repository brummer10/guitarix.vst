
# info colors
ifneq ($(TERM),)
  red :=$(shell tput setaf 1)
  green :=$(shell tput setaf 2)
  yellow :=$(shell tput setaf 3)
  blue :=$(shell tput setaf 4)
  purple :=$(shell tput setaf 5)
  cyan :=$(shell tput setaf 6)
  reset :=$(shell tput sgr0)
  ceol :=$(shell tput cuu1 && tput el)
  ceol2 :=$(shell tput cuu 2 && tput el)
else
  red :=$()
  green :=$()
  yellow :=$()
  blue :=$()
  purple :=$()
  cyan :=$()
  reset :=$()
  ceol :=$()
  ceol2 :=$()
  ECHO :=echo
endif

# counter
ifndef ECHO
HIT_TOTAL != ${MAKE} ${MAKECMDGOALS} --dry-run ECHO="HIT_MARK" | grep -c "HIT_MARK"
HIT_COUNT = $(eval HIT_N != expr ${HIT_N} + 1)${HIT_N}
ECHO = echo "${ceol}${yellow}[`expr ${HIT_COUNT} '*' 100 / ${HIT_TOTAL}`%]${purple}"
endif

# build with "V=1" for verbose builds
ifeq ($(V), 1)
V_AT =
else
V_AT = @
endif

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef PKG_CONFIG
  PKG_CONFIG=pkg-config
endif

ifndef STRIP
  STRIP=strip
endif

ifndef AR
  AR=ar
endif

ifndef CONFIG
  CONFIG=Release
endif

ifeq ($(MAKECMDGOALS),LV2)
  $(info $(yellow)INFO: $(red)LV2 format is experimental only! Use it on your own risk!!$(reset))
endif

STANDALONE_IR_PATH ?= /usr/share/gx_head/sounds/
JUCE_VST3DESTDIR := $(HOME)/.vst3

NAM_FLAGS := "-DDSP_SAMPLE_FLOAT" "-DNAM_SAMPLE_FLOAT" "-Dneural_amp_modeler_EXPORTS"

NAM_INCLUD_DIRS := \
        -I../../guitarix/trunk/src/NAM/NeuralAmpModelerCore/NAM/ \
        -I../../guitarix/trunk/src/NAM/NeuralAmpModelerCore/Dependencies/eigen/ \
        -I../../guitarix/trunk/src/NAM/NeuralAmpModelerCore/Dependencies/nlohmann/

RTNEURAL_FLAGS := "-DRTNEURAL_DEFAULT_ALIGNMENT=32" "-DRTNEURAL_USE_EIGEN=1" "-DRTNEURAL_NAMESPACE=RTNeural"

RTNEURAL_INCLUD_DIRS := \
        -I../../guitarix/trunk/src/RTNeural/RTNeural/RTNeural/ \
        -I../../guitarix/trunk/src/RTNeural/RTNeural/modules/Eigen/ \
        -I../../guitarix/trunk/src/RTNeural/RTNeural/modules/json/

ifneq ($(MAKECMDGOALS),install)
ifneq ($(MAKECMDGOALS),clean)
ifndef JUCE_DIR
  JUCE_DIR := ../../JuceModules
  ifdef USE_SYSTEM_JUCE
    JUCE_DIR := /usr/share/juce
    SYS_INCLUDE_FLAGS = -lpng16 -lz -ljpeg -lFLAC -logg -lvorbis -lvorbisenc -lvorbisfile `pkg-config --cflags --libs sigc++-2.0`
    $(info $(yellow)INFO: $(reset)Using system Juce modules $(blue)[${JUCE_DIR}]$(reset))
  else
    $(info $(yellow)INFO: $(reset)Using included Juce modules $(blue)[${JUCE_DIR}]$(reset))
  endif
else
  $(info $(yellow)INFO: $(reset)Using Juce modules from $(blue)${JUCE_DIR}$(reset))
endif
endif
endif

ifndef VST3_DIR
  VST3_DIR := $(JUCE_DIR)/modules/juce_audio_processors/format_types/VST3_SDK
else
  $(info Using VST§_SDK from $(blue)${VST§_DIR}$(reset))
endif

JUCE_ARCH_LABEL := $(shell uname -m)
GUITARIX_VST_VERSION := 0.3

ifeq ($(CONFIG),Debug)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Debug
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := 
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) $(NAM_FLAGS) $(RTNEURAL_FLAGS) "-DGXV=\"$(GUITARIX_VST_VERSION)\"" "-DIRPATH=\"$(STANDALONE_IR_PATH)\"" "-DLINUX=1" "-DDEBUG=1" "-D_DEBUG=1" "-DJUCE_USE_CURL=0" "-DJUCE_DISABLE_CAUTIOUS_PARAMETER_ID_CHECKING=1" "-DJUCE_DISPLAY_SPLASH_SCREEN=0" "-DJUCE_USE_DARK_SPLASH_SCREEN=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_devices=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_formats=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_plugin_client=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_processors=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_utils=1" "-DJUCE_MODULE_AVAILABLE_juce_core=1" "-DJUCE_MODULE_AVAILABLE_juce_data_structures=1" "-DJUCE_MODULE_AVAILABLE_juce_events=1" "-DJUCE_MODULE_AVAILABLE_juce_graphics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_extra=0" "-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1" "-DJUCE_VST3_CAN_REPLACE_VST2=0" "-DJUCE_STRICT_REFCOUNTEDPOINTER=1" "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=1" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=1" "-DJucePlugin_Build_Unity=0" "-DJucePlugin_Build_LV2=1" "-DJucePlugin_Enable_IAA=0" "-DJucePlugin_Enable_ARA=0" "-DJucePlugin_Name=\"Guitarix\"" "-DJucePlugin_Desc=\"Guitarix\"" "-DJucePlugin_Manufacturer=\"brummer\"" "-DJucePlugin_ManufacturerWebsite=\"www.brummer.com\"" "-DJucePlugin_ManufacturerEmail=\"\"" "-DJucePlugin_ManufacturerCode=0x4d616e75" "-DJucePlugin_PluginCode=0x4c736166" "-DJucePlugin_IsSynth=0" "-DJucePlugin_WantsMidiInput=1" "-DJucePlugin_ProducesMidiOutput=0" "-DJucePlugin_IsMidiEffect=0" "-DJucePlugin_EditorRequiresKeyboardFocus=0" "-DJucePlugin_Version=1.0.0" "-DJucePlugin_VersionCode=0x10000" "-DJucePlugin_VersionString=\"1.0.0\"" "-DJucePlugin_VSTUniqueID=JucePlugin_PluginCode" "-DJucePlugin_VSTCategory=kPlugCategEffect" "-DJucePlugin_Vst3Category=\"Fx\"" "-DJucePlugin_AUMainType='aufx'" "-DJucePlugin_AUSubType=JucePlugin_PluginCode" "-DJucePlugin_AUExportPrefix=GuitarixAU" "-DJucePlugin_AUExportPrefixQuoted=\"GuitarixAU\"" "-DJucePlugin_AUManufacturerCode=JucePlugin_ManufacturerCode" "-DJucePlugin_CFBundleIdentifier=https://guitarix.org/" "-DJucePlugin_AAXIdentifier=https://guitarix.org/" "-DJucePlugin_AAXManufacturerCode=JucePlugin_ManufacturerCode" "-DJucePlugin_AAXProductId=JucePlugin_PluginCode" "-DJucePlugin_AAXCategory=0" "-DJucePlugin_AAXDisableBypass=0" "-DJucePlugin_AAXDisableMultiMono=0" "-DJucePlugin_IAAType=0x61757278" "-DJucePlugin_IAASubType=JucePlugin_PluginCode" "-DJucePlugin_IAAName=\"brummer: Guitarix\"" "-DJucePlugin_VSTNumMidiInputs=16" "-DJucePlugin_VSTNumMidiOutputs=16" "-DJucePlugin_ARAContentTypes=0" "-DJucePlugin_ARATransformationFlags=0" "-DJucePlugin_ARAFactoryID=\"https://guitarix.org/.factory\"" "-DJucePlugin_ARADocumentArchiveID=\"https://guitarix.org/.aradocumentarchive.1.0.0\"" "-DJucePlugin_ARACompatibleArchiveIDs=\"\"" "-DJUCE_STANDALONE_APPLICATION=JucePlugin_Build_Standalone" "-DJUCE_MODAL_LOOPS_PERMITTED=1" "-DGUITARIX_AS_PLUGIN=1" "-DJUCER_LINUX_MAKE_6D53C8B4=1" "-DJUCE_APP_VERSION=1.0.0" "-DJUCE_APP_VERSION_HEX=0x10000" $(shell $(PKG_CONFIG) --cflags freetype2 libcurl glibmm-2.4 giomm-2.4 fftw3f sndfile  lilv-0 ) -pthread -I$(JUCE_DIR)/modules/juce_audio_processors/format_types/VST3_SDK -I$(VST3_DIR) -I../../JuceLibraryCode -I$(JUCE_DIR)/modules $(CPPFLAGS)

  JUCE_CPPFLAGS_VST3 := 
  JUCE_CFLAGS_VST3 := -fPIC -fvisibility=hidden
  JUCE_LDFLAGS_VST3 := -shared -Wl,--no-undefined
  JUCE_VST3DIR := Guitarix.vst3
  VST3_PLATFORM_ARCH := $(shell $(CXX) make_helpers/arch_detection.cpp 2>&1 | tr '\n' ' ' | sed "s/.*JUCE_ARCH \([a-zA-Z0-9_-]*\).*/\1/")
  JUCE_VST3SUBDIR := Contents/$(VST3_PLATFORM_ARCH)-linux
  JUCE_TARGET_VST3 := $(JUCE_VST3DIR)/$(JUCE_VST3SUBDIR)/Guitarix.so
  JUCE_COPYCMD_VST3 := $(JUCE_OUTDIR)/$(JUCE_VST3DIR) $(JUCE_VST3DESTDIR)

  JUCE_CPPFLAGS_STANDALONE_PLUGIN := $(shell $(PKG_CONFIG) --cflags alsa )
  JUCE_LDFLAGS_STANDALONE_PLUGIN :=  $(shell $(PKG_CONFIG) --libs alsa )
  JUCE_TARGET_STANDALONE_PLUGIN := Guitarix

  JUCE_CPPFLAGS_LV2_PLUGIN :=  -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/lilv/src -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/lilv -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/sratom -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/sord/src -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/sord -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/serd -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/lv2 -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/ 
  JUCE_CFLAGS_LV2_PLUGIN := -fPIC -fvisibility=hidden
  JUCE_LDFLAGS_LV2_PLUGIN := -shared -Wl,--no-undefined
  JUCE_LV2DIR := Guitarix.lv2
  JUCE_TARGET_LV2_PLUGIN := $(JUCE_LV2DIR)/Guitarix.so
  JUCE_LV2_FULL_PATH := $(JUCE_OUTDIR)/$(JUCE_TARGET_LV2_PLUGIN)
  JUCE_LV2DESTDIR := $(HOME)/.lv2
  JUCE_COPYCMD_LV2_PLUGIN := $(JUCE_OUTDIR)/$(JUCE_LV2DIR) $(JUCE_LV2DESTDIR)

  UNUSED_CPPFLAGS := $(shell $(PKG_CONFIG) --cflags avahi-gobject avahi-glib avahi-client )
  UNUSED_LDFLAGS := $(shell $(PKG_CONFIG) --libs avahi-gobject avahi-glib avahi-client )

  JUCE_CPPFLAGS_SHARED_CODE :=  "-DJUCE_SHARED_CODE=1"
  JUCE_TARGET_SHARED_CODE := Guitarix.a

  JUCE_CPPFLAGS_LV2_MANIFEST_HELPER := 
  JUCE_TARGET_LV2_MANIFEST_HELPER := juce_lv2_helper

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -fPIC -g -ggdb -O0 $(NAM_INCLUD_DIRS) $(RTNEURAL_INCLUD_DIRS) -I../../guitarix/trunk/src/headers -I../../guitarix/trunk/libgxwmm -I../../guitarix/trunk/libgxw -I../../guitarix/trunk -I../../guitarix/trunk/src/faust-generated -I../../guitarix/trunk/src/gx_head/engine -I../../guitarix/trunk/src/gx_head/engine/tabels -I../../guitarix/trunk/src/zita-convolver -I../../guitarix/trunk/src/zita-resampler-1.1.0 -I../../guitarix/trunk/src/zita-resampler-1.1.0/zita-resampler -I../../guitarix/trunk/src/plugins -I../../guitarix/trunk/src/plugins/genarated -I/usr/lib/x86_64-linux-gnu/ $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=gnu++17 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell $(PKG_CONFIG) --libs freetype2 libcurl glibmm-2.4 giomm-2.4 fftw3f sndfile  lilv-0 ) -fvisibility=hidden -I../../guitarix/trunk/src/headers -lrt -ldl -lpthread $(SYS_INCLUDE_FLAGS) $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

ifeq ($(CONFIG),Release)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Release
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := 
  endif

# check CPU and supported optimization flags
  ifneq ($(shell cat /proc/cpuinfo | grep sse3 ) , )
    SSE_CFLAGS = -msse3 -mfpmath=sse -mfxsr
  else ifneq ($(shell cat /proc/cpuinfo | grep sse2 ) , )
    SSE_CFLAGS = -msse2 -mfpmath=sse -mfxsr
  else ifneq ($(shell cat /proc/cpuinfo | grep sse ) , )
    SSE_CFLAGS = -msse -mfpmath=sse -mfxsr
  else ifneq ($(shell cat /proc/cpuinfo | grep ARM ) , )
    ifneq ($(shell cat /proc/cpuinfo | grep ARMv7 ) , )
      ifneq ($(shell cat /proc/cpuinfo | grep vfpd32 ) , )
        SSE_CFLAGS = -march=armv7-a -mfpu=vfpv3 
      else ifneq ($(shell cat /proc/cpuinfo | grep vfpv3 ) , )
        SSE_CFLAGS = -march=armv7-a -mfpu=vfpv3
      endif
    else
      ARMCPU = "YES"
    endif
  else ifeq ($(shell arch), aarch64)
    SSE_CFLAGS = -march=armv8-a
  else
    SSE_CFLAGS =
  endif

ifneq ($(MAKECMDGOALS),install)
ifneq ($(MAKECMDGOALS),clean)
  ifeq ($(shell arch), x86_64)
    ifneq ($(shell /usr/lib64/ld-linux-x86-64.so.2 --help 2>/dev/null | grep 'x86-64-v3 (supported, searched)') , )
      JUCE_CXXFLAGS += -march=x86-64-v3
      $(info $(yellow)INFO: $(reset)optimised for $(blue)x86-64-v3$(reset))
    else
      JUCE_CXXFLAGS += -march=x86-64
      $(info $(yellow)INFO: $(reset)optimised for $(blue)x86_64$(reset))
    endif
  else ifeq ($(shell arch), aarch64)
      JUCE_CXXFLAGS += -march=armv8
      $(info $(yellow)INFO: $(reset)optimised for $(blue)aarch64$(reset))
  endif
endif
endif

  JUCE_CPPFLAGS := $(DEPFLAGS) $(NAM_FLAGS) $(RTNEURAL_FLAGS) "-DGXV=\"$(GUITARIX_VST_VERSION)\"" "-DIRPATH=\"$(STANDALONE_IR_PATH)\"" "-DLINUX=1" "-DNDEBUG=1" "-DJUCE_USE_CURL=0" "-DJUCE_DISABLE_CAUTIOUS_PARAMETER_ID_CHECKING=1" "-DJUCE_DISPLAY_SPLASH_SCREEN=0" "-DJUCE_USE_DARK_SPLASH_SCREEN=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_devices=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_formats=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_plugin_client=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_processors=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_utils=1" "-DJUCE_MODULE_AVAILABLE_juce_core=1" "-DJUCE_MODULE_AVAILABLE_juce_data_structures=1" "-DJUCE_MODULE_AVAILABLE_juce_events=1" "-DJUCE_MODULE_AVAILABLE_juce_graphics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_extra=0" "-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1" "-DJUCE_VST3_CAN_REPLACE_VST2=0" "-DJUCE_STRICT_REFCOUNTEDPOINTER=1" "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=1" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=1" "-DJucePlugin_Build_Unity=0" "-DJucePlugin_Build_LV2=1" "-DJucePlugin_Enable_IAA=0" "-DJucePlugin_Enable_ARA=0" "-DJucePlugin_Name=\"Guitarix\"" "-DJucePlugin_Desc=\"Guitarix\"" "-DJucePlugin_Manufacturer=\"brummer\"" "-DJucePlugin_ManufacturerWebsite=\"www.brummer.com\"" "-DJucePlugin_ManufacturerEmail=\"\"" "-DJucePlugin_ManufacturerCode=0x4d616e75" "-DJucePlugin_PluginCode=0x4c736166" "-DJucePlugin_IsSynth=0" "-DJucePlugin_WantsMidiInput=1" "-DJucePlugin_ProducesMidiOutput=0" "-DJucePlugin_IsMidiEffect=0" "-DJucePlugin_EditorRequiresKeyboardFocus=0" "-DJucePlugin_Version=1.0.0" "-DJucePlugin_VersionCode=0x10000" "-DJucePlugin_VersionString=\"1.0.0\"" "-DJucePlugin_VSTUniqueID=JucePlugin_PluginCode" "-DJucePlugin_VSTCategory=kPlugCategEffect" "-DJucePlugin_Vst3Category=\"Fx\"" "-DJucePlugin_AUMainType='aufx'" "-DJucePlugin_AUSubType=JucePlugin_PluginCode" "-DJucePlugin_AUExportPrefix=GuitarixAU" "-DJucePlugin_AUExportPrefixQuoted=\"GuitarixAU\"" "-DJucePlugin_AUManufacturerCode=JucePlugin_ManufacturerCode" "-DJucePlugin_CFBundleIdentifier=https://guitarix.org/" "-DJucePlugin_AAXIdentifier=https://guitarix.org/" "-DJucePlugin_AAXManufacturerCode=JucePlugin_ManufacturerCode" "-DJucePlugin_AAXProductId=JucePlugin_PluginCode" "-DJucePlugin_AAXCategory=0" "-DJucePlugin_AAXDisableBypass=0" "-DJucePlugin_AAXDisableMultiMono=0" "-DJucePlugin_IAAType=0x61757278" "-DJucePlugin_IAASubType=JucePlugin_PluginCode" "-DJucePlugin_IAAName=\"brummer: Guitarix\"" "-DJucePlugin_VSTNumMidiInputs=16" "-DJucePlugin_VSTNumMidiOutputs=16" "-DJucePlugin_ARAContentTypes=0" "-DJucePlugin_ARATransformationFlags=0" "-DJucePlugin_ARAFactoryID=\"https://guitarix.org/.factory\"" "-DJucePlugin_ARADocumentArchiveID=\"https://guitarix.org/.aradocumentarchive.1.0.0\"" "-DJucePlugin_ARACompatibleArchiveIDs=\"\"" "-DJUCE_STANDALONE_APPLICATION=JucePlugin_Build_Standalone" "-DJUCE_MODAL_LOOPS_PERMITTED=1" "-DGUITARIX_AS_PLUGIN=1" "-DJUCER_LINUX_MAKE_6D53C8B4=1" "-DJUCE_APP_VERSION=1.0.0" "-DJUCE_APP_VERSION_HEX=0x10000" $(shell $(PKG_CONFIG) --cflags freetype2 libcurl glibmm-2.4 giomm-2.4 fftw3f sndfile  lilv-0 ) -Wl,-Bstatic -pthread -I$(JUCE_DIR)/modules/juce_audio_processors/format_types/VST3_SDK -I$(VST3_DIR) -I../../JuceLibraryCode -I$(JUCE_DIR)/modules $(CPPFLAGS)

  JUCE_CPPFLAGS_VST3 := 
  JUCE_CFLAGS_VST3 := -fPIC -fvisibility=hidden
  JUCE_LDFLAGS_VST3 := -shared -Wl,--no-undefined
  JUCE_VST3DIR := Guitarix.vst3
  VST3_PLATFORM_ARCH := $(shell $(CXX) make_helpers/arch_detection.cpp 2>&1 | tr '\n' ' ' | sed "s/.*JUCE_ARCH \([a-zA-Z0-9_-]*\).*/\1/")
  JUCE_VST3SUBDIR := Contents/$(VST3_PLATFORM_ARCH)-linux
  JUCE_TARGET_VST3 := $(JUCE_VST3DIR)/$(JUCE_VST3SUBDIR)/Guitarix.so
  JUCE_COPYCMD_VST3 := $(JUCE_OUTDIR)/$(JUCE_VST3DIR) $(JUCE_VST3DESTDIR)

  JUCE_CPPFLAGS_STANDALONE_PLUGIN := $(shell $(PKG_CONFIG) --cflags alsa )
  JUCE_LDFLAGS_STANDALONE_PLUGIN :=  $(shell $(PKG_CONFIG) --libs alsa )
  JUCE_TARGET_STANDALONE_PLUGIN := Guitarix

  JUCE_CPPFLAGS_LV2_PLUGIN :=  -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/lilv/src -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/lilv -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/sratom -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/sord/src -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/sord -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/serd -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/lv2 -I../../JuceModules/modules/juce_audio_processors/format_types/LV2_SDK/ 
  JUCE_CFLAGS_LV2_PLUGIN := -fPIC -fvisibility=hidden
  JUCE_LDFLAGS_LV2_PLUGIN := -shared -Wl,--no-undefined
  JUCE_LV2DIR := Guitarix.lv2
  JUCE_TARGET_LV2_PLUGIN := $(JUCE_LV2DIR)/Guitarix.so
  JUCE_LV2_FULL_PATH := $(JUCE_OUTDIR)/$(JUCE_TARGET_LV2_PLUGIN)
  JUCE_LV2DESTDIR := $(HOME)/.lv2
  JUCE_COPYCMD_LV2_PLUGIN := $(JUCE_OUTDIR)/$(JUCE_LV2DIR) $(JUCE_LV2DESTDIR)

  JUCE_CPPFLAGS_LV2_MANIFEST_HELPER := 
  JUCE_TARGET_LV2_MANIFEST_HELPER := juce_lv2_helper

  JUCE_CPPFLAGS_SHARED_CODE :=  "-DJUCE_SHARED_CODE=1"
  JUCE_TARGET_SHARED_CODE := Guitarix.a

  UNUSED_CPPFLAGS := $(shell $(PKG_CONFIG) --cflags avahi-gobject avahi-glib avahi-client )
  UNUSED_LDFLAGS := $(shell $(PKG_CONFIG) --libs avahi-gobject avahi-glib avahi-client )

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -fPIC -O3 $(NAM_INCLUD_DIRS) $(RTNEURAL_INCLUD_DIRS) -I../../guitarix/trunk/src/headers -I../../guitarix/trunk/libgxwmm -I../../guitarix/trunk/libgxw -I../../guitarix/trunk -I../../guitarix/trunk/src/faust-generated -I../../guitarix/trunk/src/gx_head/engine -I../../guitarix/trunk/src/gx_head/engine/tabels -I../../guitarix/trunk/src/zita-convolver -I../../guitarix/trunk/src/zita-resampler-1.1.0 -I../../guitarix/trunk/src/zita-resampler-1.1.0/zita-resampler -I../../guitarix/trunk/src/plugins -I../../guitarix/trunk/src/plugins/genarated -I/usr/lib/x86_64-linux-gnu/ $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=gnu++17 -fomit-frame-pointer -ftree-loop-linear -fno-math-errno -fno-signed-zeros -fstrength-reduce -pipe $(CXXFLAGS) $(SSE_CFLAGS)

  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell $(PKG_CONFIG) --libs freetype2 libcurl glibmm-2.4 giomm-2.4 fftw3f sndfile  lilv-0 ) -fvisibility=hidden -I../../guitarix/trunk/src/headers -lrt -ldl -lpthread $(SYS_INCLUDE_FLAGS) $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

OBJECTS_ALL := \

OBJECTS_LV2_MANIFEST_HELPER := \
  $(JUCE_OBJDIR)/juce_LV2TurtleDumpProgram_69e62a81.o \

OBJECTS_LV2_PLUGIN := \
  $(JUCE_OBJDIR)/include_juce_audio_plugin_client_LV2_7d84e0a5.o \

OBJECTS_VST3 := \
  $(JUCE_OBJDIR)/include_juce_audio_plugin_client_VST3_dd633589.o \

OBJECTS_STANDALONE_PLUGIN := \
  $(JUCE_OBJDIR)/include_juce_audio_plugin_client_Standalone_1a871192.o \

OBJECTS_NAM_CODE := \
  $(JUCE_OBJDIR)/activations.o \
  $(JUCE_OBJDIR)/convnet.o \
  $(JUCE_OBJDIR)/dsp.o \
  $(JUCE_OBJDIR)/get_dsp.o \
  $(JUCE_OBJDIR)/lstm.o \
  $(JUCE_OBJDIR)/util.o \
  $(JUCE_OBJDIR)/wavenet.o \

OBJECTS_RTNEURAL_CODE := \
  $(JUCE_OBJDIR)/RTNeural.o

OBJECTS_SHARED_CODE := \
  $(JUCE_OBJDIR)/abgate_3dab4bb7.o \
  $(JUCE_OBJDIR)/aclipper_274cf3d7.o \
  $(JUCE_OBJDIR)/astrofuzz_8c50b14b.o \
  $(JUCE_OBJDIR)/axface_c72ddd4f.o \
  $(JUCE_OBJDIR)/bassboom_a881d19.o \
  $(JUCE_OBJDIR)/bfuzz_d1208148.o \
  $(JUCE_OBJDIR)/bmp_f167cdb4.o \
  $(JUCE_OBJDIR)/bossds1_dd8980c4.o \
  $(JUCE_OBJDIR)/buffb_cfe089ca.o \
  $(JUCE_OBJDIR)/buzz_49e75e70.o \
  $(JUCE_OBJDIR)/coverd_9ed62a36.o \
  $(JUCE_OBJDIR)/cstb_7b047285.o \
  $(JUCE_OBJDIR)/eldist_de7416f6.o \
  $(JUCE_OBJDIR)/ffreak_e0339566.o \
  $(JUCE_OBJDIR)/flanger_gx_baa4f914.o \
  $(JUCE_OBJDIR)/foxeylady_d1bc00ec.o \
  $(JUCE_OBJDIR)/fumaster_37af8212.o \
  $(JUCE_OBJDIR)/fuzzdrive_27d0389e.o \
  $(JUCE_OBJDIR)/fuzzfacefm_ba8dec90.o \
  $(JUCE_OBJDIR)/fuzzfacerm_bb37069c.o \
  $(JUCE_OBJDIR)/gcb_95_e1e77dce.o \
  $(JUCE_OBJDIR)/gx_convolver_2fcbb5a9.o \
  $(JUCE_OBJDIR)/gx_engine_f500b135.o \
  $(JUCE_OBJDIR)/gx_engine_audio_37228c8c.o \
  $(JUCE_OBJDIR)/gx_faust_plugins_4afd0c13.o \
  $(JUCE_OBJDIR)/gx_internal_plugins_9f9f4b91.o \
  $(JUCE_OBJDIR)/gx_internal_ui_plugins_eafcae6e.o \
  $(JUCE_OBJDIR)/gx_jack_wrapper_f7417426.o \
  $(JUCE_OBJDIR)/gx_json_dd7f185b.o \
  $(JUCE_OBJDIR)/gx_logging_ebc4d916.o \
  $(JUCE_OBJDIR)/gx_main_7a86e84b.o \
  $(JUCE_OBJDIR)/gx_paramtable_acdeb3d4.o \
  $(JUCE_OBJDIR)/gx_pitch_tracker_b232870.o \
  $(JUCE_OBJDIR)/gx_pluginloader_26957b79.o \
  $(JUCE_OBJDIR)/gx_preset_f7ee0352.o \
  $(JUCE_OBJDIR)/gx_resampler_20f75cac.o \
  $(JUCE_OBJDIR)/gx_system_c57d6aa2.o \
  $(JUCE_OBJDIR)/hfb_fb3bdfb5.o \
  $(JUCE_OBJDIR)/hogsfoot_c44f64e2.o \
  $(JUCE_OBJDIR)/hornet_bf3f1a31.o \
  $(JUCE_OBJDIR)/jcm800pre_3c615609.o \
  $(JUCE_OBJDIR)/jsonrpc_86bd3962.o \
  $(JUCE_OBJDIR)/ladspaback_d9977da1.o \
  $(JUCE_OBJDIR)/ladspaplugin_6c0a94ad.o \
  $(JUCE_OBJDIR)/lpbboost_1d31ee7e.o \
  $(JUCE_OBJDIR)/machine_fe310c4b.o \
  $(JUCE_OBJDIR)/mbc_38c312b.o \
  $(JUCE_OBJDIR)/mbchor_3af773a6.o \
  $(JUCE_OBJDIR)/mbclipper_eafced81.o \
  $(JUCE_OBJDIR)/mbcs_6e188b3e.o \
  $(JUCE_OBJDIR)/mbd_38ca58a.o \
  $(JUCE_OBJDIR)/mbdel_5603d483.o \
  $(JUCE_OBJDIR)/mbe_38d19e9.o \
  $(JUCE_OBJDIR)/metronome_2053bd67.o \
  $(JUCE_OBJDIR)/mole_84c001a8.o \
  $(JUCE_OBJDIR)/muff_8ea8fcbb.o \
  $(JUCE_OBJDIR)/mxrdist_1a614e6c.o \
  $(JUCE_OBJDIR)/rangem_fc0cf413.o \
  $(JUCE_OBJDIR)/resampler-table_7cb6c3e.o \
  $(JUCE_OBJDIR)/resampler_a044e05f.o \
  $(JUCE_OBJDIR)/reversedelay_a7711462.o \
  $(JUCE_OBJDIR)/ringbuffer_42114019.o \
  $(JUCE_OBJDIR)/rolandwah_71068d23.o \
  $(JUCE_OBJDIR)/ruin_97560e7b.o \
  $(JUCE_OBJDIR)/scream_300a9a94.o \
  $(JUCE_OBJDIR)/susta_ccc8595b.o \
  $(JUCE_OBJDIR)/ts9sim_93733ac6.o \
  $(JUCE_OBJDIR)/tunerswitcher_8af09ec0.o \
  $(JUCE_OBJDIR)/vibe_afe046ed.o \
  $(JUCE_OBJDIR)/vumeter_c5d6ccaf.o \
  $(JUCE_OBJDIR)/vumeter_st_13fca2cd.o \
  $(JUCE_OBJDIR)/zita-convolver_886c041b.o \
  $(JUCE_OBJDIR)/zita_rev1_84405f08.o \
  #$(JUCE_OBJDIR)/avahi_register_a890e2da.o \

JUCE_UI_SHARED_CODE := \
  $(JUCE_OBJDIR)/JuceUiBuilder_14cda45b.o \
  $(JUCE_OBJDIR)/PluginEditor_94d4fb09.o \
  $(JUCE_OBJDIR)/GuitarixEditor_cb2a0a8f.o \
  $(JUCE_OBJDIR)/GuitarixProcessor_54f35e3a.o \
  $(JUCE_OBJDIR)/TunerDisplay_6dee1c1a.o \

JUCE_SHARED_CODE := \
  $(JUCE_OBJDIR)/include_juce_audio_basics_8a4e984a.o \
  $(JUCE_OBJDIR)/include_juce_audio_devices_63111d02.o \
  $(JUCE_OBJDIR)/include_juce_audio_formats_15f82001.o \
  $(JUCE_OBJDIR)/include_juce_audio_plugin_client_ARA_31a052ed.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_10c03666.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_ara_2a4c6ef7.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_lv2_libs_12bdca08.o \
  $(JUCE_OBJDIR)/include_juce_audio_utils_9f9fb2d6.o \
  $(JUCE_OBJDIR)/include_juce_core_f26d17db.o \
  $(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o \
  $(JUCE_OBJDIR)/include_juce_events_fd7d695.o \
  $(JUCE_OBJDIR)/include_juce_graphics_f817e147.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o \
 # $(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o \

ifndef USE_SYSTEM_JUCE
JUCE_SHARED_CODE += \
  $(JUCE_OBJDIR)/include_juce_audio_plugin_client_utils_e32edaee.o \

endif

.PHONY: clean all strip install VST3 Standalone LV2 LV2_MANIFEST_HELPER

all : VST3 # Standalone

VST3 : $(JUCE_OUTDIR)/$(JUCE_TARGET_VST3)
Standalone : $(JUCE_OUTDIR)/$(JUCE_TARGET_STANDALONE_PLUGIN)
LV2 : $(JUCE_OUTDIR)/$(JUCE_TARGET_LV2_PLUGIN)
LV2_MANIFEST_HELPER : $(JUCE_OUTDIR)/$(JUCE_TARGET_LV2_MANIFEST_HELPER)

inform :
	@echo "$(yellow)INFO:$(reset) Compiling modules $(purple)\n"

finish:
	@echo "$(ceol)$(ceol)$(yellow)INFO:$(reset) Compiling modules - $(purple)done$(reset)"

# tracking header files
-include $(JUCE_UI_SHARED_CODE:%.o=%.d)
-include $(JUCE_SHARED_CODE:%.o=%.d)
-include $(OBJECTS_SHARED_CODE:%.o=%.d)
-include $(OBJECTS_NAM_CODE:%.o=%.d)
-include $(OBJECTS_RTNEURAL_CODE:%.o=%.d)
-include $(OBJECTS_LV2_PLUGIN:%.o=%.d)
-include $(OBJECTS_LV2_MANIFEST_HELPER:%.o=%.d)

install :
ifneq ("$(wildcard $(JUCE_OUTDIR)/$(JUCE_VST3DIR))","")
	$(info $(yellow)INFO: $(reset)install Guitarix.vst3 to $(blue)$(JUCE_VST3DESTDIR)$(reset))
	$(V_AT)[ ! "$(JUCE_VST3DESTDIR)" ] || (mkdir -p $(JUCE_VST3DESTDIR) && cp -R $(JUCE_COPYCMD_VST3))
endif
ifneq ("$(wildcard $(JUCE_OUTDIR)/$(JUCE_LV2DIR))","")
	$(info $(yellow)INFO: $(reset)install Guitarix.lv2 to $(blue)$(JUCE_LV2DESTDIR)$(reset))
	$(V_AT)[ ! "$(JUCE_LV2DESTDIR)" ] || (mkdir -p $(JUCE_LV2DESTDIR) && cp -R $(JUCE_COPYCMD_LV2_PLUGIN))
endif

$(JUCE_OUTDIR)/$(JUCE_TARGET_VST3) : inform $(OBJECTS_VST3) $(RESOURCES) $(JUCE_OUTDIR)/$(JUCE_TARGET_SHARED_CODE)
	@command -v $(PKG_CONFIG) >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@$(PKG_CONFIG) --print-errors freetype2 libcurl glibmm-2.4 giomm-2.4 fftw3f sndfile
	@echo "$(blue)Linking Guitarix - VST3$(reset)"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)/$(JUCE_VST3DIR)/$(JUCE_VST3SUBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)/$(JUCE_VST3DIR)/$(JUCE_VST3SUBDIR)/gx_head/sounds/amps
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)/$(JUCE_VST3DIR)/$(JUCE_VST3SUBDIR)/gx_head/sounds/bands
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)/$(JUCE_VST3DIR)/$(JUCE_VST3SUBDIR)/gx_head/factorysettings
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_VST3) $(OBJECTS_VST3) $(JUCE_OUTDIR)/$(JUCE_TARGET_SHARED_CODE) $(JUCE_LDFLAGS)  $(JUCE_LDFLAGS_VST3) $(RESOURCES) $(TARGET_ARCH)
ifneq ($(CONFIG),Debug)
	@echo "$(blue)Stripping Guitarix - VST3$(reset)"
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(JUCE_TARGET_VST3)
endif
	-$(V_AT)cp -R ../../guitarix/trunk/IR/greathall.wav $(JUCE_OUTDIR)/$(JUCE_VST3DIR)/$(JUCE_VST3SUBDIR)/gx_head/sounds/
	-$(V_AT)cp -R ../../guitarix/trunk/IR/BestPlugins_Amps/* $(JUCE_OUTDIR)/$(JUCE_VST3DIR)/$(JUCE_VST3SUBDIR)/gx_head/sounds/amps
	-$(V_AT)cp -R ../../guitarix/trunk/IR/BestPlugins_Bands/* $(JUCE_OUTDIR)/$(JUCE_VST3DIR)/$(JUCE_VST3SUBDIR)/gx_head/sounds/bands
	-$(V_AT)cp -R ../../guitarix/trunk/factorysettings/dirlist.js $(JUCE_OUTDIR)/$(JUCE_VST3DIR)/$(JUCE_VST3SUBDIR)/gx_head/factorysettings/dirlist.js
	-$(V_AT)cp -R ../../guitarix/trunk/factorysettings/Musiclab.gx $(JUCE_OUTDIR)/$(JUCE_VST3DIR)/$(JUCE_VST3SUBDIR)/gx_head/factorysettings/Musiclab.gx

$(JUCE_OUTDIR)/$(JUCE_TARGET_LV2_PLUGIN) :  inform $(OBJECTS_LV2_PLUGIN) $(RESOURCES) $(JUCE_OUTDIR)/$(JUCE_TARGET_SHARED_CODE) $(JUCE_OUTDIR)/$(JUCE_TARGET_LV2_MANIFEST_HELPER)
	@command -v $(PKG_CONFIG) >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@$(PKG_CONFIG) --print-errors freetype2 libcurl glibmm-2.4 giomm-2.4 fftw3f sndfile lilv-0  
	@echo "$(blue)Linking Guitarix - LV2$(reset)"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)/$(JUCE_LV2DIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)/$(JUCE_LV2DIR)/gx_head/sounds/amps
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)/$(JUCE_LV2DIR)/gx_head/sounds/bands
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)/$(JUCE_LV2DIR)/gx_head/factorysettings
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_LV2_PLUGIN) $(OBJECTS_LV2_PLUGIN) $(JUCE_OUTDIR)/$(JUCE_TARGET_SHARED_CODE) $(JUCE_LDFLAGS) $(JUCE_LDFLAGS_LV2_PLUGIN) $(RESOURCES) $(TARGET_ARCH)
	$(V_AT) $(JUCE_OUTDIR)/$(JUCE_TARGET_LV2_MANIFEST_HELPER) $(JUCE_LV2_FULL_PATH)
ifneq ($(CONFIG),Debug)
	@echo "$(blue)Stripping Guitarix - LV2$(reset)"
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(JUCE_TARGET_LV2_PLUGIN)
endif
	-$(V_AT)cp -R ../../guitarix/trunk/IR/greathall.wav $(JUCE_OUTDIR)/$(JUCE_LV2DIR)/gx_head/sounds/
	-$(V_AT)cp -R ../../guitarix/trunk/IR/BestPlugins_Amps/* $(JUCE_OUTDIR)/$(JUCE_LV2DIR)/gx_head/sounds/amps
	-$(V_AT)cp -R ../../guitarix/trunk/IR/BestPlugins_Bands/* $(JUCE_OUTDIR)/$(JUCE_LV2DIR)/gx_head/sounds/bands
	-$(V_AT)cp -R ../../guitarix/trunk/factorysettings/dirlist.js $(JUCE_OUTDIR)/$(JUCE_LV2DIR)/gx_head/factorysettings/dirlist.js
	-$(V_AT)cp -R ../../guitarix/trunk/factorysettings/Musiclab.gx $(JUCE_OUTDIR)/$(JUCE_LV2DIR)/gx_head/factorysettings/Musiclab.gx

$(JUCE_OUTDIR)/$(JUCE_TARGET_LV2_MANIFEST_HELPER) : $(OBJECTS_LV2_MANIFEST_HELPER) $(RESOURCES) $(JUCE_OUTDIR)/$(JUCE_TARGET_SHARED_CODE)
	@command -v $(PKG_CONFIG) >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@$(PKG_CONFIG) --print-errors alsa freetype2 libcurl zlib libjpeg libpng flac vorbis vorbisfile vorbisenc ogg jack glibmm-2.4 giomm-2.4 fftw3f sndfile eigen3 # avahi-gobject avahi-glib avahi-client
	@echo Linking "$(blue)Guitarix - LV2 Manifest Helper$(reset)"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_LV2_MANIFEST_HELPER) $(OBJECTS_LV2_MANIFEST_HELPER) $(JUCE_OUTDIR)/$(JUCE_TARGET_SHARED_CODE) $(JUCE_LDFLAGS) $(RESOURCES) $(TARGET_ARCH)

$(JUCE_OUTDIR)/$(JUCE_TARGET_STANDALONE_PLUGIN) : inform $(OBJECTS_STANDALONE_PLUGIN) $(RESOURCES) $(JUCE_OUTDIR)/$(JUCE_TARGET_SHARED_CODE)
	@command -v $(PKG_CONFIG) >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@$(PKG_CONFIG) --print-errors alsa freetype2 libcurl glibmm-2.4 giomm-2.4 fftw3f sndfile  lilv-0 
	@echo "$(blue)Linking Guitarix - Standalone Plugin$(reset)"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_STANDALONE_PLUGIN) $(OBJECTS_STANDALONE_PLUGIN) $(JUCE_OUTDIR)/$(JUCE_TARGET_SHARED_CODE) $(JUCE_LDFLAGS) $(JUCE_LDFLAGS_STANDALONE_PLUGIN) $(RESOURCES) $(TARGET_ARCH)
ifneq ($(CONFIG),Debug)
	@echo "$(blue)Stripping Guitarix - Standalone Plugin$(reset)"
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(JUCE_TARGET_STANDALONE_PLUGIN)
endif

$(JUCE_OUTDIR)/$(JUCE_TARGET_SHARED_CODE) : $(JUCE_SHARED_CODE) $(JUCE_UI_SHARED_CODE) $(OBJECTS_SHARED_CODE) $(OBJECTS_NAM_CODE) $(OBJECTS_RTNEURAL_CODE) $(RESOURCES) finish
	@command -v $(PKG_CONFIG) >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@$(PKG_CONFIG) --print-errors freetype2 libcurl glibmm-2.4 giomm-2.4 fftw3f sndfile  lilv-0 
	@echo "$(blue)Linking Guitarix - Shared Code"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	$(V_AT)$(AR) -rcs $(JUCE_OUTDIR)/$(JUCE_TARGET_SHARED_CODE) $(JUCE_SHARED_CODE) $(JUCE_UI_SHARED_CODE) $(OBJECTS_SHARED_CODE) $(OBJECTS_NAM_CODE)

$(JUCE_OBJDIR)/include_juce_audio_plugin_client_LV2_7d84e0a5.o: ../../JuceLibraryCode/include_juce_audio_plugin_client_LV2.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_audio_plugin_client_LV2.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_LV2_PLUGIN) $(JUCE_CFLAGS_LV2_PLUGIN) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_plugin_client_VST3_dd633589.o: ../../JuceLibraryCode/include_juce_audio_plugin_client_VST3.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_audio_plugin_client_VST3.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_VST3) $(JUCE_CFLAGS_VST3) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_plugin_client_Standalone_1a871192.o: ../../JuceLibraryCode/include_juce_audio_plugin_client_Standalone.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_audio_plugin_client_Standalone.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_STANDALONE_PLUGIN) $(JUCE_CFLAGS_STANDALONE_PLUGIN) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PluginEditor_94d4fb09.o: ../../Source/PluginEditor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling PluginEditor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RTNeural.o: ../../guitarix/trunk/src/RTNeural/RTNeural/RTNeural/RTNeural.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling RTNeural.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) -Ofast -Wno-sign-compare -Wno-reorder -Wno-infinite-recursion -fno-fat-lto-objects $(JUCE_CPPFLAGS_SHARED_CODE) $(RTNEURAL_INCLUD_DIRS) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/activations.o: ../../guitarix/trunk/src/NAM/NeuralAmpModelerCore/NAM/activations.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling activations.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) -Ofast -Wno-sign-compare -fno-fat-lto-objects $(JUCE_CPPFLAGS_SHARED_CODE) $(NAM_INCLUD_DIRS) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/convnet.o: ../../guitarix/trunk/src/NAM/NeuralAmpModelerCore/NAM/convnet.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling convnet.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) -Ofast -Wno-sign-compare -fno-fat-lto-objects $(JUCE_CPPFLAGS_SHARED_CODE) $(NAM_INCLUD_DIRS) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/dsp.o: ../../guitarix/trunk/src/NAM/NeuralAmpModelerCore/NAM/dsp.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling dsp.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) -Ofast -Wno-sign-compare -fno-fat-lto-objects $(JUCE_CPPFLAGS_SHARED_CODE) $(NAM_INCLUD_DIRS) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/get_dsp.o: ../../guitarix/trunk/src/NAM/NeuralAmpModelerCore/NAM/get_dsp.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling get_dsp.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) -Ofast -Wno-sign-compare -fno-fat-lto-objects $(JUCE_CPPFLAGS_SHARED_CODE) $(NAM_INCLUD_DIRS) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lstm.o: ../../guitarix/trunk/src/NAM/NeuralAmpModelerCore/NAM/lstm.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling lstm.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) -Ofast -Wno-sign-compare -fno-fat-lto-objects $(JUCE_CPPFLAGS_SHARED_CODE) $(NAM_INCLUD_DIRS) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/util.o: ../../guitarix/trunk/src/NAM/NeuralAmpModelerCore/NAM/util.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling util.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) -Ofast -Wno-sign-compare -fno-fat-lto-objects $(JUCE_CPPFLAGS_SHARED_CODE) $(NAM_INCLUD_DIRS) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/wavenet.o: ../../guitarix/trunk/src/NAM/NeuralAmpModelerCore/NAM/wavenet.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling wavenet.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) -Ofast -Wno-sign-compare -fno-fat-lto-objects $(JUCE_CPPFLAGS_SHARED_CODE) $(NAM_INCLUD_DIRS) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"


$(JUCE_OBJDIR)/abgate_3dab4bb7.o: ../../guitarix/trunk/src/plugins/abgate.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling abgate.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/aclipper_274cf3d7.o: ../../guitarix/trunk/src/plugins/generated/aclipper.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling aclipper.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/astrofuzz_8c50b14b.o: ../../guitarix/trunk/src/plugins/generated/astrofuzz.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling astrofuzz.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/avahi_register_a890e2da.o: ../../guitarix/trunk/src/gx_head/engine/avahi_register.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling avahi_register.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/axface_c72ddd4f.o: ../../guitarix/trunk/src/plugins/generated/axface.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling axface.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/bassboom_a881d19.o: ../../guitarix/trunk/src/plugins/generated/bassboom.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling bassboom.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/bfuzz_d1208148.o: ../../guitarix/trunk/src/plugins/generated/bfuzz.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling bfuzz.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/bmp_f167cdb4.o: ../../guitarix/trunk/src/plugins/generated/bmp.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling bmp.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/bossds1_dd8980c4.o: ../../guitarix/trunk/src/plugins/generated/bossds1.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling bossds1.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/buffb_cfe089ca.o: ../../guitarix/trunk/src/plugins/generated/buffb.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling buffb.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/buzz_49e75e70.o: ../../guitarix/trunk/src/plugins/generated/buzz.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling buzz.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/coverd_9ed62a36.o: ../../guitarix/trunk/src/plugins/generated/coverd.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling coverd.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cstb_7b047285.o: ../../guitarix/trunk/src/plugins/generated/cstb.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling cstb.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/eldist_de7416f6.o: ../../guitarix/trunk/src/plugins/generated/eldist.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling eldist.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ffreak_e0339566.o: ../../guitarix/trunk/src/plugins/generated/ffreak.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling ffreak.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/flanger_gx_baa4f914.o: ../../guitarix/trunk/src/plugins/generated/flanger_gx.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling flanger_gx.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/foxeylady_d1bc00ec.o: ../../guitarix/trunk/src/plugins/generated/foxeylady.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling foxeylady.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/fumaster_37af8212.o: ../../guitarix/trunk/src/plugins/generated/fumaster.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling fumaster.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/fuzzdrive_27d0389e.o: ../../guitarix/trunk/src/plugins/generated/fuzzdrive.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling fuzzdrive.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/fuzzfacefm_ba8dec90.o: ../../guitarix/trunk/src/plugins/generated/fuzzfacefm.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling fuzzfacefm.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/fuzzfacerm_bb37069c.o: ../../guitarix/trunk/src/plugins/generated/fuzzfacerm.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling fuzzfacerm.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gcb_95_e1e77dce.o: ../../guitarix/trunk/src/plugins/generated/gcb_95.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gcb_95.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/GuitarixEditor_cb2a0a8f.o: ../../Source/GuitarixEditor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling GuitarixEditor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/GuitarixProcessor_54f35e3a.o: ../../Source/GuitarixProcessor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling GuitarixProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_convolver_2fcbb5a9.o: ../../guitarix/trunk/src/gx_head/engine/gx_convolver.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_convolver.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_engine_f500b135.o: ../../guitarix/trunk/src/gx_head/engine/gx_engine.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_engine.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_engine_audio_37228c8c.o: ../../guitarix/trunk/src/gx_head/engine/gx_engine_audio.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_engine_audio.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_faust_plugins_4afd0c13.o: ../../guitarix/trunk/src/gx_head/engine/gx_faust_plugins.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_faust_plugins.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_internal_plugins_9f9f4b91.o: ../../guitarix/trunk/src/gx_head/engine/gx_internal_plugins.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_internal_plugins.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_internal_ui_plugins_eafcae6e.o: ../../guitarix/trunk/src/gx_head/engine/gx_internal_ui_plugins.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_internal_ui_plugins.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_jack_wrapper_f7417426.o: ../../guitarix/trunk/src/gx_head/engine/gx_jack_wrapper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_jack_wrapper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_json_dd7f185b.o: ../../guitarix/trunk/src/gx_head/engine/gx_json.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_json.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_logging_ebc4d916.o: ../../guitarix/trunk/src/gx_head/engine/gx_logging.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_logging.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_main_7a86e84b.o: ../../guitarix/trunk/src/gx_head/gui/gx_main.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_main.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_paramtable_acdeb3d4.o: ../../guitarix/trunk/src/gx_head/engine/gx_paramtable.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_paramtable.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_pitch_tracker_b232870.o: ../../guitarix/trunk/src/gx_head/engine/gx_pitch_tracker.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_pitch_tracker.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_pluginloader_26957b79.o: ../../guitarix/trunk/src/gx_head/engine/gx_pluginloader.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_pluginloader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_preset_f7ee0352.o: ../../guitarix/trunk/src/gx_head/engine/gx_preset.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_preset.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_resampler_20f75cac.o: ../../guitarix/trunk/src/gx_head/engine/gx_resampler.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_resampler.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/gx_system_c57d6aa2.o: ../../guitarix/trunk/src/gx_head/engine/gx_system.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling gx_system.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/hfb_fb3bdfb5.o: ../../guitarix/trunk/src/plugins/generated/hfb.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling hfb.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/hogsfoot_c44f64e2.o: ../../guitarix/trunk/src/plugins/generated/hogsfoot.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling hogsfoot.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/hornet_bf3f1a31.o: ../../guitarix/trunk/src/plugins/generated/hornet.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling hornet.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jcm800pre_3c615609.o: ../../guitarix/trunk/src/plugins/jcm800pre.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling jcm800pre.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/jsonrpc_86bd3962.o: ../../guitarix/trunk/src/gx_head/engine/jsonrpc.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling jsonrpc.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/JuceUiBuilder_14cda45b.o: ../../Source/JuceUiBuilder.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling JuceUiBuilder.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TunerDisplay_6dee1c1a.o:  ../../Source/TunerDisplay.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling TunerDisplay.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ladspaback_d9977da1.o: ../../guitarix/trunk/src/gx_head/engine/ladspaback.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling ladspaback.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ladspaplugin_6c0a94ad.o: ../../guitarix/trunk/src/gx_head/engine/ladspaplugin.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling ladspaplugin.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/lpbboost_1d31ee7e.o: ../../guitarix/trunk/src/plugins/generated/lpbboost.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling lpbboost.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/machine_fe310c4b.o: ../../guitarix/trunk/src/gx_head/gui/machine.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling machine.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/mbc_38c312b.o: ../../guitarix/trunk/src/plugins/generated/mbc.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling mbc.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/mbchor_3af773a6.o: ../../guitarix/trunk/src/plugins/generated/mbchor.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling mbchor.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/mbclipper_eafced81.o: ../../guitarix/trunk/src/plugins/generated/mbclipper.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling mbclipper.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/mbcs_6e188b3e.o: ../../guitarix/trunk/src/plugins/generated/mbcs.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling mbcs.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/mbd_38ca58a.o: ../../guitarix/trunk/src/plugins/generated/mbd.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling mbd.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/mbdel_5603d483.o: ../../guitarix/trunk/src/plugins/generated/mbdel.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling mbdel.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/mbe_38d19e9.o: ../../guitarix/trunk/src/plugins/generated/mbe.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling mbe.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/metronome_2053bd67.o: ../../guitarix/trunk/src/plugins/metronome.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling metronome.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/mole_84c001a8.o: ../../guitarix/trunk/src/plugins/generated/mole.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling mole.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/muff_8ea8fcbb.o: ../../guitarix/trunk/src/plugins/generated/muff.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling muff.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/mxrdist_1a614e6c.o: ../../guitarix/trunk/src/plugins/generated/mxrdist.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling mxrdist.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rangem_fc0cf413.o: ../../guitarix/trunk/src/plugins/generated/rangem.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling rangem.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/resampler-table_7cb6c3e.o: ../../guitarix/trunk/src/zita-resampler-1.1.0/resampler-table.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling resampler-table.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/resampler_a044e05f.o: ../../guitarix/trunk/src/zita-resampler-1.1.0/resampler.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling resampler.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/reversedelay_a7711462.o: ../../guitarix/trunk/src/plugins/reversedelay.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling reversedelay.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ringbuffer_42114019.o: ../../Source/ringbuffer.c
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling ringbuffer.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/rolandwah_71068d23.o: ../../guitarix/trunk/src/plugins/generated/rolandwah.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling rolandwah.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ruin_97560e7b.o: ../../guitarix/trunk/src/plugins/generated/ruin.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling ruin.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/scream_300a9a94.o: ../../guitarix/trunk/src/plugins/generated/scream.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling scream.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/susta_ccc8595b.o: ../../guitarix/trunk/src/plugins/generated/susta.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling susta.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ts9sim_93733ac6.o: ../../guitarix/trunk/src/plugins/generated/ts9sim.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling ts9sim.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/tunerswitcher_8af09ec0.o: ../../guitarix/trunk/src/gx_head/engine/tunerswitcher.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling tunerswitcher.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/vibe_afe046ed.o: ../../guitarix/trunk/src/plugins/vibe.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling vibe.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/vumeter_c5d6ccaf.o: ../../guitarix/trunk/src/plugins/vumeter.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling vumeter.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/vumeter_st_13fca2cd.o: ../../guitarix/trunk/src/plugins/vumeter_st.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling vumeter_st.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/zita-convolver_886c041b.o: ../../guitarix/trunk/src/zita-convolver/zita-convolver.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling zita-convolver.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/zita_rev1_84405f08.o: ../../guitarix/trunk/src/plugins/generated/zita_rev1.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling zita_rev1.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_basics_8a4e984a.o: ../../JuceLibraryCode/include_juce_audio_basics.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_audio_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_devices_63111d02.o: ../../JuceLibraryCode/include_juce_audio_devices.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_audio_devices.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_formats_15f82001.o: ../../JuceLibraryCode/include_juce_audio_formats.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_audio_formats.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_plugin_client_ARA_31a052ed.o: ../../JuceLibraryCode/include_juce_audio_plugin_client_ARA.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_audio_plugin_client_ARA.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_plugin_client_utils_e32edaee.o: ../../JuceLibraryCode/include_juce_audio_plugin_client_utils.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_audio_plugin_client_utils.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_10c03666.o: ../../JuceLibraryCode/include_juce_audio_processors.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_audio_processors.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_ara_2a4c6ef7.o: ../../JuceLibraryCode/include_juce_audio_processors_ara.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_audio_processors_ara.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_lv2_libs_12bdca08.o: ../../JuceLibraryCode/include_juce_audio_processors_lv2_libs.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_audio_processors_lv2_libs.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_utils_9f9fb2d6.o: ../../JuceLibraryCode/include_juce_audio_utils.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_audio_utils.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_core_f26d17db.o: ../../JuceLibraryCode/include_juce_core.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_core.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o: ../../JuceLibraryCode/include_juce_data_structures.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_data_structures.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_events_fd7d695.o: ../../JuceLibraryCode/include_juce_events.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_events.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_f817e147.o: ../../JuceLibraryCode/include_juce_graphics.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_graphics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o: ../../JuceLibraryCode/include_juce_gui_basics.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_gui_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o: ../../JuceLibraryCode/include_juce_gui_extra.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling include_juce_gui_extra.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_SHARED_CODE) $(JUCE_CFLAGS_SHARED_CODE) -o "$@" -c "$<"

$(JUCE_OBJDIR)/juce_LV2TurtleDumpProgram_69e62a81.o: ../../JuceModules/modules/juce_audio_plugin_client/LV2/juce_LV2TurtleDumpProgram.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@$(ECHO) "Compiling juce_LV2TurtleDumpProgram.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_LV2_MANIFEST_HELPER) $(JUCE_CFLAGS_LV2_MANIFEST_HELPER) -o "$@" -c "$<"

clean:
	@echo "$(yellow)INFO: $(reset)Cleaning Guitarix"
	$(V_AT)$(CLEANCMD)

strip:
	@echo Stripping Guitarix
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(TARGET)

-include $(OBJECTS_VST3:%.o=%.d)
-include $(OBJECTS_STANDALONE_PLUGIN:%.o=%.d)
-include $(OBJECTS_SHARED_CODE:%.o=%.d)
-include $(OBJECTS_NAM_CODE:%.o=%.d)
-include $(OBJECTS_RTNEURAL_CODE:%.o=%.d)
-include $(OBJECTS_LV2_PLUGIN:%.o=%.d)
-include $(OBJECTS_LV2_MANIFEST_HELPER:%.o=%.d)
