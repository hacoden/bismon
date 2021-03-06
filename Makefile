## the Makefile
NINJA= ninja
GCC= gcc
GXX= g++
CC= $(GCC)
CXX= $(GXX)
#CCACHE= ccache
MARKDOWN= markdown
INDENT= indent
ASTYLE= astyle
MD5SUM= md5sum
INDENTFLAGS= --gnu-style --no-tabs --honour-newlines
ASTYLEFLAGS= --style=gnu -s2
RM= rm -fv
-include _cflagsmodule.mk
MARKDOWN_SOURCES= $(sort $(wildcard *.md))
MODULES_SOURCES= $(sort $(wildcard modules/modbm*.c))
BM_HEADERS= $(wildcard [a-z]*BM.h bismon.h)
BM_CSOURCES= $(wildcard [a-z]*BM.c)


.PHONY: all programs clean verbose indent count modules measure doc redump outdump checksum indentsinglemodule indenttempmodule 




all: programs modules #doc

programs: bismon modules

verbose: | build.ninja
	$(NINJA) -v

bismon: build.ninja $(wildcard *BM.c *_BM.cc)  $(wildcard store*bmon)
	$(NINJA) $@

_cflagsmodule.mk: build.ninja 
	$(NINJA) $@

build.ninja: generate-ninja-builder.sh
	./$^ > $@.tmp; mv --backup -v $@.tmp $@

#checksum:
#	@cat bismon.h $(BM_HEADERS) $(CSOURCES) | $(MD5SUM) | cut -d' ' -f1
 -C -E $< | sed s:^#://#: > $@



modubin/modbm_%.so: modules/modbm_%.c $(BISMONHEADERS) | _cflagsmodule.mk
	$(LINK.c) -fPIC $(BISMONMODULECFLAGS) \
	      -DBISMON_MODID=$(patsubst modules/modbm_%.c,_%,$<)  \
              -DBISMON_MOMD5='"$(shell md5sum $< | cut '-d ' -f1)"' \
              -DBISMON_PERSISTENT_MODULE -shared $< -o $@


modubin/tmpmobm_%.so: modules/tmpmobm_%.c $(BISMONHEADERS) | _cflagsmodule.mk
	$(LINK.c) -fPIC   $(BISMONMODULECFLAGS) \
	     -DBISMON_MODID=$(patsubst modules/tmpmobm_%.c,_%,$<) \
	     -DBISMON_MOMD5='"$(shell md5sum $< | cut '-d ' -f1)"' -DBISMON_TEMPORARY_MODULE \
	     -shared $< -o $@

modules: _cflagsmodule.mk
	$(MAKE) -k $(MAKEFLAGS)  $(patsubst modules/%.c,modubin/%.so,$(MODULES_SOURCES)) ; exit 0




doc: $(MARKDOWN_SOURCES) bismon bismongtk modules $(wildcard doc/*.tex doc/*.bib doc/*.hva)
	for f in $(MARKDOWN_SOURCES) ; do  $(MARKDOWN) $$f > $$(basename $$f .md).html ; done
	./build-bismon-doc.sh

count:
	@wc -cl $(wildcard *.c *.h *.cc modules/_*.c) | sort -n

redump: bismon bismongtk bismonion modules
	@for f in $(GENERATED_HEADERS) $(GENERATED_CSOURCES) $(MODULES_SOURCES) *.bmon ; \
           do cp -vab $$f $$f%~ ; done
	time ./bismongtk --dump-after-load . --batch
	$(MAKE)  bismonion
	printf "\n\n\n **** second redump run ***\n"
	time ./bismonion --dump-after-load . --batch

outdump: bismon bismongtk bismonion  modules
	time ./bismongtk  --run-command 'rm -rvf /tmp/bd'  --dump-after-load /tmp/bd --batch
	for f in /tmp/bd/* ; do cmp $$f $$(basename $$f); done

################################################################


clean:
	[ -f build.ninja ] && $(NINJA) -t clean
	$(RM) .*~ *~ *% *.o *.so */*.so *.log */*~ */*.orig *.i *.orig *.gch README.html
	$(RM) .ninja* *mkd  _cflagsmodule.mk
	$(RM) build.ninja
	$(RM) core* *.i *.ii *prof.out gmon.out
	$(RM) *BM.const.h _bm_allconsts*.c
	$(RM) $(patsubst %.thtml, _%.c, $(ONIONBM_WEBTEMPLATES))
	$(RM) $(patsubst %.thtml, _%.h, $(ONIONBM_WEBTEMPLATES))
	$(RM) modubin/*.so modules/*.i modules/*% modules/*~ modules/*- bismon BM_makeconst
	$(RM) modules/badmodbm*
	$(RM) __timestamp.*
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
	@for c in $(BM_CSOURCES); do \
	  cp -a $$c $$c% ; \
	  $(INDENT) $(INDENTFLAGS) $$c ; \
	  $(INDENT) $(INDENTFLAGS) $$c ; \
	  if cmp -s $$c $$c% ; then echo unchanged $$c ; mv $$c% $$c ; \
	  else echo '*indented' $$c ; fi ; \
	done
#!-      @printf "\n *** C modules sources *** \n"
#!-      @for c in $(MODULES_SOURCES); do \
#!-        cp -a $$c $$c% ; \
#!-        $(INDENT) $(INDENTFLAGS) $$c ; \
#!-        $(INDENT) $(INDENTFLAGS) $$c ; \
#!-        if cmp -s $$c $$c% ; then echo unchanged $$c ; mv $$c% $$c ; \
#!-        else echo '*indented' $$c ; fi ; \
#!-      done
	@printf "\n *** C++ sources *** \n"
	@for x in $(BM_CXXSOURCES); do \
	  $(ASTYLE) $(ASTYLEFLAGS) $$x ; \
	done
	@printf "\n *** C++ tool source *** \n"
	@$(ASTYLE) $(ASTYLEFLAGS)  BM_makeconst.cc
	@printf "\n"
