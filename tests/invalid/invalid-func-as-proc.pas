{ This program calls a function as if it were a procedure }
PROGRAM test;

function f (a : integer) : integer;
begin
	f := a
end;

begin
	f(1) { Using function as procedure }
end.
