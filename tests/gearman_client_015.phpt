--TEST--
GearmanClient::doJobHandle(), gearman_client_do_job_handle()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
$client->addServer('localhost', 4730);

$job_handle = $client->doBackground("test_doJobHandle", "test_doJobHandle");
print "GearmanClient::doJobHandle() (OO): "
	. ($job_handle == $client->doJobHandle() ? 'Success' : 'Failure') . PHP_EOL;



$client2 = gearman_client_create();
gearman_client_add_server($client2, 'localhost', 4730);

$job_handle = gearman_client_do_background($client2, "test_do_job_handle", "test_do_job_handle");

print "gearman_client_do_job_handle() (Procedural): "
	. ($job_handle == gearman_client_do_job_handle($client2) ? 'Success' : 'Failure') . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient::doJobHandle() (OO): Success
gearman_client_do_job_handle() (Procedural): Success
OK
