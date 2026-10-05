{ Example program implementing some old IP assignment }
PROGRAM alt;
var shift : integer;
begin
	{readln(shift);}
	shift := 3;
	if shift < 0 then
		shift := shift + shift
	else
		shift := shift - shift
	{writeln(shift)}
end
.
