split-graph: split-graph.cpp Makefile
	g++ -O2 -o split-graph split-graph.cpp

1rfc_index.txt:
	wget http://www.ietf.org/iesg/1rfc_index.txt
	-rm -f dots

.PHONY: pngs clean

dots: 1rfc_index.txt
	./index2dot.awk < 1rfc_index.txt | sort -r | ./split-graph `tail 1rfc_index.txt | grep -E '^[[:digit:]]+' | tail -n 1 | cut -d' ' -f 1`
	touch dots

pngs: dots
	for i in *.dot; do dot -Tpng "$$i" > `basename "$$i" .dot`.png; done

clean:
	-rm -f 1rfc_index.txt *.dot *.png index.html
