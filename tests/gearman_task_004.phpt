--TEST--
GearmanTask::is_known(), gearman_task_is_known(),
GearmanTask::is_running(), gearman_task_is_running(),
GearmanTask::numerator(), gearman_task_numerator(),
GearmanTask::denominator(), gearman_task_denominator()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
$client->addServer('localhost', 4730);
$client->setCreatedCallback(function ($task) {
	print "GearmanTask::isKnown (OO): "
		. ($task->isKnown() === true ? 'Success' : 'Failure') . PHP_EOL;
	print "GearmanTask::isRunning (OO): "
		. ($task->isRunning() === false ? 'Success' : 'Failure') . PHP_EOL;
	print "GearmanTask::taskNumerator (OO): "
		. ($task->taskNumerator() == 0 ? 'Success' : 'Failure') . PHP_EOL;
	print "GearmanTask::taskDenominator (OO): "
		. ($task->taskDenominator() == 0 ? 'Success' : 'Failure') . PHP_EOL;
});

$client->addTaskBackground("GearmanTaskFunction", "normal");
$client->runTasks();


$client2 = gearman_client_create();
gearman_client_add_server($client2, 'localhost', 4730);
gearman_client_set_created_callback($client2, function ($task) {
	print "gearman_task_is_known (Procedural): "
		. (gearman_task_is_known($task) === true ? 'Success' : 'Failure') . PHP_EOL;
	print "gearman_task_is_running (Procedural): "
		. (gearman_task_is_running($task) === false ? 'Success' : 'Failure') . PHP_EOL;
	print "gearman_task_numerator (Procedural): "
		. (gearman_task_numerator($task) == 0 ? 'Success' : 'Failure') . PHP_EOL;
	print "gearman_task_denominator (Procedural): "
		. (gearman_task_denominator($task) == 0 ? 'Success' : 'Failure') . PHP_EOL;
});
gearman_client_add_task_background($client2, "GearmanTaskFunction2", "normal");
gearman_client_run_tasks($client2);

print "OK";
?>
--EXPECT--
GearmanTask::isKnown (OO): Success
GearmanTask::isRunning (OO): Success
GearmanTask::taskNumerator (OO): Success
GearmanTask::taskDenominator (OO): Success
gearman_task_is_known (Procedural): Success
gearman_task_is_running (Procedural): Success
gearman_task_numerator (Procedural): Success
gearman_task_denominator (Procedural): Success
OK
