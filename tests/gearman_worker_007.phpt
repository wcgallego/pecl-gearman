--TEST--
gearman_worker_add_options()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$worker = new GearmanWorker();
print "GearmanWorker::addOptions() (OO): " . ($worker->addOptions(GEARMAN_WORKER_NON_BLOCKING) === true ? 'Success' : 'Failure') . "\n";
print "GearmanWorker::options() (OO): " . $worker->options() . "\n";

$worker2 = gearman_worker_create();
print "gearman_worker_add_options() (Procedural): " . (gearman_worker_add_options($worker2, GEARMAN_WORKER_NON_BLOCKING) === true ? 'Success' : 'Failure') . "\n";
print "gearman_worker_options() (OO): " . gearman_worker_options($worker2) . "\n";

print "OK";
?>
--EXPECT--
GearmanWorker::addOptions() (OO): Success
GearmanWorker::options() (OO): 646
gearman_worker_add_options() (Procedural): Success
gearman_worker_options() (OO): 646
OK
