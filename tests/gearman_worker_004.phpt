--TEST--
gearman_worker_errno()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$worker = new GearmanWorker();
print "GearmanWorker::getErrno (OO): " . $worker->getErrno() . PHP_EOL;

$worker2 = gearman_worker_create();
print "gearman_worker_errno (Procedural): " . gearman_worker_errno($worker2) . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanWorker::getErrno (OO): 0
gearman_worker_errno (Procedural): 0
OK
