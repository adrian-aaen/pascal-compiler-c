program missing;
{ Takes a number N, then N unique numbers <= N of which one is 0. Prints the missing number <= N }

var n, total, a, i : integer;

begin
	{readln(n);}
	n := 4;
	total := n;
	i := 0;

	while i < n do
	begin
		{readln(a);}
		a := 3;
		total := total + i;
		total := total - a;
		i := i + 1
	end

	{writeln(total)}
end.