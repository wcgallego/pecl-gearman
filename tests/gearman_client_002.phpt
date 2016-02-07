--TEST--
GearmanClient::returnCode(), gearman_client_return_code()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
print "GearmanClient::returnCode (OO): " . $client->returnCode() . PHP_EOL;

$client2 = gearman_client_create();
print "gearman_client_return_code (Procedural): " . gearman_client_return_code($client2) . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient::returnCode (OO): 0
gearman_client_return_code (Procedural): 0
OK
