
ALL=modeloRedSocial recorridoRedSocial recorridoRedSocial_seq
FLAGS=-DVARIANTE_1
LIBS=-lm

todo: $(ALL)

modeloRedSocial: digraph.o modeloRedSocial.c
	gcc $(FLAGS) $^ $(LIBS) -o $@

recorridoRedSocial: digraph.o recorridoRedSocial.c
	gcc $(FLAGS) -fopenmp $^ $(LIBS) -o $@

recorridoRedSocial_seq: digraph.o recorridoRedSocial.c
	gcc $(FLAGS) $^ $(LIBS) -lgomp -o $@

digraph.o: digraph.c digraph.h
	gcc -c $(FLAGS) $(LIBS) $< -o $@

clean:
	rm -f $(ALL) *.o
