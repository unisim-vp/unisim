CC=gcc
PDFLATEX=pdflatex
BIBTEX=bibtex

DVIVIEWER=kdvi
PDFVIEWER=kghostview

# TLM
TLM_LATEX_SOURCES=tlm/tlm.tex \
tlm/appendix.tex
TLM_CPP_SOURCES= \
tlm/interface1.cpp \
tlm/interface2.cpp \
tlm/interface3.cpp \
tlm/new_operator.cpp \
tlm/memory_req_rsp.cpp \
tlm/utlm_processor.cpp \
tlm/utlm_memory.cpp \
tlm/ttlm_processor.cpp \
tlm/ttlm_memory.cpp \
tlm/ttlm_router.cpp \
tlm/ttlm_router_adv.cpp \
tlm/ttlm_router_adv2.cpp 

TLM_FIGURES= \
tlm/utlm_principle.pdf \
tlm/ttlm_principle.pdf \
tlm/chaining_communication_steps.pdf \
tlm/n_to_m.pdf

TLM_GENERATED_LATEX=$(patsubst %.cpp,%.tex,$(TLM_CPP_SOURCES))

# services
SERVICES_LATEX_SOURCES=services/services.tex
SERVICES_CPP_SOURCES= \
services/instanciation.cpp \
services/connection.cpp \
services/simple_service.cpp
SERVICES_FIGURES=

SERVICES_GENERATED_LATEX=$(patsubst %.cpp,%.tex,$(SERVICES_CPP_SOURCES))

# installation
INSTALL_LATEX_SOURCES=installation/install.tex
INSTALL_CPP_SOURCES=
INSTALL_FIGURES=

INSTALL_GENERATED_LATEX=$(patsubst %.cpp,%.tex,$(INSTALL_CPP_SOURCES))

# TLM manual
TLM_MANUAL_LATEX_SOURCES=tlm_manual.tex $(TLM_LATEX_SOURCES)
TLM_MANUAL_FIGURES=$(TLM_FIGURES)
TLM_MANUAL_TARGET_PDF=tlm_manual.pdf

# services manual
SERVICES_MANUAL_LATEX_SOURCES=services_manual.tex $(SERVICES_LATEX_SOURCES)
SERVICES_MANUAL_FIGURES=$(SERVICES_FIGURES)
SERVICES_MANUAL_TARGET_PDF=services_manual.pdf

# installation manual
INSTALL_MANUAL_LATEX_SOURCES=install_manual.tex $(INSTALL_LATEX_SOURCES)
INSTALL_MANUAL_FIGURES=$(INSTALL_FIGURES)
INSTALL_MANUAL_TARGET_PDF=install_manual.pdf


# Whole manual
MANUAL_LATEX_SOURCES=manual.tex $(TLM_LATEX_SOURCES) $(SERVICES_LATEX_SOURCES) $(INSTALL_LATEX_SOURCES)
MANUAL_GENERATED_LATEX=$(TLM_GENERATED_LATEX) $(SERVICES_GENERATED_LATEX) $(INSTALL_GENERATED_LATEX)
MANUAL_FIGURES=$(TLM_FIGURES) $(SERVICES_FIGURES) $(INSTALL_FIGURES)
MANUAL_TARGET_PDF=manual.pdf

all: $(MANUAL_TARGET_PDF) $(TLM_MANUAL_TARGET_PDF) $(SERVICES_MANUAL_TARGET_PDF) $(INSTALL_MANUAL_TARGET_PDF)

clean:
	rm -f $(MANUAL_TARGET_PDF) $(TLM_MANUAL_TARGET_PDF) $(SERVICES_MANUAL_TARGET_PDF) $(MANUAL_GENERATED_LATEX) *.aux *.log *.bbl *.blg *.toc *.lof ./c_to_tex
	
c_to_tex: c_to_tex.c
	$(CC) c_to_tex.c -o c_to_tex

$(TLM_MANUAL_TARGET_PDF): $(TLM_MANUAL_LATEX_SOURCES) $(TLM_GENERATED_LATEX) $(TLM_MANUAL_FIGURES)
$(SERVICES_MANUAL_TARGET_PDF): $(SERVICES_MANUAL_LATEX_SOURCES) $(SERVICES_GENERATED_LATEX) $(SERVICES_MANUAL_FIGURES)
$(INSTALL_MANUAL_TARGET_PDF): $(INSTALL_MANUAL_LATEX_SOURCES) $(INSTALL_GENERATED_LATEX) $(INSTALL_MANUAL_FIGURES)
$(MANUAL_TARGET_PDF): $(MANUAL_LATEX_SOURCES) $(MANUAL_GENERATED_LATEX) $(MANUAL_FIGURES)

%.tex: %.cpp ./c_to_tex
	./c_to_tex $< $@ 10000 95

%.pdf: %.tex
	$(PDFLATEX) $<; $(BIBTEX) $<; $(PDFLATEX) $<

