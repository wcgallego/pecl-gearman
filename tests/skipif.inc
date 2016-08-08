<?php

if (!extension_loaded("gearman")) {
    die("skip gearman extension unavailable");
}

if (!extension_loaded("pcntl")) {
    die("skip pcntl extension unavailable");
}
