{ This program uses DIV on an expression that is real because one of its
  operands is real }
PROGRAM test;

var x : integer;

begin
	x := (1 + 2.5) DIV 2 { 1 + 2.5 is a real }
end.
