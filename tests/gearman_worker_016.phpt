--TEST--
GearmanWorker::addFunction(), context param
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
/*
TODO - requires gearmand to be running
*/
--FILE--
<?php 
$host = 'localhost';
$port = 4730;
$job = 'AddFunctionArrCtxTest';
$workload = '{"workload":"test"}';

$client = new GearmanClient();
$client->addServer($host, $port);
$handle = $client->doBackground($job, $workload);
$client->doBackground($job, $workload);
$client->doBackground($job, $workload);
$client->doBackground($job, $workload);

print "GearmanWorker::doBackground() (OO): ".(preg_match('/^H:'.gethostname().':\d+$/', $handle) === 1? 'Success' : 'Failure').PHP_EOL;

$worker = new TestWorker();
print "GearmanWorker::work() (OO, array ctx): " .($worker->work() === true ? 'Success' : 'Failure') . PHP_EOL;
print "GearmanWorker::work() (OO, array ctx): " .($worker->work() === true ? 'Success' : 'Failure') . PHP_EOL;
print "GearmanWorker::work() (OO, array ctx): " .($worker->work() === true ? 'Success' : 'Failure') . PHP_EOL;


print "OK";

class TestWorker extends \GearmanWorker
{
    public function __construct()
    {
	global $job;
        parent::__construct();
        $this->addServer();
        $this->addFunction($job, [$this, 'test'], ['firstArg' => 'firstValue']);
    }

    public function test($job, $context)
    {
        echo "Starting job {$job->workload()}". PHP_EOL;
        $firstArg = $context['firstArg'];
        echo "FirstArg is $firstArg" . PHP_EOL;
    }
}

?>
--EXPECT--
GearmanWorker::doBackground() (OO): Success
Starting job {"workload":"test"}
FirstArg is firstValue
GearmanWorker::work() (OO, array ctx): Success
Starting job {"workload":"test"}
FirstArg is firstValue
GearmanWorker::work() (OO, array ctx): Success
Starting job {"workload":"test"}
FirstArg is firstValue
GearmanWorker::work() (OO, array ctx): Success
OK
