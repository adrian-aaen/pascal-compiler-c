program arr;

const N = 5;

var arr : array [5 .. 6] of real;
var x : real;

function some (rra : array [1 .. 2] of real; y : real; z : real) : real;
begin
	some := rra[1] + y + z
end;

begin
	arr[5] := 1;
	arr[6] := 2;
	x := some(arr, arr[5], N);
	x := some(arr, 5, 5.6)
end.