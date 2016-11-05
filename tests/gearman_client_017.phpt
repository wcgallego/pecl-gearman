--TEST--
GearmanClient::jobStatus(), gearman_client_job_status()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$job_name = uniqid();

$client = new GearmanClient();
$client->addServer('localhost', 4730);

$job_handle = $client->doBackground($job_name . "_OO", "test_jobStatus");
list($is_known, $is_running, $numerator, $denominator) = $client->jobStatus($job_handle);

print "GearmanClient::jobStatus() (OO): " . PHP_EOL
	. "  is_known is true: " . ($is_known === true ? 'Success' : 'Failure') .
	PHP_EOL
	. "  is_running is false: " . ($is_running=== false ? 'Success' : 'Failure') .
	PHP_EOL
	. "  Numerator is 0: " . ($numerator == 0 ? 'Success' : 'Failure') .
	PHP_EOL
	. "  Denominator is 0: " . ($denominator == 0 ? 'Success' : 'Failure') .
	PHP_EOL;

$client2 = gearman_client_create();
gearman_client_add_server($client2, 'localhost', 4730);

$job_handle = gearman_client_do_background($client2, $job_name .
	"_procedural", "test_jobStatus");
list($is_known, $is_running, $numerator, $denominator) =
	gearman_client_job_status($client2, $job_handle);

print "gearman_client_job_status() (Procedural): " . PHP_EOL
	. "  is_known is true: " . ($is_known === true ? 'Success' : 'Failure') .
	PHP_EOL
	. "  is_running is false: " . ($is_running === false ? 'Success' : 'Failure') .
	PHP_EOL
	. "  Numerator is 0: " . ($numerator == 0 ? 'Success' : 'Failure') .
	PHP_EOL
	. "  Denominator is 0: " . ($denominator == 0 ? 'Success' : 'Failure') .
	PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient::jobStatus() (OO): 
  is_known is true: Success
  is_running is false: Success
  Numerator is 0: Success
  Denominator is 0: Success
gearman_client_job_status() (Procedural): 
  is_known is true: Success
  is_running is false: Success
  Numerator is 0: Success
  Denominator is 0: Success
OK
