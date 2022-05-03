PACKAGE = coinc_proc
CINTGEN = gencintlinkdef.pl

CXXFLAGS = -O2 -g -fPIC -I.  -I$(ROOTSYS)/include -I$(ONLINE_MAIN)/include 

LDFLAGS = -L$(ONLINE_MAIN)/lib -lpmonitor


HDRFILES = $(PACKAGE).h
LINKFILE = $(PACKAGE)LinkDef.h

ADDITIONAL_SOURCES = pet_datum.cc


SO = lib$(PACKAGE).so

$(SO) : $(PACKAGE).cc $(PACKAGE)_dict.C $(ADDITIONAL_SOURCES) $(LINKFILE)
	$(CXX) $(CXXFLAGS) -o $@ -shared  $<  $(ADDITIONAL_SOURCES) $(PACKAGE)_dict.C $(LDFLAGS) 


$(PACKAGE)_dict.C : $(HDRFILES) $(LINKFILE)
	rootcint -f $@  -c $(CXXFLAGS) $^


#$(LINKFILE): $(HDRFILES)
#	$(CINTGEN) $(HDRFILES) > $@

.PHONY: clean

clean: 
	rm -f $(SO) $(LINKFILE) $(PACKAGE)_dict.C $(PACKAGE)_dict.h
