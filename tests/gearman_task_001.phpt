--TEST--
GearmanTask::functionName, gearman_task_function_name
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
$client->addServer('localhost', 4730);
$task = $client->addTask("GearmanTaskFunction", "normal");
print "GearmanTask::functionName (OO): " . $task->functionName() . PHP_EOL;

$client2 = gearman_client_create();
gearman_client_add_server($client2, 'localhost', 4730);
$task2 = gearman_client_add_task($client2, "GearmanTaskFunction2", "normal");
print "gearman_client_function_name (Procedural): "
	. gearman_task_function_name($task2) . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanTask::functionName (OO): GearmanTaskFunction
gearman_client_function_name (Procedural): GearmanTaskFunction2
OK
