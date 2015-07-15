--TEST--
gearman_worker_error()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$worker = new GearmanWorker();
print "GearmanWorker::error (OO): '" . $worker->error() . "'\n";

$worker2 = gearman_worker_create();
print "gearman_worker_error (Procedural): '" . gearman_worker_error($worker2) . "'\n";

print "OK";
?>
--EXPECT--
GearmanWorker::error (OO): ''
gearman_worker_error (Procedural): ''
OK
