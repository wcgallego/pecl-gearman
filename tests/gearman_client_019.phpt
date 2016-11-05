--TEST--
GearmanClient::ping(), gearman_client_ping()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
$client->addServer('localhost', 4730);
print "GearmanClient::ping (OO): " . ($client->ping('oo data') ? 'Success' : 'Failure') . PHP_EOL;

$client2 = gearman_client_create();
gearman_client_add_server($client2, 'localhost', 4730);
print "gearman_client_ping (Procedural): "
	. (gearman_client_ping($client2,'procedural data') ? 'Success' : 'Failure') . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient::ping (OO): Success
gearman_client_ping (Procedural): Success
OK
