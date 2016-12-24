--TEST--
Test Gearman worker methods
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifversion.inc');
require_once('skipifconnect.inc');

?>
--FILE--
<?php
require_once('connect.inc');

print "Start" . PHP_EOL;

$job_name = uniqid();

$contexts = [
        "success",
	"fail",
	"exception"
    ];

$pid = pcntl_fork();
if ($pid == -1) {
    die("Could not fork");
} else if ($pid == 0) {
    // Child. This is the worker.
    // Don't echo anything here
    $worker = new GearmanWorker();
    $worker->addServer($host, $port);
    $worker->addFunction(
        $job_name,
        function($job) {
            if ($job->workload() == "success") {
                return "done";
            } else if ($job->workload() == "exception") {
                $job->sendException("unhandled");
                return "exception";
            } else if ($job->workload() == "fail") {
                $job->sendFail();
                return "fail";
            }
        }
    );

    for ($i = 0; $i < count($contexts); $i++) {
        $worker->work();
    }

    $worker->unregister($job_name);
    exit(0);
} else {
    //Parent. This is the client.
    $client = new GearmanClient();
    if ($client->addServer($host, $port) !== true) {
        exit(1); // error
    };

    $client->setCompleteCallback(function($task) {
        print "Complete: " . $task->data() . PHP_EOL;
    });
    $client->setDataCallback(function($task) {
        print "Data: " . $task->data() . PHP_EOL;
    });
    $client->setExceptionCallback(function($task) {
        print "Exception: " . $task->data() . PHP_EOL;
    });
    $client->setFailCallback(function($task) {
        print "Fail" . PHP_EOL;
    });

    $tasks = [];
    foreach ($contexts as $c) {
        $tasks[] = $client->addTask($job_name, $c);
    }
    $client->runTasks();
    print "returnCode: " . var_export($client->returnCode(), true) . PHP_EOL;
    print "clearCallbacks: " . var_export($client->clearCallbacks(), true) . PHP_EOL;

    // Wait for child
    $exit_status = 0;
    if (pcntl_wait($exit_status) <= 0) {
        print "pcntl_wait exited with error" . PHP_EOL;
    } else if (!pcntl_wifexited($exit_status)) {
        print "child exited with error" . PHP_EOL;
    }
}

print "Done";
--EXPECTF--
Start
Exception: unhandled
Fail
Complete: done
returnCode: 0
clearCallbacks: true
Done
