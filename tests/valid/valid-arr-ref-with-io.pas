{ This program does some work on arrays,
  passing them by reference and using I/O. }
program arr;

var arr : array [5 .. 6] of real;

procedure some (var rra : array [2 .. 3] of real);
begin
	rra[3] := rra[3] + 20
end;

begin
	readln(arr[5], arr[6]);
	some(arr);
	writeln(arr[5], arr[6])
end.