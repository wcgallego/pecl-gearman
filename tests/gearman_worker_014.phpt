--TEST--
gearman_worker_work()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
/*
TODO - requires gearmand to be running
*/
--FILE--
<?php 

$host = 'localhost';
$port = 4730;
$job = 'GenericTestJob';
$func = 'do_work';
$workload = '{"workload":"test"}';

// Adding 2 jobs, one for OO and one for procedural
$client = new GearmanClient();
$client->addServer($host, $port);
$handle = $client->doBackground($job, $workload);
$client->doBackground($job, $workload);

print "GearmanWorker::doBackground() (OO): ".(preg_match('/^H:'.gethostname().':\d+$/', $handle) === 1? 'Success' : 'Failure').PHP_EOL;

$worker = new GearmanWorker();
$worker->addServer($host, $port);
$worker->addFunction($job, $func);

print "GearmanWorker::work() (OO): ".($worker->work() === true ? 'Success' : 'Failure') . PHP_EOL;

$worker2 = gearman_worker_create();
gearman_worker_add_server($worker, $host, $port);
gearman_worker_add_function($worker, $job, $func);

print "gearman_worker_work() (Procedural): ".(gearman_worker_work($worker) === true ? 'Success' : 'Failure') . PHP_EOL;

print "OK";

function do_work($job) {
	print "Calling function ".__FUNCTION__.PHP_EOL;
}
?>
--EXPECT--
GearmanWorker::doBackground() (OO): Success
Calling function do_work
GearmanWorker::work() (OO): Success
Calling function do_work
gearman_worker_work() (Procedural): Success
OK
