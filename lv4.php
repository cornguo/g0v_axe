<?php

$dataRows = array();
$cookie = NULL;

for ($page = 1; $page <= 24; $page ++) {
    $ch = curl_init('http://axe-level-4.herokuapp.com/lv4/?page=' . $page);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_HTTPHEADER, array(
            'Accept: text/html',
            'User-Agent: MSIE FireFox Chrome Safari',
            'Referer: http://axe-level-4.herokuapp.com/lv4/',
        )
    );
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
    array_shift($rows);
    $dataRows = array_merge($dataRows, $rows);
}

$keys = array('town', 'village', 'name');
$ret = array();

foreach ($dataRows as $row) {
    $ret[] = array_combine($keys, explode(',', $row));
}

echo json_encode($ret, JSON_UNESCAPED_UNICODE);
