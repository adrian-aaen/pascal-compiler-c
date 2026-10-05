PROGRAM euclid;
{ MiniPAS implementation of Euclid’s algorithm for computing GCDs }
VAR x, y : integer;
function gcd(a, b : integer) : integer;
begin
while a <> b do
begin
while a > b do a := a - b;
while b > a do b := b - a
end;
gcd := a
end;
{ main program starts here }
BEGIN
x := 20;
y := 4;
x := gcd(x,y)
{ readln(x, y); }
{ writeln(gcd(x,y)) }
END.