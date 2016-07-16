--TEST--
GearmanClient::context(), GearmanClient::setContext(), gearman_client_context(), gearman_client_set_context()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
print "GearmanClient::setContext(OO): " . ($client->setContext('context1_context1_context1_context1') ? 'Success' : 'Failure') . PHP_EOL;
print "GearmanClient::context (OO): " . $client->context() . PHP_EOL;

$client2 = gearman_client_create();
print "gearman_client_set_context (Procedural): " . (gearman_client_set_context($client2, 'context2_context2') ? 'Success' : 'Failure') . PHP_EOL;
print "gearman_client_context (Procedural): " . gearman_client_context($client2) . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient::setContext(OO): Success
GearmanClient::context (OO): context1_context1_context1_context1
gearman_client_set_context (Procedural): Success
gearman_client_context (Procedural): context2_context2
OK
