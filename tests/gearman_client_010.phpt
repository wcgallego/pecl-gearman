--TEST--
GearmanClient::setTimeout(), gearman_client_set_timeout()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
print "GearmanClient::timeout (OO): " . $client->timeout() . PHP_EOL;
print "GearmanClient::setTimeout (OO): " . ($client->setTimeout(3) ? 'Success' : 'Failure') . PHP_EOL;
print "GearmanClient::timeout (OO): " . $client->timeout() . PHP_EOL;

$client2 = gearman_client_create();
print "gearman_client_timeout (Procedural): " . gearman_client_timeout($client2) . PHP_EOL;
print "gearman_client_set_timeout (Procedural): " . (gearman_client_set_timeout($client2, 3) ? 'Success' : 'Failure') . PHP_EOL;
print "gearman_client_timeout (Procedural): " . gearman_client_timeout($client2) . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient::timeout (OO): -1
GearmanClient::setTimeout (OO): Success
GearmanClient::timeout (OO): 3
gearman_client_timeout (Procedural): -1
gearman_client_set_timeout (Procedural): Success
gearman_client_timeout (Procedural): 3
OK
