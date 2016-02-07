--TEST--
GearmanWorker::getErrno(), gearman_client_get_errno()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
print "GearmanClient::getErrno (OO): " . $client->getErrno() . "\n";

$client2 = gearman_client_create();
print "gearman_client_get_errno (Procedural): " . gearman_client_get_errno($client2) . "\n";

print "OK";
?>
--EXPECT--
GearmanClient::getErrno (OO): 0
gearman_client_get_errno (Procedural): 0
OK
