{ Passing a variable by reference }
program ref;
var a : integer;

procedure dub(var x : integer);
begin
	x := 2 * x
end;

begin
	a := 2;
	{writeln(a);}
	dub(a)
	{writeln(a)}
end.