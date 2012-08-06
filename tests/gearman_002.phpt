--TEST--
gearman_worker_set_id()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$worker= new GearmanWorker();
$worker->setId('test');

$worker = gearman_worker_create();
gearman_worker_set_id($worker, 'test');

echo "OK";
?>
--EXPECT--
OK
