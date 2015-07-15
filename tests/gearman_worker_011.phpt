--TEST--
gearman_worker_wait()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$host = 'localhost';
$port = 4730;

$worker = new GearmanWorker();
$worker->addServer($host, $port);
$worker->setTimeout(1);
/*
Still need to figure out how to test wait here...
$worker->wait();
*/

print "OK";
?>
--EXPECT--
OK
