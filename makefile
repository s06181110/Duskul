#CFLAGS = -std=c99 -Wall -DDEBUG -O0 -g
CFLAGS = -std=c99 -Wall -O2
##OBJ##
OBJ = abort.o errmessages.o evaluate.o execute.o \
    expression.o exptree.o functions.o getitem.o getsymbol.o \
    identifiers.o idtable.o main.o newnode.o nextch.o \
    printitem.o simplestat.o statements.o strliteral.o symset.o \

dusk: $(OBJ)
	$(CC) -o $@ $(OBJ)

clean:
	rm -rf *.o dusk graph.pdf *.dSYM

graph: makedot.sh
	./makedot.sh | dot -Tpdf -o graph.pdf
	open graph.pdf

make: make_template
	@ls *.c | sed 's/\.c/.o/' | awk -f mkobjs.awk > _tmp
	@sed '/^##OBJ##/r _tmp' make_template > makefile
	@cc -MM *.c >> makefile
	@rm _tmp

##DEPENDENCY##
abort.o: abort.c getitem.h nextch.h commontype.h token.h abort.h
errmessages.o: errmessages.c
evaluate.o: evaluate.c evaluate.h stnode.h expnode.h expression.h \
  exp_imp.h getitem.h nextch.h commontype.h token.h stnode_imp.h \
  statements.h symset.h strliteral.h abort.h
execute.o: execute.c evaluate.h stnode.h expnode.h exp_imp.h getitem.h \
  nextch.h commontype.h token.h stnode_imp.h statements.h symset.h \
  strliteral.h abort.h
expression.o: expression.c statements.h commontype.h stnode.h token.h \
  getitem.h nextch.h symset.h expression.h expnode.h exp_imp.h \
  stnode_imp.h abort.h
exptree.o: exptree.c token.h commontype.h expression.h expnode.h \
  exp_imp.h getitem.h nextch.h stnode_imp.h stnode.h statements.h \
  symset.h abort.h
functions.o: functions.c getitem.h nextch.h commontype.h token.h \
  identifiers.h statements.h stnode.h symset.h stnode_imp.h expnode.h \
  abort.h
getitem.o: getitem.c getsymbol.h getitem.h nextch.h commontype.h token.h \
  identifiers.h abort.h
getsymbol.o: getsymbol.c getsymbol.h getitem.h nextch.h commontype.h \
  token.h strliteral.h abort.h
identifiers.o: identifiers.c identifiers.h getitem.h nextch.h \
  commontype.h token.h idtable.h abort.h
idtable.o: idtable.c identifiers.h getitem.h nextch.h commontype.h \
  token.h idtable.h abort.h
main.o: main.c getitem.h nextch.h commontype.h token.h symset.h \
  identifiers.h statements.h stnode.h evaluate.h expnode.h abort.h
newnode.o: newnode.c commontype.h stnode_imp.h stnode.h expnode.h abort.h
nextch.o: nextch.c nextch.h commontype.h abort.h
printitem.o: printitem.c abort.h identifiers.h getitem.h nextch.h \
  commontype.h token.h strliteral.h
simplestat.o: simplestat.c getitem.h nextch.h commontype.h token.h \
  identifiers.h statements.h stnode.h symset.h stnode_imp.h expnode.h \
  expression.h abort.h
statements.o: statements.c getitem.h nextch.h commontype.h token.h \
  identifiers.h statements.h stnode.h symset.h stnode_imp.h expnode.h \
  expression.h abort.h
strliteral.o: strliteral.c
symset.o: symset.c symset.h token.h commontype.h abort.h
