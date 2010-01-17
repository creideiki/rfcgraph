all: pngs rmdots legend.png

split-graph: split-graph.cpp Makefile
	g++ -O2 -o split-graph split-graph.cpp

rfc-index.txt:
	wget http://www.ietf.org/download/rfc-index.txt
	-rm -f dots

.PHONY: all pngs clean rmdots

dots: rfc-index.txt split-graph
	./index2dot.awk < rfc-index.txt | sort -r | ./split-graph `tail rfc-index.txt | grep -E '^[[:digit:]]+' | tail -n 1 | cut -d' ' -f 1`
	touch dots

pngs: dots
	for i in *.dot; do dot -Tpng "$$i" -o `basename "$$i" .dot`.png; done

legend.png:
	dot -Tpng legend -o legend.png

clean:
	-rm -f rfc-index.txt *.dot *.png index.html dots split-graph

rmdots:
	-rm -f *.dot
