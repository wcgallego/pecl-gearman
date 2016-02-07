--TEST--
GearmanWorker::returnCode(), gearman_client_return_code()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
print "GearmanClient::returnCode (OO): " . $client->returnCode() . "\n";

$client2 = gearman_client_create();
print "gearman_client_return_code (Procedural): " . gearman_client_return_code($client2) . "\n";

print "OK";
?>
--EXPECT--
GearmanClient::returnCode (OO): 0
gearman_client_return_code (Procedural): 0
OK
