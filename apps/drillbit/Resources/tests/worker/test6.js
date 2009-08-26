


try
{
	sleep(2000);
}
catch(e)
{
	postMessage(String(e) == "interrupted");
	return;
}

postMessage(0);