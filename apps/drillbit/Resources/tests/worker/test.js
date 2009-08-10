
onmessage = function(msg)
{
	postMessage("you said: "+msg.message);
}