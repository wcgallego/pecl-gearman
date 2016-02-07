--TEST--
gearman_worker_options()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$worker = new GearmanWorker();
print "GearmanWorker::options (OO): " . $worker->options() . PHP_EOL;

$worker2 = gearman_worker_create();
print "gearman_worker_options (Procedural): " . gearman_worker_options($worker2) . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanWorker::options (OO): 644
gearman_worker_options (Procedural): 644
OK
