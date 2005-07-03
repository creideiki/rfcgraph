all: pngs rmdots legend.png

split-graph: split-graph.cpp Makefile
	g++ -O2 -o split-graph split-graph.cpp

1rfc_index.txt:
	wget http://www.ietf.org/iesg/1rfc_index.txt
	-rm -f dots

.PHONY: all pngs clean rmdots

dots: 1rfc_index.txt split-graph
	./index2dot.awk < 1rfc_index.txt | sort -r | ./split-graph `tail 1rfc_index.txt | grep -E '^[[:digit:]]+' | tail -n 1 | cut -d' ' -f 1`
	touch dots

pngs: dots
	for i in *.dot; do dot -Tpng "$$i" -o `basename "$$i" .dot`.png; done

legend.png:
	dot -Tpng legend -o legend.png

clean:
	-rm -f 1rfc_index.txt *.dot *.png index.html dots split-graph

rmdots:
	-rm -f *.dot
