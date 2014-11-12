<?php
  function getVersion($string) {
    if (preg_match("#(\d+\.\d+(\.\d+)*)#", $string, $match)) {
        return $match[1];
    }
  }

  # get HTTP GET parameters
  $clientVersion = explode('.', $_GET['version']);

  #echo $clientVersion[1];

  $releaseVersion = array(2,7,3);

  # versions comparition
  $count = count($releaseVersion);

  for ($i = 0; $i < $count; $i++) {
    if ($clientVersion[$i] == $releaseVersion[$i])
      continue;
    if ($clientVersion[$i] < $releaseVersion[$i]) {
      echo 'new_version';
      exit;
    }
    # when above conditions failed break loop and print 'no_new_version' text
    break;
  }
  # client version is up-to-date or newer than release or release is unavailable
  echo "no_new_version";
?>
