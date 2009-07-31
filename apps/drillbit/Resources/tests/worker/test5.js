
onmessage = function(event)
{
	postMessage([typeof(event.message), event.message[1]]);
}