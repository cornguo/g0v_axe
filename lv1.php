<?php

$ch = curl_init('http://axe-level-1.herokuapp.com/');
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

$data = curl_exec($ch);
$data = strstr($data, '<table');
$data = substr($data, 0, strpos($data, '</table>'));

$pattern = array(
    '/<\/td>/',
    '/\s/',
    '/<\/tr>/',
    '/,EOL/',
);

$replace = array(
    ',',
    '',
    'EOL',
    PHP_EOL,
);

$data = preg_replace($pattern, $replace, $data);
$data = strip_tags($data);

$rows = explode("\n", trim($data));
$keys = explode(',', array_shift($rows));
array_shift($keys);

$ret = array();

foreach ($rows as $row) {
    $tmp = explode(',', $row);
    $name = array_shift($tmp);
    $scores = array();
    foreach ($tmp as $t) {
        $scores[] = intval($t);
    }
    $ret[] = array(
        'name'   => $name,
        'grades' => array_combine($keys, $scores)
    );
}

echo json_encode($ret, JSON_UNESCAPED_UNICODE);
