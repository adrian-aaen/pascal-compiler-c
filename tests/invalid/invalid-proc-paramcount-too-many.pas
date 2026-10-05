{ This program has incorrect procedure call argument count }
PROGRAM test;

procedure p (a : integer);
begin
	skip
end;

begin
	p(1, 2) { Only takes a single parameter }
end.
