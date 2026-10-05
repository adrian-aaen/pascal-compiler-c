PROGRAM calc;
{small calculator with 4 registers
<register> : integer [1..4]
<value> : real
Options:
 - 0 exit
 - 1 <register> <value> : assign value to register
 - 2 <ra> <rb> <rc> store (rb + rc) in ra (registers)
 - 3 <ra> <rb> <rc> store (rb * rc) in ra (registers)
}

var rt : real;
var r : array [1 .. 4] of real;
var option, rid1, rid2, rid3 : integer;

function readRegister(var regs : array [1 .. 4] of real; regId : integer) : real;
begin
	if regId >= 1 and regId <= 4 then
		readRegister := regs[regId]
	else
		readRegister := 0
end;

procedure writeRegister(var regs : array [1 .. 4] of real; regId : integer; number : real);
begin
	if regId >= 1 and regId <= 4 then
		regs[regId] := number
	else
		skip
end;

begin
	r[1] := 0.0;
	r[2] := 0.0; 
	r[3] := 0.0;
	r[4] := 0.0;
	{writeln(r[1], r[2], r[3], r[4]);}
	{readln(option);}
	option := 1;
	while option <> 0 do
	begin
		if option <= 1 then
		begin
			{readln(rid1, rt);}
			rid1 := 1;
			rt := 2;
			writeRegister(r, rid1, rt)
		end
		else if option <= 2 then
		begin
			{readln(rid1, rid2, rid3);}
			rid1 := 2;
			rid2 := 3;
			rid3 := 4;
			writeRegister(r, rid1, readRegister(r, rid2) + readRegister(r, rid3))
		end
		else if option <= 3 then
		begin
			{readln(rid1, rid2, rid3);}
			rid1 := 2;
			rid2 := 3;
			rid3 := 4;
			writeRegister(r, rid1, readRegister(r, rid2) * readRegister(r, rid3))
		end
		else skip;
		{writeln(r[1], r[2], r[3], r[4]);}
		{readln(option)}
		option := 0
	end
end
.
