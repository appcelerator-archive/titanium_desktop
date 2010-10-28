// ==UserScript==
// @name	helloworld
// @author	Jeff
// @description	test user script
// @include	app://*-userscript.html
// @version 	0.1
// ==/UserScript==

helloworld();

function helloworld()
{
	Titanium.API.debug("hello world from a user script");
}


document.body.style.backgroundColor = "#f00";