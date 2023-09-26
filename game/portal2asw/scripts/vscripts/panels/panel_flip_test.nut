function Min(num1,num2)
{
	if (num1 < num2)
	{
		return num1
	}
	else
	{
		return num2	
	}
}

function Abs(num)
{
	if (num < 0)
	{
		return -1.0*num
	}
	else
	{
		return num	
	}
}

// --------------------------------------------------------
// StartRipple
// --------------------------------------------------------
function StartRipple(start,width,height)
{
	for(local i=0;i<width;i+=1)
	{
		for(local j=0;j<height;j+=1)
		{
			local panelNum = start + width*j + i
			local time = Abs(0.15*(i-((width-1)/2.0))*(j-((height-1)/2.0)))
			EntFire("panel_flip_" + panelNum + "-panel_flip","Trigger", "", time)
		}
	}
}

// --------------------------------------------------------
// StartRipple2
// --------------------------------------------------------
function StartRipple2(start,width,height)
{
	for(local i=0;i<width;i+=1)
	{
		for(local j=0;j<height;j+=1)
		{
			local panelNum = start + width*j + i
			local time = 0.1*(height*i + 6.0*Abs((j-((height-1)/2.0))) )
			EntFire("panel_flip_" + panelNum + "-panel_flip","Trigger", "", time)
		}
	}
}


// --------------------------------------------------------
// StartRipple3
// --------------------------------------------------------
function StartRipple3(start,width,height)
{
	for(local i=0;i<width;i+=1)
	{
		for(local j=0;j<height;j+=1)
		{
			local panelNum = start + width*j + i
			local time = RandomFloat(0,4.0)
			EntFire("panel_flip_" + panelNum + "-panel_flip","Trigger", "", time)
		}
	}
}