--TEST--
GearmanClient::addServer(), gearman_client_add_server()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
print "GearmanClient::addServer(OO), implicit port: " . ($client->addServer('localhost') ? 'Success' : 'Failure') . PHP_EOL;
print "GearmanClient::addServer(OO), explicit port: " . ($client->addServer('localhost', 4730) ? 'Success' : 'Failure') . PHP_EOL;

$client2 = gearman_client_create();
print "gearman_client_add_server (Procedural), implicit port: " . (gearman_client_add_server($client2, 'localhost') ? 'Success' : 'Failure') . PHP_EOL;
print "gearman_client_add_server (Procedural): explicit port" . (gearman_client_add_server($client2, 'localhost', 4730) ? 'Success' : 'Failure') . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient::addServer(OO), implicit port: Success
GearmanClient::addServer(OO), explicit port: Success
gearman_client_add_server (Procedural), implicit port: Success
gearman_client_add_server (Procedural): explicit portSuccess
OK
