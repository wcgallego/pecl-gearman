--TEST--
GearmanTask::jobHandle(), gearman_task_job_handle()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
$client->addServer('localhost', 4730);
$client->setCreatedCallback(function ($task) {
	print "GearmanTask::jobHandle (OO): "
		. (preg_match('/^(.*):(.*):(.*)$/', $task->jobHandle())
			? 'String matches' : 'String does not match')
		. PHP_EOL;
});

$client->addTaskBackground("GearmanTaskFunction", "normal");
$client->runTasks();


$client2 = gearman_client_create();
gearman_client_add_server($client2, 'localhost', 4730);
gearman_client_set_created_callback($client2, function ($task) {
	print "gearman_task_job_handle (Procedural): "
		. (preg_match('/^(.*):(.*):(.*)$/', $task->jobHandle())
			? 'String matches' : 'String does not match')
		. PHP_EOL;
});
gearman_client_add_task_background($client2, "GearmanTaskFunction2", "normal");
gearman_client_run_tasks($client2);

print "OK";
?>
--EXPECT--
GearmanTask::jobHandle (OO): String matches
gearman_task_job_handle (Procedural): String matches
OK
