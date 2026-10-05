{ This program declares the same identifier twice in one scope }
PROGRAM test;

var x : integer;
var x : real; { x is already declared }

begin
	skip
end.
