--TEST--
new GearmanClient(), gearman_client_create()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
print "GearmanClient() (OO) class: " . get_class($client) . PHP_EOL;

$client2 = gearman_client_create();
print "gearman_client_create() (Procedural) class: " . get_class($client2) . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient() (OO) class: GearmanClient
gearman_client_create() (Procedural) class: GearmanClient
OK
