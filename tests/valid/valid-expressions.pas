program sign;

const N = 4;
const M = 2.3;

var x, y, xy, xyz : integer;
var z : real;
var as, cs : array [2 .. 2] of real; { Array of size 1 }
var bs : array [2 .. 2] of integer;

begin
	as[2] := 25.3; { assign real to *real* }
	as[2] := 2; { assign int to *real* }
	bs[2] := 1;
	cs[2] := bs[2];
	as[2] := cs[2];
	x := N;
	z := M;
	z := 3 / 2;
	z := 3.5 * 3 + 2 - 1;
	z := 3.4 / 5;
	z := 3.5 / 3.4 - 3.4;
	y := 3 div 2;
	z := bs[2];
	x := 42;
	y := -1;
	xy := x*y;
	xyz := -1*x + (-xy);
	x := -x - x;
	z := as[2]
	{writeln(z)}
end.
