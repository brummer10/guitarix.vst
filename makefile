
ifneq ($(TERM),)
  red :=$(shell tput setaf 1)
  yellow :=$(shell tput setaf 3)
  blue :=$(shell tput setaf 4)
  reset :=$(shell tput sgr0)
  ceol :=$(shell tput cuu1 && tput el)
else
  red :=$()
  yellow :=$()
  blue :=$()
  reset :=$()
  ceol :=$()
endif

SUBDIR := Builds/LinuxMakefile

NOGOAL := install

.PHONY: $(SUBDIR) 

$(MAKECMDGOALS) recurse: $(SUBDIR)

clean:

check-and-reinit-submodules :
ifeq (,$(filter $(NOGOAL),$(MAKECMDGOALS)))
ifeq (,$(findstring clean,$(MAKECMDGOALS)))
	@if git submodule status 2>/dev/null | egrep -q '^[-]|^[+]' ; then \
		echo "$(yellow)INFO: $(red)Need to reinitialize git submodules$(reset)"; \
		git submodule update --init --recursive; \
		echo "$(ceol)$(yellow)INFO: $(reset)Need to reinitialize git submodules - $(blue)Done$(reset)"; \
	else echo "$(yellow)INFO: $(reset)Submodule up to date"; \
	fi
endif
endif

$(SUBDIR): check-and-reinit-submodules
	@exec $(MAKE) --no-print-directory -C $@ $(MAKECMDGOALS)
ifneq ($(MAKECMDGOALS),install)
ifneq ($(MAKECMDGOALS),clean)
	@echo "$(yellow)INFO:$(reset) Build finish, now run $(blue)make install$(reset)"
endif
endif
