<?php

require_once('connect.inc');

$sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if ($sock === false) {
    die("skip unable to create socket");
}

if (socket_connect($sock, $host, $port) !== true) {
    die("skip unable to connect");
}

$command = "getpid\n";
if (socket_write($sock, $command) !== strlen($command)) {
    die("skip unable to write getpid");
}

if (socket_read($sock, 8) === false) {
    die("skip unable to read pid");
}

socket_close($sock);
