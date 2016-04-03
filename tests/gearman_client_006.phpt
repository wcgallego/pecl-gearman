--TEST--
GearmanClient::setOptions(), gearman_client_set_options()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
$client->setOptions(32);
print "GearmanClient::options (OO): " . $client->options() . PHP_EOL;
print "GearmanClient::setOptions (OO): " . ($client->setOptions(GEARMAN_CLIENT_NON_BLOCKING) ? 'Success' : 'Failure') . PHP_EOL;
print "GearmanClient::options (OO): " . $client->options() . PHP_EOL;

$client2 = gearman_client_create();
gearman_client_set_options($client2, 32);
print "gearman_client_options (Procedural): " . gearman_client_options($client2) . PHP_EOL;
print "gearman_client_set_options (Procedural): " . (gearman_client_set_options($client2, GEARMAN_CLIENT_NON_BLOCKING) ? 'Success' : 'Failure') . PHP_EOL;
print "gearman_client_options (Procedural): " . gearman_client_options($client2) . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient::options (OO): 32
GearmanClient::setOptions (OO): Success
GearmanClient::options (OO): 2
gearman_client_options (Procedural): 32
gearman_client_set_options (Procedural): Success
gearman_client_options (Procedural): 2
OK
