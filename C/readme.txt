compile with
	make all

run with
	./graph.o


A lot of the code is not very well optimized, but since this is past the
due date I just wanted to make your life easier, no one should have to
slog through poorly documented code. So nothing has been changed besides
formatting and comments, even obvious inefficiencies in the code.

TODO Implement better partitioning. Currently graphs are partitioned only by
vertex degree, but there are other metrics that can be used. For example, each
vertex is assigned two numbers, first its degree and then the sum of the degrees
of all vertices connected to it. This is however resisted by highly symmetric
(aka regular) graphs.
