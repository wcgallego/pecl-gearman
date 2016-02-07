--TEST--
GearmanClient::addServers(), gearman_client_add_servers()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
print "GearmanClient::addServer(OO): " . ($client->addServers('localhost,localhost:4730') ? 'Success' : 'Failure') . PHP_EOL;

$client2 = gearman_client_create();
print "gearman_client_add_servers (Procedural): " . (gearman_client_add_servers($client2, 'localhost,localhost:4730') ? 'Success' : 'Failure') . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient::addServer(OO): Success
gearman_client_add_servers (Procedural): Success
OK
