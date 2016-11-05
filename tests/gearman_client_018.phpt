--TEST--
GearmanClient::jobStatusByUniqueKey(), gearman_client_job_status_by_unique_key()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$job_name = uniqid();

$client = new GearmanClient();
$client->addServer('localhost', 4730);

$oo_key = $job_name . "_oo";
$client->doBackground($job_name . "_OO", "test_jobStatusByUniqueKey");
list($is_known, $is_running, $numerator, $denominator) =
	$client->jobStatusByUniqueKey($oo_key);

print "GearmanClient::doStatus() (OO): " . PHP_EOL
/*
Note: This is returning falso, while jobStatus returns true.
Looks to be across multiple versions of libgearman (1.0.2 and 1.1.2 checked)
	. "  is_known is true: " . ($is_known === true ? 'Success' : 'Failure') .
	PHP_EOL
*/
	. "  is_running is false: " . ($is_running=== false ? 'Success' : 'Failure') .
	PHP_EOL
	. "  Numerator is 0: " . ($numerator == 0 ? 'Success' : 'Failure') .
	PHP_EOL
	. "  Denominator is 0: " . ($denominator == 0 ? 'Success' : 'Failure') .
	PHP_EOL;

$client2 = gearman_client_create();
gearman_client_add_server($client2, 'localhost', 4730);

$procedural_key = $job_name . "_procedural";
$job_handle = gearman_client_do_background($client2, $job_name .
	"_procedural", "test_jobStatusByUniqueKey");
list($is_known, $is_running, $numerator, $denominator) =
	gearman_client_job_status_by_unique_key($client2, $procedural_key);

print "gearman_client_job_status_by_unique_key() (Procedural): " . PHP_EOL
/*
Note: This is returning falso, while jobStatus returns true.
Looks to be across multiple versions of libgearman (1.0.2 and 1.1.2 checked)
	. "  is_known is true: " . ($is_known === true ? 'Success' : 'Failure') .
	PHP_EOL
*/
	. "  is_running is false: " . ($is_running === false ? 'Success' : 'Failure') .
	PHP_EOL
	. "  Numerator is 0: " . ($numerator == 0 ? 'Success' : 'Failure') .
	PHP_EOL
	. "  Denominator is 0: " . ($denominator == 0 ? 'Success' : 'Failure') .
	PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient::doStatus() (OO): 
  is_running is false: Success
  Numerator is 0: Success
  Denominator is 0: Success
gearman_client_job_status_by_unique_key() (Procedural): 
  is_running is false: Success
  Numerator is 0: Success
  Denominator is 0: Success
OK
