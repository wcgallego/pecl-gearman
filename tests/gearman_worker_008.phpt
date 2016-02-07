--TEST--
gearman_worker_timeout(), gearman_worker_set_timeout()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$timeout = 5;

$worker = new GearmanWorker();
print "GearmanWorker::timeout() (OO, before setting): " . $worker->timeout() . PHP_EOL;
print "GearmanWorker::setTimeout() (OO): " .($worker->setTimeout($timeout) === true ? "Success" : "Failure") . PHP_EOL;
print "GearmanWorker::timeout() (OO, after setting): " . $worker->timeout() . PHP_EOL;

$worker2 = gearman_worker_create();
print "gearman_worker_timeout() (Procedural, before setting): " . gearman_worker_timeout($worker2) . PHP_EOL;
print "gearman_worker_set_timeout() (Procedural): " . (gearman_worker_set_timeout($worker2, $timeout) === true ? "Success" : "Failure") . PHP_EOL;
print "gearman_worker_timeout() (Procedural, after setting): " . gearman_worker_timeout($worker2) . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanWorker::timeout() (OO, before setting): -1
GearmanWorker::setTimeout() (OO): Success
GearmanWorker::timeout() (OO, after setting): 5
gearman_worker_timeout() (Procedural, before setting): -1
gearman_worker_set_timeout() (Procedural): Success
gearman_worker_timeout() (Procedural, after setting): 5
OK
