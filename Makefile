ifeq (,$(wildcard dots))
.DEFAULT_GOAL := dots
else
.DEFAULT_GOAL := pngs
endif

split-graph: split-graph.cpp Makefile
	g++ -O2 -o split-graph split-graph.cpp

rfc-index.txt:
	wget https://www.rfc-editor.org/in-notes/rfc-index.txt
	-rm -f dots

dots: rfc-index.txt split-graph
	./index2dot.awk < rfc-index.txt | sort -r | ./split-graph `tail rfc-index.txt | grep -E '^[[:digit:]]+' | tail -n 1 | cut -d' ' -f 1`
	touch dots
	echo -e "\n\n\nGraph built successfully.\nNow run \"make\" again, with maximum parallelism.\n(On Linux, try \"make -j \`getconf _NPROCESSORS_ONLN\`\")"

pngs: $(patsubst %.dot,%.png,$(wildcard *.dot))
	touch pngs

%.png: %.dot
	dot -Tpng:gd:gd $< -o $@

.PHONY: clean

clean:
	-rm -f rfc-index.txt *.dot *.png index.html dots pngs split-graph
