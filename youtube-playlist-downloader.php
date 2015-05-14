<?php
 // youtube-playlist
 // (c) 2015 Andrzej Budzanowski
 
 // argumenty
 // klucz playlista
 if ( $argc <= 2 )
  die( "Brak argumentow!" );
  
 $ytkey = $argv[1];
 $playlist = $argv[2];
 
 $url = "https://www.googleapis.com/youtube/v3/playlistItems?";
 $url .= http_build_query([ "key" => $ytkey, "playlistId" => $playlist, "maxResults" => 50, "part" => "contentDetails" ]);
 $ret_data = '';
 
 $stream_ctx = stream_context_create([
  'http' => [
   'method' => 'GET',
  ],
 ]);
 
 do
 {
  if ( !empty($data) && isset($data['nextPageToken']) )
   $data = file_get_contents( $url.'&pageToken='.$data['nextPageToken'], false, $stream_ctx );
  else
   $data = file_get_contents( $url, false, $stream_ctx );
   
  $data = json_decode( $data, true );
 
  // zapisujemy do pliku
  foreach ( $data['items'] as $v )
   $ret_data .= "https://www.youtube.com/watch?".http_build_query([ 'v' => $v['contentDetails']['videoId'] ]).PHP_EOL;
 } while ( isset($data['nextPageToken']) );
  
 file_put_contents("playlist.txt", $ret_data);
 