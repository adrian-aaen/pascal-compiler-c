{ This program indexes a variable that is not an array }
PROGRAM test;

var x : integer;

begin
	x[1] := 2 { x is not an array }
end.
