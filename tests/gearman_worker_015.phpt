--TEST--
gearman_worker_echo()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
/*
TODO - requires gearmand to be running
*/
--FILE--
<?php 

$host = 'localhost';
$port = 4730;

$worker = new GearmanWorker();
$worker->addServer($host, $port);
echo "GearmanWorker::server() (OO): ".($worker->echo(json_encode(['test' => 'workload'])) ? "Success" : "Failure")."\n";

$worker2 = gearman_worker_create();
gearman_worker_add_server($worker, $host, $port);
echo "gearman_worker_work() (Procedural): ".($worker->echo(json_encode(['test' => 'workload'])) ? "Success" : "Failure")."\n";

print "OK";
?>
--EXPECT--
GearmanWorker::server() (OO): Success
gearman_worker_work() (Procedural): Success
OK
