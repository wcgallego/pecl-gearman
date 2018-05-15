--TEST--
GearmanClient::enableExceptionHandler(),gearman_client_enable_exception_handler()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

// Test 1: GearmanClient::addServers, Exception callback disabled. Exceptions
// should be skipped until we call enableExceptionHandler. Port 4731 is not
// being used as the port for GearmanD, so it will fail
$client = new GearmanClient();
$client->addServers('localhost:4731,localhost', false);

// Enabling the exception handler, which will attempt to connect to
// the server and in doing so throw an exception since we can't
// connect to a server that doesn't exist
try {
	$client->enableExceptionHandler();
} catch (Exception $e) {
	print "Exception 1 caught: " . $e->getMessage() . PHP_EOL;
}

// Test 2: GearmanClient::addServers,  Exception callback enabled (by default).
// Here, we don't give the second param, so the exception handler is enabled
// upon calling addServers instead of later in enableExceptionHandler
$client2 = new GearmanClient();

try {
	$client2->addServers('localhost:4731,localhost');
} catch (Exception $e) {
	print "Exception 2 caught: " . $e->getMessage() . PHP_EOL;
}

// Test 3: GearmanClient::addServers, Also, when we explicitly enable in addServers
$client3 = new GearmanClient();

try {
	$client3->addServers('localhost:4731,localhost', true);
} catch (Exception $e) {
	print "Exception 3 caught: " . $e->getMessage() . PHP_EOL;
}

// Now, do the same as above but with addServer (singular)
// Test 4: GearmanClient::addServer, Exception callback disabled
$client4 = new GearmanClient();
$client4->addServer('localhost', 4731, false);

try {
	$client4->enableExceptionHandler();
} catch (Exception $e) {
	print "Exception 4 caught: " . $e->getMessage() . PHP_EOL;
}

// Test 5: GearmanClient::addServer, default
$client5 = new GearmanClient();

try {
	$client5->addServer('localhost', 4731);
} catch (Exception $e) {
	print "Exception 5 caught: " . $e->getMessage() . PHP_EOL;
}

// Test 6: GearmanClient::addServer, explicitly set enableExceptionHandler
$client6 = new GearmanClient();

try {
	$client6->addServer('localhost', 4731, true);
} catch (Exception $e) {
	print "Exception 6 caught: " . $e->getMessage() . PHP_EOL;
}

print "OK";
?>
--EXPECTF--
Exception 1 caught: Failed to set exception option
Exception 2 caught: Failed to set exception option
Exception 3 caught: Failed to set exception option
Exception 4 caught: Failed to set exception option
Exception 5 caught: Failed to set exception option
Exception 6 caught: Failed to set exception option
OK
