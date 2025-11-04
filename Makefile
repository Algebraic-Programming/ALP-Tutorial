# Build main.pdf from main.tex (use existing main.tex which loads utf8 & listings)

MAIN      := main
TEXSRC    := $(MAIN).tex
PDF       := $(MAIN).pdf

LATEXMK   := $(shell command -v latexmk 2>/dev/null)
TEXENGINE ?= pdflatex

ifeq ($(LATEXMK),)
	PDF_CMD := $(TEXENGINE) -interaction=nonstopmode -halt-on-error $(TEXSRC) && \
			   $(TEXENGINE) -interaction=nonstopmode -halt-on-error $(TEXSRC)
	WATCH_CMD := @echo "latexmk not found; 'make watch' unavailable."
else
	PDF_CMD := latexmk -pdf -interaction=nonstopmode -halt-on-error -file-line-error $(TEXSRC)
	WATCH_CMD := latexmk -pdf -pvc -interaction=nonstopmode -halt-on-error -file-line-error $(TEXSRC)
endif

.PHONY: all pdf watch clean distclean open beamer

all: pdf

pdf: $(PDF)

$(PDF): $(TEXSRC)
	@echo "==> Building $(PDF)"
	@$(PDF_CMD)

# --- new: beamer target ----------------------------------------------------
BEAMER_MAIN := beamer-tutorial
BEAMER_SRC  := $(BEAMER_MAIN).tex
BEAMER_PDF  := $(BEAMER_MAIN).pdf

beamer: $(BEAMER_PDF)

$(BEAMER_PDF): $(BEAMER_SRC)
	@echo "==> Building $(BEAMER_PDF)"
	@if command -v latexmk >/dev/null 2>&1; then \
		latexmk -pdf -interaction=nonstopmode -halt-on-error -file-line-error $(BEAMER_SRC); \
	else \
		$(TEXENGINE) -interaction=nonstopmode -halt-on-error $(BEAMER_SRC) && \
		$(TEXENGINE) -interaction=nonstopmode -halt-on-error $(BEAMER_SRC); \
	fi

watch:
	$(WATCH_CMD)

open: $(PDF)
	xdg-open $(PDF) >/dev/null 2>&1 || true

clean:
	@echo "==> Cleaning aux files"
	@-rm -f *.aux *.log *.out *.toc *.lof *.lot *.fls *.fdb_latexmk *.synctex.gz *.bbl *.blg *.nav *.snm *.vrb *.run.xml

distclean: clean
	@echo "==> Removing PDF"
	@-rm -f $(PDF)