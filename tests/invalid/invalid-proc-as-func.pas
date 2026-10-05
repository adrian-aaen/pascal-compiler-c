{ This program uses a procedure as a function }
PROGRAM test;

var x : integer;

procedure a (x : integer);
begin
end;

begin
	x := a(3) { Using procedure as function }
end.