<?php
 // wallpaper
 // (c) 2015 Andrzej Budzanowski

 echo "wallpaper.php".PHP_EOL.
      "(c) 2015 Andrzej Budzanowski".PHP_EOL;

 if ( $argc < 2 )
 {
  echo "usage:".PHP_EOL.
       " wallpaper.php folder_to_scan width_desktop height_desktop".PHP_EOL;

  return 1;
 }

 $path = $argv[1];
 $desktop_width  = $argc >= 3 ? $argv[2] : null;
 $desktop_height = $argc >= 4 ? $argv[3] : null;

 if ( $desktop_height == null || $desktop_width == null )
  $desktop_height = $desktop_width = null;

 function is_portrait( $w, $h )
 {
  return $h > $w;
 }

 function is_landscape( $w, $h )
 {
  return $h < $w;
 }

 function is_smaller_then_desktop( $w, $dw )
 {
  if ( $dw == null )
   return false;

  return $w < $dw;
 }

 function rec_move( $base, $current_catalog, $valid )
 {
  foreach ( $valid as $k => $v )
  {
   if ( is_array($v) )
   {
    @mkdir( $current_catalog. DIRECTORY_SEPARATOR. $k );
    rec_move( $base, $current_catalog. DIRECTORY_SEPARATOR. $k, $v );
   } else
   {
    @rename( $base . DIRECTORY_SEPARATOR . $v,
             $current_catalog. DIRECTORY_SEPARATOR . $v );
   }
  }
 }

 $sd     = scandir( $path );
 $valid  = [];

 echo "Scanning directory ";

 $tmp_it = 0;
 $tmp_ml = (count($sd)-2) * 10 / 100;

 foreach ( $sd as $v )
 {
  if ( $v == "." || $v == ".." )
   continue;

  $img_src = getimagesize($argv[1]. DIRECTORY_SEPARATOR . $v);

  if ( is_portrait($img_src[0], $img_src[1]))
  {
   if (is_smaller_then_desktop($img_src[0], $desktop_width))
    $valid['portrait']['smaller'][] = $v;
   else
    $valid['portrait'][$img_src[0]][$img_src[1]][] = $v;
  } else if ( is_landscape($img_src[0], $img_src[1]) )
  {
   if (is_smaller_then_desktop($img_src[1], $desktop_height))
    $valid['landscape']['smaller'][] = $v;
   else
    $valid['landscape'][$img_src[0]][$img_src[1]][] = $v;
  }

  $tmp_it++;
  if ( $tmp_it % $tmp_ml == 0 )
   echo ".";
 }

 echo " DONE".PHP_EOL;

 echo "Moving files ";
 rec_move( $argv[1], $argv[1], $valid );
 echo " DONE".PHP_EOL;

 return 0;
