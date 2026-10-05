{ Parameters of one subprogram must not leak into later ones: here some
  parameters share a name with a global or with a later subprogram. }
PROGRAM scopeleak;

VAR x : integer;
VAR y : real;

function twice (inc : integer) : integer; { parameter named like a later procedure }
begin
	twice := inc + inc
end;

function half (bump : real) : real; { parameter named like a later function }
begin
	half := bump / 2
end;

procedure inc (var v : integer);
begin
	v := v + 1
end;

function bump (var r : real) : real;
begin
	r := r + 1;
	bump := r
end;

procedure seven (var y : integer); { integer var-parameter y, global y is real }
begin
	y := 7
end;

procedure show;
begin
	writeln(x, y) { the globals: an integer and a real }
end;

begin
	x := 3;
	y := 1.5;
	inc(x);
	writeln(bump(y)); { this will print 2.500000 }
	writeln(twice(x), half(y)); { this will print 8 1.250000 }
	show { this will print 4 2.500000 }
end.
