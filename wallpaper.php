<?php
	// wallpaper
	// (c) 2015 - 2017 Andrzej Budzanowski

	echo "wallpaper.php".PHP_EOL.
		 "(c) 2015 - 2017 Andrzej Budzanowski".PHP_EOL;

	if ($argc < 2)
	{
		echo "usage:".PHP_EOL.
			 " wallpaper.php folder [width height [epsilon]]" .PHP_EOL;

		return 1;
	}

	$path = $argv[1];

	$width = null;
	$height = null;
	$wdiff = 0;
	$hdiff = 0;
	$epsilon = 0.05;

	if ($argc >= 4)
	{
		$width = intval($argv[2]);
		$height = intval($argv[3]);

		if ($argc >= 5)
		{
			$epsilon = floatval($argv[4]) / 100;
		}
	}

	if (empty($width) || empty($height))
	{
		$width = $height = null;

		echo " Desktop resolution: not set".PHP_EOL;
	} else
	{
		echo " Desktop resolution: {$width}x{$height} (".($epsilon * 100)."%)".PHP_EOL;

		$wdiff = $width * $epsilon;
		$hdiff = $height * $epsilon;
	}

	function getDirectoryFor($path)
	{
		global $width;
		global $height;
		global $wdiff;
		global $hdiff;

		$img = getimagesize($path);

		if (empty($width))
		{
			return [
				$img[0], $img[1],
			];
		} else
		{
			$w = $img[0];
			$h = $img[1];

			if (abs($w - $width) <= $wdiff)
			{
				if (abs($h - $height) <= $hdiff)
				{
					return [ 'good' ];
				}
			}

			return [ 'bad' ];
		}
	}

	function processRecursive($dir, $path)
	{
		foreach (scandir($dir) as $v)
		{
			if (in_array($v, ['.', '..', '_']))
				continue;

			$name = $dir. DIRECTORY_SEPARATOR .$v;

			if (is_dir($name))
				processRecursive ($name, $path);

			$xpath = getDirectoryFor($name);

			$npath = makeDir($path, $xpath);

			rename($name, $npath . DIRECTORY_SEPARATOR . pathinfo($name, PATHINFO_BASENAME));
		}
	}

	function makeDir($base, $dir)
	{
		$xdir = $base . DIRECTORY_SEPARATOR . implode(DIRECTORY_SEPARATOR, $dir);

		@mkdir($xdir, 0777, true);

		return $xdir;
	}

	processRecursive($path, $path . DIRECTORY_SEPARATOR . '_');

	return 1;
