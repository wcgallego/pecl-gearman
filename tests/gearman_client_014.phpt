--TEST--
GearmanClient::wait(), gearman_client_wait()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
$client->addServer('localhost', 4730);
print "Placeholder for GearmanClient::wait function" . PHP_EOL;
/*
print "GearmanClient::wait (OO): " . ($client->wait() ? 'Success' : 'Failure') . PHP_EOL;
*/

$client2 = gearman_client_create();
/*
print "gearman_client_wait (Procedural): " . (gearman_client_wait($client2) ? 'Success' : 'Failure') . PHP_EOL;
*/

print "OK";
?>
--EXPECT--
Placeholder for GearmanClient::wait function
OK
