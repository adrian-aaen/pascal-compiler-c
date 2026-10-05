{ This program redeclares a global as a parameter of a different type, which
  is allowed because the parameter lives in its own scope }
PROGRAM test;

var x : integer;

procedure p (x : real);
var y : integer;
begin
	y := 1
end;

begin
	x := 1;
	p(2.5)
end.
