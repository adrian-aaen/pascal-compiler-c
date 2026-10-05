{ This program assigns to a function outside of its body }
PROGRAM test;

function f (a : integer) : integer;
begin
	f := a
end;

begin
	f := 3 { Only allowed inside f }
end.
