// ==UserScript==
// @name          Ad-less Gmail
// @version       3.1
// @description   Ad-less Gmail
// @author        dfeng - Based on this one: http://userscripts.org/scripts/show/37693
// @include		  http://mail.google.com/mail/
// @include       http://mail.google.com/*
// @include       https://mail.google.com/*
// @include       http://*.mail.google.com/*
// @include       https://*.mail.google.com/*
// ==/UserScript==

// this will change the silly message
// the message isn't immediately there so we need a delay
setTimeout(function()
{
	var bm = document.getElementById("bm");
	if (bm && bm.childNodes && bm.childNodes.length > 0)
	{
		bm.childNodes[1].innerHTML = "Gmail is better with Titanium! w00t";
	};
},20);
