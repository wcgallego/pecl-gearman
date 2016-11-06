--TEST--
GearmanClient::clearCallbacks(), gearman_client_clear_callbacks()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$client = new GearmanClient();
$client->addServer('localhost', 4730);
print "GearmanClient::clearCallbacks (OO) (No callbacks set): " . ($client->clearCallbacks() ? 'Success' : 'Failure') . PHP_EOL;
$client->setWorkloadCallback(function($task, $context){});
$client->setCreatedCallback(function($task, $context){});
$client->setDataCallback(function($task, $context){});
$client->setWarningCallback(function($task, $context){});
$client->setStatusCallback(function($task, $context){});
$client->setCompleteCallback(function($task, $context){});
$client->setExceptionCallback(function($task, $context){});
$client->setFailCallback(function($task, $context){});
print "GearmanClient::clearCallbacks (OO) (Callbacks set): " . ($client->clearCallbacks() ? 'Success' : 'Failure') . PHP_EOL;

$client2 = gearman_client_create();
gearman_client_add_server($client2, 'localhost', 4730);
print "gearman_client_clear_callbacks (Procedural) (No callbacks set: "
	. (gearman_client_clear_callbacks($client2) ? 'Success' : 'Failure') . PHP_EOL;
gearman_client_set_workload_callback($client2, function($task, $context){});
gearman_client_set_created_callback($client2, function($task, $context){});
gearman_client_set_data_callback($client2, function($task, $context){});
gearman_client_set_warning_callback($client2, function($task, $context){});
gearman_client_set_status_callback($client2, function($task, $context){});
gearman_client_set_complete_callback($client2, function($task, $context){});
gearman_client_set_exception_callback($client2, function($task, $context){});
gearman_client_set_fail_callback($client2, function($task, $context){});
print "gearman_client_clear_callbacks (Procedural) (Callbacks set: "
	. (gearman_client_clear_callbacks($client2) ? 'Success' : 'Failure') . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanClient::clearCallbacks (OO) (No callbacks set): Success
GearmanClient::clearCallbacks (OO) (Callbacks set): Success
gearman_client_clear_callbacks (Procedural) (No callbacks set: Success
gearman_client_clear_callbacks (Procedural) (Callbacks set: Success
OK
