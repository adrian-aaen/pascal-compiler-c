{ Do a lot of logic }
program logic;

var x, z : integer;
var xs : array [1 .. 10] of integer;
var y : real;
var ys : array [1 .. 10] of real;

begin
	x := 1;
	xs[1] := 2;
	y := 3.2;
	ys[1] := 3.6;
	if x < 10 and xs[1] < 1 or not(y > 0 and ys[1] <= 3 and y < 2.3 and ys[1] > 3.3) then
		z := 100
	else
		z := 999
	{writeln(z)}
end.