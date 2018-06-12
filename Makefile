## the Makefile
GCC= gcc
GXX= g++
CC= $(GCC)
CXX= $(GXX)
CCACHE= ccache
MARKDOWN= markdown
WARNFLAGS= -Wall -Wextra -Wmissing-prototypes -Wstack-usage=1500 -fdiagnostics-color=auto
## -Wmissing-prototypes dont exist for g++
SKIPCXXWARNFLAGS= -Wmissing-prototypes
OPTIMFLAGS= -O1 -g3
PLUGINFLAGS=
CFLAGS= -std=gnu11 $(PLUGINFLAGS) $(WARNFLAGS) $(PREPROFLAGS) $(OPTIMFLAGS)
CXXFLAGS= -std=gnu++14  $(PLUGINFLAGS) $(filter-out $(SKIPCXXWARNFLAGS), $(WARNFLAGS)) $(PREPROFLAGS) $(OPTIMFLAGS)
GCCPLUGINS_DIR:= $(shell $(CXX) -print-file-name=plugin)
INDENT= indent
ASTYLE= astyle
MD5SUM= md5sum
INDENTFLAGS= --gnu-style --no-tabs --honour-newlines
ASTYLEFLAGS= --style=gnu -s2
GTKPACKAGES= glib-2.0 gtk+-3.0
ONIONPACKAGES= glib-2.0 
MODULESPACKAGES= glib-2.0 
PKGCONFIG= pkg-config
OTEMPLATE= otemplate
PREPROFLAGS= -I. -I/usr/local/include -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED

GTKLIBES= -L/usr/local/lib -lbacktrace $(shell $(PKGCONFIG) --libs $(GTKPACKAGES)) -ldl -lm
ONIONLIBES= -L/usr/local/lib -lonion -lbacktrace $(shell $(PKGCONFIG) --libs $(ONIONPACKAGES)) -ldl -lm
RM= rm -fv

# the hand-written C source files common to both bismonion & bismongtk
# keep in alphabetical order
BM_COMMON_CSOURCES=				\
 agenda_BM.c					\
 allocgc_BM.c					\
 assoc_BM.c					\
 code_BM.c					\
 dump_BM.c					\
 emitcode_BM.c					\
 engine_BM.c					\
 gencode_BM.c					\
 guicode_BM.c					\
 id_BM.c					\
 list_BM.c					\
 load_BM.c					\
 main_BM.c					\
 node_BM.c					\
 object_BM.c					\
 parser_BM.c					\
 primes_BM.c					\
 scalar_BM.c					\
 sequence_BM.c

# the hand-written C++ source files common to both bismonion & bismongtk
BM_COMMON_CXXSOURCES= misc_BM.cc

# the hand-written C source files specific to GTK GUI bismongtk
BM_GTK_CSOURCES=		\
 gui_GTKBM.c			\
 newgui_GTKBM.c

# the hand-written C source files specific to Onion-based Web server bismonion
BM_ONION_CSOURCES= web_ONIONBM.c

# hand-written C/C++ headers (common to both bismonion & bismongtk)
BM_COMMON_HEADERS= bismon.h			\
 cmacros_BM.h					\
 fundecl_BM.h					\
 globals_BM.h					\
 inline_BM.h					\
 types_BM.h

# generated headers
BM_GENERATED_HEADERS=				\
 _bm_delim.h					\
 _bm_global.h					\
 _bm_predef.h					\
 _bm_types.h

# web templates for Onion's otemplates.
# ∀ foo, a C file _foo_ONIONBM.h and a C header _foo_ONIONBM.h is generated from foo_ONIONBM.thtml
ONIONBM_WEBTEMPLATES= login_ONIONBM.thtml
MARKDOWN_SOURCES= $(sort $(wildcard *.md))
MODULES_SOURCES= $(sort $(wildcard modules/modbm*.c))

# the object files for the GTK GUI bismongtk
BISMONGTKOBJECTS=								\
 $(patsubst %.c,%.gtk.o, $(sort $(BM_COMMON_CSOURCES) $(BM_GTK_CSOURCES)))	\
 $(patsubst %.cc,%.gtk.o, $(sort $(BM_COMMON_CXXSOURCES)))

# the object files for the Onion web-server bismonion
BISMONIONOBJECTS= 	\
 $(patsubst %.c,%.onion.o, $(sort $(BM_COMMON_CSOURCES) $(BM_ONION_CSOURCES)))	\
 $(patsubst %.cc,%.onion.o, $(sort $(BM_COMMON_CXXSOURCES)))

.PHONY: all programs clean indent count modules measure measured-bismon doc redump outdump checksum indentsinglemodule singlemodule indenttempmodule tempmodule


programs: bismongtk bismonion

bismongtk: $(BISMONGTKOBJECTS) _bm_allconsts-GTK.o | modules __timestamp.c 
	echo bismongtk: $(BISMONGTKOBJECTS) 
	@if [ -f $@ ]; then echo -n backup old executable: ' ' ; mv -v $@ $@~ ; fi
	$(MAKE) __timestamp.c __timestamp.o _bm_allconsts-GTK.o
	$(LINK.cc) -pthread  $(LINKFLAGS) -rdynamic $(OPTIMFLAGS) $(BISMONGTKOBJECTS) __timestamp.o _bm_allconsts-GTK.o $(GTKLIBES) -o $@
	ls -l $@
	$(RM) __timestamp.*

bismonion: $(BISMONIONOBJECTS) _bm_allconsts-ONION.o | modules __timestamp.c 
	echo bismonion: $(BISMONIONOBJECTS)
	@if [ -f $@ ]; then echo -n backup old executable: ' ' ; mv -v $@ $@~ ; fi
	$(MAKE) __timestamp.c __timestamp.o _bm_allconsts-ONION.o
	$(LINK.cc) -pthread  $(LINKFLAGS) -rdynamic $(OPTIMFLAGS) $(BISMONIONOBJECTS) __timestamp.o _bm_allconsts-ONION.o $(ONIONLIBES) -o $@
	ls -l $@
	$(RM) __timestamp.*

all: programs modules doc


## we could use git rev-parse HEAD for the lastgitcommit, but it does
## not give any log comment... Notice that tr command is interpreting
## some backslash escapes itself

__timestamp.c: Makefile
	@date +'const char bismon_timestamp[]="%c";%n const unsigned long bismon_timelong=%sL;' > __timestamp.tmp
	@(echo -n 'const char bismon_lastgitcommit[]="' ; \
	   git log --format=oneline --abbrev=12 --abbrev-commit -q  \
	     | head -1 | tr -d '\n\r\f\"\\\\' ; \
	   echo '";') >> __timestamp.tmp
	@(echo -n 'const char bismon_lastgittag[]="'; (git describe --abbrev=0 --all || echo '*notag*') | tr -d '\n\r\f\"\\\\'; echo '";') >> __timestamp.tmp
	@(echo -n 'const char bismon_checksum[]="'; cat bismon.h $(BM_HEADERS) $(CSOURCES) | $(MD5SUM) | cut -d' ' -f1 | tr -d '\n\r\f\"\\' ; echo '";') >> __timestamp.tmp
	@(echo -n 'const char bismon_directory[]="'; /bin/pwd | tr -d '\n\\"' ; echo '";') >> __timestamp.tmp
	@(echo -n 'const char bismon_makefile[]="'; echo -n  $(realpath $(lastword $(MAKEFILE_LIST))); echo '";') >> __timestamp.tmp
	@mv __timestamp.tmp __timestamp.c




checksum:
	@cat bismon.h $(BM_HEADERS) $(CSOURCES) | $(MD5SUM) | cut -d' ' -f1

%_BM-gtk.i: %_BM.c  %_BM.const.h bismon.h  $(GENERATED_HEADERS) $(BM_HEADERS)
	$(CC) $(CFLAGS) -DBISMONGTK $(shell $(PKGCONFIG) --cflags $(GTKPACKAGES)) -C -E $< | sed s:^#://#: | $(INDENT) -gnu > $@
%_GTKBM-gtk.i: %_GTKBM.c  %_BM.const.h bismon.h  $(GENERATED_HEADERS) $(BM_HEADERS)
	$(CC) $(CFLAGS)  -DBISMONGTK $(shell $(PKGCONFIG) --cflags $(GTKPACKAGES)) -C -E $< | sed s:^#://#: | $(INDENT) -gnu > $@
%_BM-onion.i: %_BM.c  %_BM.const.h bismon.h  $(GENERATED_HEADERS) $(BM_HEADERS)
	$(CC) $(CFLAGS) -DBISMONONION $(shell $(PKGCONFIG) --cflags $(ONIONPACKAGES)) -C -E $< | sed s:^#://#: | $(INDENT) -gnu > $@
%_BM-gtk.ii: %_BM.cc  $(GENERATED_HEADERS) $(BM_HEADERS)
	$(CCXX) $(CXXFLAGS)  -DBISMONGTK  $(shell $(PKGCONFIG) --cflags $(GTKPACKAGES)) -C -E $< | sed s:^#://#: > $@
%_BM-onion.ii: %_BM.cc  $(GENERATED_HEADERS) $(BM_HEADERS)
	$(CCXX) $(CXXFLAGS)  -DBISMONION  $(shell $(PKGCONFIG) --cflags $(GTKPACKAGES)) -C -E $< | sed s:^#://#: > $@

## to be used from C code as 'make indentsinglemodule MODULEID=<id>'
## in emit_module°plain_module
## for example 'make indentsinglemodule MODULEID=_9oXtCgAbkqv_4y1xhhF5Nhz'
## bismon would have perhaps passed a PREVIOUSMODULESOURCE
## see function emit_module°plain_module _1gME6zn82Kf_8hzWibLFRfz in gencode_BM.c
indentsinglemodule:
	@if [ ! -f modules/modbm$(MODULEID).c ]; then \
	   echo missing modules/modbm$(MODULEID).c; exit 1 ; fi ; \
	ms=modules/modbm$(MODULEID).c ; \
	cp -a $$ms "$$ms%"; \
	$(INDENT) $(INDENTFLAGS) $$ms; \
	$(INDENT) $(INDENTFLAGS) $$ms; \
	if cmp -s $$ms "$$ms%" ; then echo unchanged module $$ms ; mv "$$ms%" $$ms ; \
	  else echo '*indented module ' $$ms ; fi
	@if [ -f "$(PREVIOUSMODULESOURCE)" ] && cmp -s "$(PREVIOUSMODULESOURCE)" modules/modbm$(MODULEID).c ; then \
	  mv "$(PREVIOUSMODULESOURCE)" modules/modbm$(MODULEID).c ; \
	  echo same as previous modules/modbm$(MODULEID).c ; \
	else echo '**previous'  "$(PREVIOUSMODULESOURCE)" different of  modules/modbm$(MODULEID).c ; \
	fi

## to be used from C code as 'make singlemodule MODULEID=<id>'
singlemodule:
	@if [ ! -f modules/modbm$(MODULEID).c ]; then \
	   echo missing modules/modbm$(MODULEID).c; exit 1 ; fi 
	$(MAKE) modubin/modbm$(MODULEID).so

#### for temporary modules
indenttempmodule:
	@if [ ! -f modules/tmpmobm$(MODULEID).c ]; then \
	   echo missing modules/tmpmobm$(MODULEID).c; exit 1 ; fi ; \
	ms=modules/tmpmobm$(MODULEID).c ; \
	cp -a $$ms "$$ms%"; \
	$(INDENT) $(INDENTFLAGS) $$ms; \
	$(INDENT) $(INDENTFLAGS) $$ms; \
	if cmp -s $$ms "$$ms%" ; then echo unchanged module $$ms ; mv "$$ms%" $$ms ; \
	  else echo '*indented module ' $$ms ; fi
	@if [ -f "$(PREVIOUSMODULESOURCE)" ] && cmp -s "$(PREVIOUSMODULESOURCE)" modules/tmpmobm$(MODULEID).c ; then \
	  mv "$(PREVIOUSMODULESOURCE)" modules/tmpmobm$(MODULEID).c ; \
	  echo same as previous modules/tmpmobm$(MODULEID).c ; \
	else echo '**previous'  "$(PREVIOUSMODULESOURCE)" different of  modules/tmpmobm$(MODULEID).c ; \
	fi

## to be used from C code as 'make tempmodule MODULEID=<id>'
tempmodule:
	@if [ ! -f modules/tmpmobm$(MODULEID).c ]; then \
	   echo missing modules/tmpmobm$(MODULEID).c; exit 1 ; fi 
	$(MAKE) modubin/tmpmobm$(MODULEID).so

# cancel implicit rule for C files to force my explicit rules
# https://stackoverflow.com/a/29227455/841108
%.o: %.c

%_BM.gtk.o: %_BM.c bismon.h $(GENERATED_HEADERS) $(BM_HEADERS) %_BM.const.h
	echo BM.gtk objcirc is $^ left $<
	$(COMPILE.c) $(shell $(PKGCONFIG) --cflags $(GTKPACKAGES)) -DBISMONGTK -c $< -o $@

%_GTKBM.gtk.o: %_GTKBM.c bismon.h $(GENERATED_HEADERS) $(BM_HEADERS) %_GTKBM.const.h
	echo GTKBM.gtk objcirc is $^ left $<
	$(COMPILE.c)  $(shell $(PKGCONFIG) --cflags $(GTKPACKAGES)) -DBISMONGTK -c $< -o $@

%_BM.gtk.o: %_BM.cc bismon.h $(GENERATED_HEADERS) $(BM_HEADERS)
	echo BM.gtkcc objcirc is $^ left $<
	$(COMPILE.cc)  $(shell $(PKGCONFIG) --cflags $(GTKPACKAGES))  -DBISMONGTK $< -o $@

%_BM.onion.o: %_BM.c bismon.h $(GENERATED_HEADERS) $(BM_HEADERS) %_BM.const.h
	echo BM.onion objcirc is $^ left $<
	$(COMPILE.c) $(shell $(PKGCONFIG) --cflags $(ONIONPACKAGES)) -DBISMONION -c $< -o $@

%_BM.onion.o: %_BM.cc bismon.h $(GENERATED_HEADERS) $(BM_HEADERS)
	echo BM.onioncc objcirc is $^ left $<
	$(COMPILE.cc)  $(shell $(PKGCONFIG) --cflags $(GTKPACKAGES))  -DBISMONION $< -o $@

%_ONIONBM.onion.o: %_ONIONBM.c bismon.h $(GENERATED_HEADERS) $(BM_HEADERS) %_ONIONBM.const.h
	echo ONIONBM.onion objcirc is $^ left $<
	$(COMPILE.c)  $(shell $(PKGCONFIG) --cflags $(ONIONPACKAGES)) -DBISMONION -c $< -o $@

%_BM.const.h: %_BM.c BM_makeconst
	./BM_makeconst -H $@ $<
%_GTKBM.const.h: %_GTKBM.c BM_makeconst
	./BM_makeconst -H $@ $<
%_ONIONBM.const.h: %_ONIONBM.c BM_makeconst
	./BM_makeconst -H $@ $<
_%_ONIONBM.c _%_ONIONBM.h: %_ONIONBM.thtml
	$(OTEMPLATE) -a $(patsubst  %_ONIONBM.thtml,_%_ONIONBM.h,$<) $< $@
__timestamp.o: __timestamp.c
	$(COMPILE.c)  -DBMtimestamp -c $< -o $@

_bm_allconsts-GTK.o: _bm_allconsts-GTK.c
	$(COMPILE.c)  -DBMallconsts -c $< -o $@


_bm_allconsts-ONION.o: _bm_allconsts-ONION.c
	$(COMPILE.c)  -DBMallconsts -c $< -o $@

_bm_allconsts-GTK.c: $(sort $(BM_COMMON_CSOURCES) $(BM_GTK_CSOURCES))  BM_makeconst
	./BM_makeconst -C $@  $(sort $(BM_COMMON_CSOURCES) $(BM_GTK_CSOURCES))
_bm_allconsts-ONION.c: $(sort $(BM_COMMON_CSOURCES) $(BM_ONION_CSOURCES)) BM_makeconst
	./BM_makeconst -C $@ $(sort $(BM_COMMON_CSOURCES) $(BM_ONION_CSOURCES))

modubin/modbm_%.so: modules/modbm_%.c bismon.h  $(GENERATED_HEADERS) $(BM_HEADERS)
	$(CCACHE) $(LINK.c) $(shell $(PKGCONFIG) --cflags $(MODULESPACKAGES)) -fPIC  -DBISMON_MODID=$(patsubst modules/modbm_%.c,_%,$<)  '-DBISMON_MOMD5="$(shell md5sum $< | cut '-d ' -f1)"' -DBISMON_PERSISTENT_MODULE -shared $< $(shell $(PKGCONFIG) --libs $(MODULESPACKAGES)) -o $@

modubin/tmpmobm_%.so: modules/tmpmobm_%.c bismon.h  $(GENERATED_HEADERS) $(BM_HEADERS)
	$(CCACHE) $(LINK.c) $(shell $(PKGCONFIG) --cflags $(MODULESPACKAGES)) -fPIC -DBISMON_MODID=$(patsubst modules/tmpmobm_%.c,_%,$<) '-DBISMON_MOMD5="$(shell md5sum $< | cut '-d ' -f1)"' -DBISMON_TEMPORARY_MODULE -shared $< $(shell $(PKGCONFIG) --libs $(MODULESPACKAGES)) -o $@

modules:
	$(MAKE) -k $(MAKEFLAGS)  $(patsubst modules/%.c,modubin/%.so,$(MODULES_SOURCES)) ; exit 0




doc: $(MARKDOWN_SOURCES) bismongtk modules
	for f in $(MARKDOWN_SOURCES) ; do  $(MARKDOWN) $$f > $$(basename $$f .md).html ; done
	./build-bismon-doc.sh

count:
	@wc -cl $(wildcard *.c *.h *.cc modules/_*.c) | sort -n

redump: bismongtk bismonion modules
	@for f in $(GENERATED_HEADERS) $(GENERATED_CSOURCES) $(MODULES_SOURCES) *.bmon ; \
           do cp -vab $$f $$f%~ ; done
	time ./bismongtk --dump-after-load . --batch
	printf "\n\n\n **** second redump run ***\n"
	time ./bismonion --dump-after-load . --batch
	$(MAKE) indent

outdump: bismongtk bismonion  modules
	time ./bismongtk  --run-command 'rm -rvf /tmp/bd'  --dump-after-load /tmp/bd --batch
	for f in /tmp/bd/* ; do cmp $$f $$(basename $$f); done

################################################################


clean:
	$(RM) .*~ *~ *% *.o *.so */*.so *.log */*~ */*.orig *.i *.orig *.gch README.html
	$(RM) core* *.i *.ii *prof.out gmon.out
	$(RM) *_BM.const.h _bm_allconsts*.c
	$(RM) modubin/*.so modules/*.i modules/*% modules/*~ modules/*- bismon BM_makeconst
	$(RM) modules/tmpmobm*
	$(RM) $(patsubst %.md,%.html, $(MARKDOWN_SOURCES))
	$(RM)  doc/generated/* doc/htmldoc/*  doc/*/*~
	$(RM) doc/*.aux doc/*.log doc/*.bbl doc/*.blg doc/*.idx doc/*.ilg doc/*.ind doc/*.log doc/*.out doc/*.toc doc/*.haux doc/*.hind doc/*.html doc/*.htoc
	$(RM) doc/bismon-html-doc.tar.gz

indent: .indent.pro
	@printf "\n *** headers *** \n"
	@for h in $(BM_HEADERS); do \
	  cp -a $$h $$h% ; \
	  $(INDENT) $(INDENTFLAGS) $$h ; \
	  $(INDENT) $(INDENTFLAGS) $$h ; \
	  if cmp -s $$h $$h% ; then echo unchanged $$h ; mv $$h% $$h ; \
          else echo '*indented' $$h ; fi \
	done
	@printf "\n *** C sources *** \n"
	@for c in $(BM_COLDSOURCES); do \
	  cp -a $$c $$c% ; \
	  $(INDENT) $(INDENTFLAGS) $$c ; \
	  $(INDENT) $(INDENTFLAGS) $$c ; \
	  if cmp -s $$c $$c% ; then echo unchanged $$c ; mv $$c% $$c ; \
	  else echo '*indented' $$c ; fi ; \
	done
	@for c in $(GTKBM_COLDSOURCES); do \
	  cp -a $$c $$c% ; \
	  $(INDENT) $(INDENTFLAGS) $$c ; \
	  $(INDENT) $(INDENTFLAGS) $$c ; \
	  if cmp -s $$c $$c% ; then echo unchanged $$c ; mv $$c% $$c ; \
	  else echo '*indented' $$c ; fi ; \
	done
	@for c in $(ONIONBM_COLDSOURCES); do \
	  cp -a $$c $$c% ; \
	  $(INDENT) $(INDENTFLAGS) $$c ; \
	  $(INDENT) $(INDENTFLAGS) $$c ; \
	  if cmp -s $$c $$c% ; then echo unchanged $$c ; mv $$c% $$c ; \
	  else echo '*indented' $$c ; fi ; \
	done
	@printf "\n *** C modules sources *** \n"
	@for c in $(MODULES_SOURCES); do \
	  cp -a $$c $$c% ; \
	  $(INDENT) $(INDENTFLAGS) $$c ; \
	  $(INDENT) $(INDENTFLAGS) $$c ; \
	  if cmp -s $$c $$c% ; then echo unchanged $$c ; mv $$c% $$c ; \
	  else echo '*indented' $$c ; fi ; \
	done
	@printf "\n *** C++ sources *** \n"
	@for x in $(BM_CXXSOURCES); do \
	  $(ASTYLE) $(ASTYLEFLAGS) $$x ; \
	done
	@printf "\n *** C++ tool source *** \n"
	@$(ASTYLE) $(ASTYLEFLAGS)  BM_makeconst.cc
	@printf "\n"
