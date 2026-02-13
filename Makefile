EXPERIMENTS := linux-fs

all: out/results.db

build:
	cmake -S tool -B build

TOOL_SRCS := tool/main.cpp tool/sqlite3.c tool/sqlite3.h tool/CMakeLists.txt
build/analyze-error-handling: $(TOOL_SRCS) | build
	cmake --build build

out/%.db: experiments/%/experiment.toml \
          experiments/%/notable.json \
          experiments/%/benign.json \
          build/analyze-error-handlling
	python analyze.py experiments/$*

SCHEMA := schema.sql
DBS := $(EXPERIMENTS:%=out/%.db)
out/results.db: merge.py $(SCHEMA) $(DBS)
	python merge.py

clean:
	rm -rf build out
