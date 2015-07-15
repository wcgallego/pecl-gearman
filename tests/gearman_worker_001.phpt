--TEST--
GearmanWorker::clone(), gearman_worker_clone()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$worker = new GearmanWorker();
$worker_clone = $worker->clone();
print "GearmanWorker clone class (OO): ". get_class($worker) . "\n";

$worker2 = gearman_worker_create();
$worker2_clone = gearman_worker_clone($worker);
print "GearmanWorker clone class (Procedural): ". get_class($worker2_clone) . "\n";

print "OK";
?>
--EXPECT--
GearmanWorker clone class (OO): GearmanWorker
GearmanWorker clone class (Procedural): GearmanWorker
OK
