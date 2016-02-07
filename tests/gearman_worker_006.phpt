--TEST--
gearman_worker_set_options()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$worker = new GearmanWorker();
print "GearmanWorker::setOptions (OO): " . ($worker->setOptions(GEARMAN_WORKER_NON_BLOCKING) === true ? 'Success' : 'Failure') . PHP_EOL;
print "GearmanWorker::options() (OO): " . $worker->options() . PHP_EOL;

$worker2 = gearman_worker_create();
print "gearman_worker_set_options (Procedural): " . (gearman_worker_set_options($worker, GEARMAN_WORKER_NON_BLOCKING) === true ? 'Success' : 'Failure') . PHP_EOL;
print "gearman_worker_options() (Procedural): " . gearman_worker_options($worker) . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanWorker::setOptions (OO): Success
GearmanWorker::options() (OO): 6
gearman_worker_set_options (Procedural): Success
gearman_worker_options() (Procedural): 6
OK
