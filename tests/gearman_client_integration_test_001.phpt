--TEST--

--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php

$host = 'localhost';
$port = '4730';

$job_name = uniqid();

$pid = pcntl_fork();
if ($pid == -1) {
	die("Could not fork");
} else if ($pid > 0) {
	// Parent. This should be the Worker
	$worker = new GearmanWorker();
	$worker->addServer($host, $port);

	print "addFunction: " . var_export(
		$worker->addFunction(
			$job_name,
			function($job) {
				print "workload: " . var_export($job->workload(), true) . PHP_EOL;
			}
		),
		true
	) . PHP_EOL;

	for($i=0; $i<6; $i++) {
		$worker->work();
	}

	print "unregister: " . var_export($worker->unregister($job_name), true) . PHP_EOL;

	// Wait for child
	$exit_status = 0;
	if (pcntl_wait($exit_status) <= 0) {
		print "pcntl_wait exited with error" . PHP_EOL;
	} else if (!pcntl_wifexited($exit_status)) {
		print "child exited with error" . PHP_EOL;
	}

	print "OK" . PHP_EOL;
} else {
	// Child. This is the Client
	$client = new GearmanClient();
	$client->addServer($host, $port);

	$job_types = ['doNormal', 'doHigh', 'doLow'];
	foreach ($job_types as $job_type) {
		$unique_key = "{$job_name}_{$job_type}";
		$workload = "Workload for $job_type";
		$handle = $client->$job_type($job_name, $workload, $unique_key);
	}

	// Background jobs can run into a race condition if they complete out of
	// order
	$job_types = ['doBackground', 'doHighBackground', 'doLowBackground'];
	foreach ($job_types as $job_type) {
		$unique_key = "{$job_name}_{$job_type}";
		$workload = "Workload for $job_type";
		$handle = $client->$job_type($job_name, $workload, $unique_key);

		do {
			usleep(10000);
			list($is_known, $is_running, $numerator, $denominator) =
				$client->jobStatus($handle);
		} while ($is_known === true || $is_running === true);
	}
}
?>
--EXPECT--
addFunction: true
workload: 'Workload for doNormal'
workload: 'Workload for doHigh'
workload: 'Workload for doLow'
workload: 'Workload for doBackground'
workload: 'Workload for doHighBackground'
workload: 'Workload for doLowBackground'
unregister: true
OK
