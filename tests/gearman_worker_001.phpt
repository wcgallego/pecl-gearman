--TEST--
gearman_worker_return_code()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$worker = new GearmanWorker();
print "GearmanWorker::returnCode (OO): " . $worker->returnCode() . PHP_EOL;

$worker2 = gearman_worker_create();
print "gearman_worker_return_code (Procedural): " . gearman_worker_return_code($worker2) . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanWorker::returnCode (OO): 0
gearman_worker_return_code (Procedural): 0
OK
