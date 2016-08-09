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

print "Start" . PHP_EOL;
$job_name = uniqid();
$pid = pcntl_fork();
if ($pid == -1) {
    die("Could not fork");
} else if ($pid > 0) {
    // Parent. This is the worker
    $worker = new GearmanWorker();
    print "addServer: " . var_export($worker->addServer($host, $port), true) . PHP_EOL;
    print "setTimeout: " . var_export($worker->setTimeout(5), true) . PHP_EOL;
    print "register: " . var_export($worker->register($job_name, 5), true) . PHP_EOL;
    print "register: " . var_export($worker->register($job_name . "1", 5), true) . PHP_EOL;
    print "register: " . var_export($worker->register($job_name . "2", 5), true) . PHP_EOL;
    print "register: " . var_export($worker->register($job_name . "3", 5), true) . PHP_EOL;
    print "addFunction: " . var_export(
        $worker->addFunction(
            $job_name,
            function($job) {
                print "workload: " . var_export($job->workload(), true) . PHP_EOL;
            }
        ), true
    ) . PHP_EOL;
    print "work: " . var_export($worker->work(), true) . PHP_EOL;
    print "unregister: " . var_export($worker->unregister($job_name), true) . PHP_EOL;
    print "unregisterAll: " . var_export($worker->unregisterAll(), true) . PHP_EOL;

    // Wait for child
    $exit_status = 0;
    if (pcntl_wait($exit_status) <= 0) {
        print "pcntl_wait exited with error" . PHP_EOL;
    } else if (!pcntl_wifexited($exit_status)) {
        print "child exited with error" . PHP_EOL;
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
addServer: true
setTimeout: true
register: true
register: true
register: true
register: true
addFunction: true
workload: 'nothing'
work: true
unregister: true
unregisterAll: true
Done
