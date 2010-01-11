<html>
<head>
	<style>body { font-family: Monaco, Consolas; }</style>
	<title>Shades of Blue</title>
</head>
<body>

<?php
	$blue = array(0xEF, 0xEF, 0xFF);
	$text = array(0, 0, 0);
	
	function rgb($arr)
	{
		return $arr[0].",".$arr[1].",".$arr[2];
	}
	
	$step = 15;
	for ($i = 0; $i < 255; $i+=$step)
	{
		$blue[0] -= $step;
		$blue[1] -= $step;
		foreach ($text as &$t) { $t+=$step; }
?>

	<div style="background-color: rgb(<?php echo rgb($blue); ?>); color: rgb(<?php echo rgb($text); ?>);">
		Shades of blue
	</div>

<?php
	}
?>
</body>
</html>