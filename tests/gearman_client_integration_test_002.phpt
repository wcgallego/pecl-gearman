--TEST--
GearmanClient::setStatusCallback(), gearman_client_set_status_callback(),
GearmanClient::addTaskStatus(), gearman_client_add_task_status(),
GearmanClient::runTasks(), gearman_client_run_tasks()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php
function reverse_status($task, $context)
{
	print "In " . __FUNCTION__
		. " context is '$context'"
		. PHP_EOL;
}

$client = new GearmanClient();
$client->addServer('localhost');

$handle = $client->doBackground("reverse", "Hello World!");

$client->setStatusCallback("reverse_status");

$oo_context = "context passed in through OO";

$client->addTaskStatus($handle, $oo_context);

// Should print within reverse_status
$client->runTasks();

$client2 = gearman_client_create();
gearman_client_add_server($client2, 'localhost', 4730);

$handle = gearman_client_do_background($client2, "reverse", "Hello World!");

gearman_client_set_status_callback($client2, "reverse_status");

$procedural_context = "context passed in through procedural";

gearman_client_add_task_status($client2, $handle, $procedural_context);

gearman_client_run_tasks($client2);

print "OK";
?>
--EXPECT--
In reverse_status context is 'context passed in through OO'
In reverse_status context is 'context passed in through procedural'
OK
