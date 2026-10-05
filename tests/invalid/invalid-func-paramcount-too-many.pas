{ This program has incorrect function call argument count }
PROGRAM test;

var x : integer;

function a (x : integer) : integer;
begin
	a := 3
end;

begin
	x := a(5, 6) { Only takes a single parameter }
end.