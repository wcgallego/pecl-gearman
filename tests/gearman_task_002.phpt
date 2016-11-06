--TEST--
GearmanTask::unique, gearman_task_unique
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
$client->addServer('localhost', 4730);
$task = $client->addTask("GearmanTaskFunction", "normal");
print "GearmanTask::unique (OO): "
	. (preg_match('/^(.*)-(.*)-(.*)-(.*)-(.*)$/', $task->unique())
		? 'String matches' : 'String does not match')
	. PHP_EOL;

$client2 = gearman_client_create();
gearman_client_add_server($client2, 'localhost', 4730);
$task2 = gearman_client_add_task($client2, "GearmanTaskFunction2", "normal");
print "gearman_client_unique (Procedural): "
	. (preg_match('/^(.*)-(.*)-(.*)-(.*)-(.*)$/', gearman_task_unique($task))
		? 'String matches' : 'String does not match')
	. PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanTask::unique (OO): String matches
gearman_client_unique (Procedural): String matches
OK
