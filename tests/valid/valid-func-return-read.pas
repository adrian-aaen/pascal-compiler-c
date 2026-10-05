{ This program reads a function's return value inside its own body }
PROGRAM test;

function inc (n : integer) : integer;
begin
	inc := n;
	inc := inc + 1
end;

begin
	writeln(inc(1)) { this will print 2 }
end.
