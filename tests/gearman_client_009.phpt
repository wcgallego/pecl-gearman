--TEST--
GearmanClient::timeout(), gearman_client_timeout()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
print "GearmanClient::timeout (OO): " . $client->timeout() . PHP_EOL;

$client2 = gearman_client_create();
print "gearman_client_timeout (Procedural): " . gearman_client_timeout($client2) . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient::timeout (OO): -1
gearman_client_timeout (Procedural): -1
OK
