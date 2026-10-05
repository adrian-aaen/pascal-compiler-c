{ This program passes slices of arrays that do not start at index 0, of both
  integers and reals }
PROGRAM slices;

VAR a : ARRAY [1 .. 4] OF integer;
VAR r : ARRAY [10 .. 13] OF real;

FUNCTION sum(s : ARRAY [1 .. 2] OF integer) : integer;
BEGIN
	sum := s[1] + s[2]
END;

FUNCTION rsum(s : ARRAY [1 .. 2] OF real) : real;
BEGIN
	rsum := s[1] + s[2]
END;

BEGIN
	a[1] := 1; a[2] := 2; a[3] := 3; a[4] := 4;
	r[10] := 0.5; r[11] := 1.5; r[12] := 2.5; r[13] := 3.5;
	writeln(sum(a[2..3])); { this will print 5 (2+3) }
	writeln(rsum(r[12..13])) { this will print 6.000000 (2.5+3.5) }
END.
