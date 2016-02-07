--TEST--
gearman_worker_add_server(), gearman_worker_add_servers()
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
print "GearmanWorker::addServer() (OO):" . ($worker->addServer($host, $port) === true ? "Success" : "Failure").PHP_EOL;
print "GearmanWorker::addServers() (OO): " . ($worker->addServers("$host:$port") === true ? "Success" : "Failure").PHP_EOL;

$worker2 = gearman_worker_create();
print "gearman_worker_add_server() (Procedural): " . (gearman_worker_add_server($worker, $host, $port) === true ? "Success" : "Failure").PHP_EOL;
print "gearman_worker_add_servers() (Procedural): " . (gearman_worker_add_servers($worker, "$host:$port") === true ? "Success" : "Failure").PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanWorker::addServer() (OO):Success
GearmanWorker::addServers() (OO): Success
gearman_worker_add_server() (Procedural): Success
gearman_worker_add_servers() (Procedural): Success
OK
