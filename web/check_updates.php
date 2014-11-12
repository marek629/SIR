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
  
/*
  # get SIR main page
  $pageName = 'index.php';
  $website = file_get_contents($pageName);
  # create DOM document object
  $doc = new DOMDocument();
  $doc->loadHTML($website);
  $news = $doc->getElementsByTagName('h3');
  # get latest SIR release from website
  $releaseVersion = array();
  foreach ($news as $note) {
    $content = $note->textContent;
    if (strpos($content, 'Release') !== false or strpos($content, 'release') !== false) {
      $releaseVersion = explode('.', getVersion($content));
      break;
    }
  }
  # make equal length arrays
  while (count($clientVersion) < count($releaseVersion))
    array_push($clientVersion, 0);
  while (count($clientVersion) > count($releaseVersion))
    array_push($releaseVersion, 0);

*/
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
