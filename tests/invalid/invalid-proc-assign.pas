{ This program assigns to a procedure }
PROGRAM test;

procedure procwithpars(x : integer; y  : real);
var p : integer;
var q : real;
BEGIN
	p := x;
	q := y
END;

begin
	procwithpars := 2 { Invalid write }
end.