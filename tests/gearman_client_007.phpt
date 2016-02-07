--TEST--
GearmanClient::addOptions(), gearman_client_add_options()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
$client->setOptions(GEARMAN_CLIENT_NON_BLOCKING);
print "GearmanClient::options (OO): " . $client->options() . PHP_EOL;
print "GearmanClient::addOptions (OO): " . ($client->addOptions(GEARMAN_CLIENT_FREE_TASKS) ? 'Success' : 'Failure') . PHP_EOL;
print "GearmanClient::options (OO): " . $client->options() . PHP_EOL;

$client2 = gearman_client_create();
gearman_client_set_options($client2, GEARMAN_CLIENT_NON_BLOCKING);
print "gearman_client_options (Procedural): " . gearman_client_options($client2) . PHP_EOL;
print "gearman_client_add_options (Procedural): " . (gearman_client_add_options($client2, GEARMAN_CLIENT_FREE_TASKS) ? 'Success' : 'Failure') . PHP_EOL;
print "gearman_client_options (Procedural): " . gearman_client_options($client2) . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient::options (OO): 2
GearmanClient::addOptions (OO): Success
GearmanClient::options (OO): 34
gearman_client_options (Procedural): 2
gearman_client_add_options (Procedural): Success
gearman_client_options (Procedural): 34
OK
