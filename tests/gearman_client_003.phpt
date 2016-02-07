--TEST--
GearmanClient::error(), gearman_client_error()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
print "GearmanClient::error (OO): '" . $client->error() . "'\n";

$client2 = gearman_client_create();
print "gearman_client_error (Procedural): '" . gearman_client_error($client2) . "'\n";

print "OK";
?>
--EXPECT--
GearmanClient::error (OO): ''
gearman_client_error (Procedural): ''
OK
