--TEST--
GearmanClient::doStatus(), gearman_client_do_status()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$job_name = uniqid();

$client = new GearmanClient();
$client->addOptions(GEARMAN_CLIENT_NON_BLOCKING);
$client->addServer('localhost', 4730);

# Note: Still need to figure out why doNormal is blocking despite
# GEARMAN_CLIENT_NON_BLOCKING
#$job_handle = $client->doNormal($job_name . "_OO", "test_doStatus");
list($numerator, $denominator) = $client->doStatus();

print "GearmanClient::doStatus() (OO): " . PHP_EOL
	. "  Numerator is 0: " . ($numerator == 0 ? 'Success' : 'Failure') .
	PHP_EOL
	. "  Denominator is 0: " . ($denominator == 0 ? 'Success' : 'Failure') .
	PHP_EOL;

$client2 = gearman_client_create();
gearman_client_add_options($client2, GEARMAN_CLIENT_NON_BLOCKING);
gearman_client_add_server($client2, 'localhost', 4730);

# Note: Still need to figure out why doNormal is blocking despite
# GEARMAN_CLIENT_NON_BLOCKING
#$job_handle = gearman_client_do_normal->($client2, $job_name . "_procedural", "test_doStatus");
list($numerator, $denominator) = gearman_client_do_status($client2);

print "gearman_client_do_status() (Procedural): " . PHP_EOL
	. "  Numerator is 0: " . ($numerator == 0 ? 'Success' : 'Failure') .
	PHP_EOL
	. "  Denominator is 0: " . ($denominator == 0 ? 'Success' : 'Failure') .
	PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient::doStatus() (OO): 
  Numerator is 0: Success
  Denominator is 0: Success
gearman_client_do_status() (Procedural): 
  Numerator is 0: Success
  Denominator is 0: Success
OK
