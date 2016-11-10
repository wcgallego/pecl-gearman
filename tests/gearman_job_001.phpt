--TEST--
new GearmanJob()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$job = new GearmanJob();
print "GearmanJob() (OO) class: " . get_class($job) . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanJob() (OO) class: GearmanJob
OK
