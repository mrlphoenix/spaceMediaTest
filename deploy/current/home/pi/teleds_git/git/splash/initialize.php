<?php
header('Content-type: text/plain; Charset=UTF-8');
error_reporting(E_ALL ^ E_NOTICE);
ini_set('display_errors', 'on');


define('RESULT_PATH', '/home/pi/teleds/splash.png');
date_default_timezone_set('Europe/Moscow');



$cpunum = substr(file_get_contents('/home/pi/teleds/crc32.txt'),0,8);

$bg = imagecreatefrompng(__DIR__ . '/splash.png');
$color = imagecolorallocate($bg, 255, 255, 255);

$fontname = __DIR__ . '/fonts/pt-sans.ttf';

$box = imagettfbbox(70, 0, $fontname, date("d.m.y H:i")." ".$cpunum);
$left = 1900 - abs($box[4] - $box[0]);

imagettftext($bg, 70, 0, $left, 100, $color, $fontname, date("d.m.y H:i")." ".$cpunum);

imagepng($bg, RESULT_PATH);
imagedestroy($bg);
