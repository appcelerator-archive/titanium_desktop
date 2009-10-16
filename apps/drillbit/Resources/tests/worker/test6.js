try
{
	sleep(2000);
	postMessage(0);
}
catch(e)
{
	postMessage(String(e) == "interrupted");
}

