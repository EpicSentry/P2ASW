bugreporter
{
	"config_file"		"\\bugbait.valvesoftware.com\bugsub\config.txt"		
	alt_config 
	{
		"config_file"		"\\bugbait.valvesoftware.com\bugsub\config.txt"
		"network"			"172.16.0.0"
		"netmask"			"255.255.248.0"
	}
		alt_config 
	{
		"config_file"		"\\bugproxy-vs.valvesoftware.com\bugsub\config.txt"
		"network"			"172.17.0.0"
		"netmask"			"255.255.255.0"
	}
}