--TEST--
Test Gearman worker methods
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnect.inc');
?>
--FILE--
<?php
require_once('connect.inc');

print "Start\n";
$job_name = uniqid();
$pid = pcntl_fork();
if ($pid == -1) {
    die("Could not fork");
} else if ($pid > 0) {
    // Parent. This is the worker
    $worker = new GearmanWorker();
    var_dump($worker->addServer($host, $port));
    var_dump($worker->setTimeout(5));
    var_dump($worker->register($job_name, 5));
    var_dump($worker->register($job_name . "1", 5));
    var_dump($worker->register($job_name . "2", 5));
    var_dump($worker->register($job_name . "3", 5));
    var_dump(
        $worker->addFunction(
            $job_name,
            function($job) {
                var_dump($job->workload());
            }
        )
    );
    var_dump($worker->work());
    var_dump($worker->unregister($job_name));
    var_dump($worker->unregisterAll());

    // Wait for child
    $exit_status = 0;
    if (pcntl_wait($exit_status) <= 0) {
        print "pcntl_wait exited with error\n";
    } else if (!pcntl_wifexited($exit_status)) {
        print "child exited with error\n";
    }
} else {
    //Child. This is the client. Don't echo anything here
    $client = new GearmanClient();
    if ($client->addServer($host, $port) !== true) {
        exit(1); // error
    };
    $client->doBackground($job_name, "nothing");
    if ($client->returnCode() != GEARMAN_SUCCESS) {
        exit(2); // error
    }
    exit(0);
}

print "Done";
--EXPECTF--
Start
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(7) "nothing"
bool(true)
bool(true)
bool(true)
Done
