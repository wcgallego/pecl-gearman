--TEST--
GearmanTask::data(), gearman_task_data(),
GearmanTask::dataSize(), gearman_task_data_size()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
$client->addServer('localhost', 4730);
$client->setCreatedCallback(function ($task) {
	print "GearmanTask::data (OO): "
		. ($task->data() === '' ? 'Success' : 'Failure') . PHP_EOL;
	print "GearmanTask::dataSize (OO): "
		. ($task->dataSize() == 0 ? 'Success' : 'Failure') . PHP_EOL;
});

$client->addTaskBackground("GearmanTaskFunction", "normal", "OO data");
$client->runTasks();


$client2 = gearman_client_create();
gearman_client_add_server($client2, 'localhost', 4730);
gearman_client_set_created_callback($client2, function ($task) {
	print "gearman_task_data (Procedural): "
		. (gearman_task_data($task) === '' ? 'Success' : 'Failure') . PHP_EOL;
	print "gearman_task_data_size (Procedural): "
		. (gearman_task_data_size($task) == 0 ? 'Success' : 'Failure') . PHP_EOL;
});
gearman_client_add_task_background($client2, "GearmanTaskFunction2", "normal");
gearman_client_run_tasks($client2);

print "OK";
?>
--EXPECT--
GearmanTask::data (OO): Success
GearmanTask::dataSize (OO): Success
gearman_task_data (Procedural): Success
gearman_task_data_size (Procedural): Success
OK
