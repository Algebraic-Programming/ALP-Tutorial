# Build main.pdf from main.tex (use existing main.tex which loads utf8 & listings)
# Dependencies:
# - TeX Live (pdflatex) and common LaTeX packages: amsmath, hyperref, listings, xcolor
# - Beamer class for slides
# - latexmk (recommended)
#
# Debian/Ubuntu (recommended minimal set):
#   sudo apt-get update && sudo apt-get install -y \
#     latexmk texlive-latex-base texlive-latex-recommended latex-beamer \
#     texlive-latex-extra texlive-fonts-recommended
#
# Fedora/RHEL (approximate equivalents):
#   sudo dnf install -y latexmk texlive-scheme-medium texlive-beamer texlive-collection-latexrecommended

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

SHELL := /bin/bash

# Clean aux for all TeX sources in this directory
TEX_SOURCES    := $(wildcard *.tex)
TEX_BASENAMES  := $(basename $(TEX_SOURCES))
AUX_EXTS       := aux log out toc lof lot fls fdb_latexmk synctex.gz \
				  bbl blg bcf run.xml xdv dvi ps nav snm vrb brf idx ilg ind ist \
				  acn acr alg glg glo gls glsdefs nlo nls lox thm auxlock \
				  4ct 4tc lg tmp maf mtc mtc0 maf0

.PHONY: all pdf watch clean distclean veryclean open beamer beamer-clean latexmk-clean help deps check-deps

all: pdf

pdf: $(PDF)

$(PDF): $(TEXSRC)
	@echo "==> Building $(PDF)"
	@$(PDF_CMD)

# --- info / helpers ---------------------------------------------------------
help:
	@echo "Targets: make | make pdf | make beamer | make clean | make distclean"
	@echo "Helpers: make deps (install hints), make check-deps (quick sanity check)"

deps:
	@echo "Debian/Ubuntu:"
	@echo "  sudo apt-get update && sudo apt-get install -y \\"
	@echo "    latexmk texlive-latex-base texlive-latex-recommended latex-beamer \\"
	@echo "    texlive-latex-extra texlive-fonts-recommended"
	@echo
	@echo "Fedora/RHEL:"
	@echo "  sudo dnf install -y latexmk texlive-scheme-medium texlive-beamer texlive-collection-latexrecommended"

check-deps:
	@ok=1; \
	if ! command -v pdflatex >/dev/null 2>&1; then echo "Missing: pdflatex (install TeX Live)"; ok=0; fi; \
	if ! command -v latexmk  >/dev/null 2>&1; then echo "Note: latexmk not found (optional but recommended)"; fi; \
	if ! kpsewhich beamer.cls >/dev/null 2>&1; then echo "Missing: Beamer class (install latex-beamer or texlive-latex-recommended)"; ok=0; fi; \
	if [ $$ok -eq 1 ]; then echo "Dependency check: OK"; else echo "See 'make deps' for install hints."; fi

# --- beamer target ----------------------------------------------------------
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
	@echo "==> Cleaning TeX auxiliary files for: $(TEX_BASENAMES)"
	@for base in $(TEX_BASENAMES); do \
		for ext in $(AUX_EXTS); do rm -f "$${base}.$${ext}"; done; \
	done
	@-rm -rf _minted-* .latex-cache latex.out

latexmk-clean:
	@echo "==> latexmk -C (all .tex, if available)"
	@if command -v latexmk >/dev/null 2>&1; then \
		for tex in $(TEX_SOURCES); do latexmk -C "$$tex"; done; \
	else \
		echo "latexmk not found; skipping latexmk -C"; \
	fi

distclean: clean
	@echo "==> Removing generated PDFs"
	@-rm -f $(PDF) $(BEAMER_PDF)

veryclean: distclean
	@echo "==> Removing editor backup/temporary files"
	@-rm -f *~ \#*# .*~ .\#* *.bak *.bck *.tmp *.swp *.swo *.swx

beamer-clean:
	@echo "==> Cleaning beamer aux files"
	@-for base in $(TEX_BASENAMES); do \
		if [[ "$$base" == beamer-* ]]; then \
			for ext in aux log nav out snm toc vrb fls fdb_latexmk synctex.gz; do \
				rm -f "$${base}.$${ext}"; \
			done; \
		fi; \
	done
