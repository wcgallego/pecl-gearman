--TEST--
gearman_worker_register(), gearman_worker_unregister(), gearman_worker_unregister_all()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
/*
TODO - requires gearmand to be running
*/
--FILE--
<?php 

$host = 'localhost';
$port = 4730;
$registered_func = 'registered_function';

$worker = new GearmanWorker();
$worker->addServer($host, $port);

print "GearmanWorker::register() (OO): " . ($worker->register($registered_func) === true ? "Success" : "Failure") . PHP_EOL;
print "GearmanWorker::unregister() (OO): " . ($worker->unregister($registered_func) === true ? "Success" : "Failure") . PHP_EOL;
print "GearmanWorker::register() (OO): " . ($worker->register($registered_func) === true ? "Success" : "Failure") . PHP_EOL;
print "GearmanWorker::unregisterAll() (OO): " . ($worker->unregisterAll() === true ? "Success" : "Failure") . PHP_EOL;

$worker2 = gearman_worker_create();
gearman_worker_add_server($worker, $host, $port);

print "gearman_worker_register() (Procedural): " . (gearman_worker_register($worker, $registered_func) === true ? "Success" : "Failure") . PHP_EOL;
print "gearman_worker_unregister() (Procedural): " . (gearman_worker_unregister($worker, $registered_func) === true ? "Success" : "Failure") . PHP_EOL;
print "gearman_worker_register() (Procedural): " . (gearman_worker_register($worker, $registered_func) === true ? "Success" : "Failure") . PHP_EOL;
print "gearman_worker_unregister_all() (Procedural): " . (gearman_worker_unregister_all($worker) === true ? "Success" : "Failure") . PHP_EOL;

print "OK";

function registered_function() {
	print "I'm in ".__FUNCTION__.PHP_EOL;
}
?>
--EXPECT--
GearmanWorker::register() (OO): Success
GearmanWorker::unregister() (OO): Success
GearmanWorker::register() (OO): Success
GearmanWorker::unregisterAll() (OO): Success
gearman_worker_register() (Procedural): Success
gearman_worker_unregister() (Procedural): Success
gearman_worker_register() (Procedural): Success
gearman_worker_unregister_all() (Procedural): Success
OK

