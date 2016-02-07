--TEST--
gearman_worker_add_function()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
/*
TODO - requires gearmand to be running
*/
--FILE--
<?php 

$host = 'localhost';
$port = 4730;
$job = 'GenericJob';
$func = 'do_work';

$worker = new GearmanWorker();
$worker->addServer($host, $port);
print "GearmanWorker::addFunction() (OO): ".($worker->addFunction($job, $func) ? "Success" : "Failure").PHP_EOL;

$worker2 = gearman_worker_create();
gearman_worker_add_server($worker, $host, $port);
print "gearman_worker_add_function() (Procedural): ".(gearman_worker_add_function($worker, $job, $func) ? "Success" : "Failure").PHP_EOL;

print "OK";

function do_work() {
	print "I'm in ".__FUNCTION__.PHP_EOL;
}
?>
--EXPECT--
GearmanWorker::addFunction() (OO): Success
gearman_worker_add_function() (Procedural): Success
OK
