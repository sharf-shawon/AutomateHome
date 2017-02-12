<?php
/*
	Error 101	No data send
	Error 102	No MODE
	Error 103	Invalid MODE
	
	PUSH Errors
	Error 104	No MYID
	Error 105	Invalid MYID
	Error 106	No DID
	Error 107	Invalid DID
	Error 108	No ACTION
	Error 109	Invalid ACTION
	
	PULL Errors
	Error 110	No API
	Error 111	Invalid API
	Error 112	No CID
	Error 113	Invalid CID
	
	
	PUSH: ?MODE=1&MYID=controller1&DID=brl1&ACTION=2
	PULL: ?MODE=2&API=qwertyuiop&CID=controller1
*/
define("API", "qwertyuiop");
define("statusFile", "./.htStatus");
define("OFF", 0);
define("ON", 1);
define("TOG", 2);
//$authDevices = array("wemos1" => array("brl1" => 0, "brf1" => 1, "lamp" => 0, "out1" => 1));
if(!file_exists(statusFile))
	die("Please login as admin");
if(empty($_GET))
	die("!101");
else
{
	if(!isset($_GET["MODE"]))
		die("!102");
	else
	{
		$authDevices = unserialize(base64_decode(file_get_contents(statusFile)));
		//Data Push Mode
		if($_GET["MODE"] == 1)
		{
			if(isset($_GET["MYID"])) 
			{
				if(array_key_exists($_GET["MYID"], $authDevices))
				{
					if(isset($_GET["DID"]))
					{
						if(array_key_exists($_GET["DID"], $authDevices[$_GET["MYID"]]))
						{
							$curStat = $authDevices[$_GET["MYID"]][$_GET["DID"]];
							if(isset($_GET["ACTION"]))
							{
								
								if($_GET["ACTION"] == ON)
									$newStat = ON;
								else if($_GET["ACTION"] == OFF)
									$newStat = OFF;
								else if($_GET["ACTION"] == TOG)
									$newStat = ($curStat == ON) ? OFF : ON;
								else
									die("!109");
								$authDevices[$_GET["MYID"]][$_GET["DID"]] = $newStat;
								$writeStatus = file_put_contents(statusFile, base64_encode(serialize($authDevices))); 
								if($writeStatus != false)
									die("OK");
							}
							else
								die("!108");
						}
						else
							die("!107");
					}
					else
						die("!106");
				}
				else
					die("!105");
			}
			else
				die("!104");
		}
		//Data Pull Mode
		else if($_GET["MODE"] == 2)
		{
			if(isset($_GET["API"]))
			{
				if($_GET["API"] == API)
				{
					if(isset($_GET["CID"]))
					{
						if(array_key_exists($_GET["CID"], $authDevices))
						{
							$returnString = "#";
							foreach($authDevices[$_GET["CID"]] as $device => $status)
							{
								$returnString .= $status;
							}
							die($returnString);
						}
						else
							die("!113");
					}
					else
						die("!112");
				}
				else
					die("!111");
			}
			else
				die("!110");
		}
		else if($_GET["MODE"] == "3")
		{
			if(isset($_GET["CID"]))
			{
				if(array_key_exists($_GET["CID"], $authDevices))
				{
					
				}
				else
					die("!113");
			}
			else
				die("!112");
		}
		else if($_GET["MODE"] == "admin")
		{
			$authDevices = array(
				"controller1" => array( 
					"light1" => 1, 
					"light2" => 1,
					"fan1" => 1, 
					"fan2" => 1
					),
				"sonoff1" => array(
					"lamp1" => 0 
					)
			);
			file_put_contents(statusFile, base64_encode(serialize($authDevices)));
			die("DONE");
		}
		else
			die("!103");
	}
}
?>
