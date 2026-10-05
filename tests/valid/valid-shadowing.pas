{ This program does a lot of shadowing }
PROGRAM shadowing;

var z : integer;

function x (a : integer) : integer;
begin
	x := a
end;

procedure b;
var b : integer;
begin
	b := 4
end;

function a (b : integer) : integer;
var x : integer;
begin
	x := b;
	a := x
end;

begin
	z := a(x(4))
end.