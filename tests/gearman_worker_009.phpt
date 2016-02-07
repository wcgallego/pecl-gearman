--TEST--
gearman_worker_set_id()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$id = 'test';

$worker = new GearmanWorker();
print "GearmanWorker::setId() (OO): " . ($worker->setId($id) === true ? "Success" : "Failure").PHP_EOL;

$worker2 = gearman_worker_create();
print "gearman_worker_set_id() (Procedural): " . (gearman_worker_set_id($worker, $id) === true ? "Success" : "Failure").PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanWorker::setId() (OO): Success
gearman_worker_set_id() (Procedural): Success
OK
