<?php

$dataRows = array();
$cookie = NULL;

for ($page = 1; $page <= 76; $page ++) {
    $ch = curl_init('http://axe-level-1.herokuapp.com/lv3/');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    if (NULL !== $cookie) {
        curl_setopt($ch, CURLOPT_URL, 'http://axe-level-1.herokuapp.com/lv3/?page=next');
        curl_setopt($ch, CURLOPT_COOKIE, $cookie);
        $data = curl_exec($ch);
    } else {
        curl_setopt($ch, CURLOPT_HEADER, true);
        $data = curl_exec($ch);
        $cookie = substr($data, strpos($data, 'PHPSESSID'));
        $cookie = substr($cookie, 0, strpos($cookie, ';') + 1);
    }

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
    array_shift($rows);
    $dataRows = array_merge($dataRows, $rows);
}

$keys = array('town', 'village', 'name');
$ret = array();

foreach ($dataRows as $row) {
    $ret[] = array_combine($keys, explode(',', $row));
}

echo json_encode($ret, JSON_UNESCAPED_UNICODE);
