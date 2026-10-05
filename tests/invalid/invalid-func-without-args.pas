{ This program reads a function without calling it }
PROGRAM test;

var x : integer;

function f (a : integer) : integer;
begin
	f := a
end;

begin
	x := f { f needs an argument }
end.
