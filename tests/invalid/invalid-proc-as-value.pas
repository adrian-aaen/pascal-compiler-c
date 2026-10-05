{ This program uses a procedure as a value }
PROGRAM test;

var x : integer;

procedure p;
begin
	skip
end;

begin
	x := p { p has no value }
end.
