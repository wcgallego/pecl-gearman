<?php

$min_ver = '1.1.2';

$version_string= exec('gearmand -V');
$version = explode(' ', $version_string)[1];
if (version_compare($version, $min_ver, '<')) {
    die("skip requires gearmand version >= ${min_ver}, found version ${version}\n");
}


