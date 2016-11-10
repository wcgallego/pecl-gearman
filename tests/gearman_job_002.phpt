--TEST--
GearmanJob::returnCode(), gearman_job_return_code(),
GearmanJob::setReturn(), gearman_job_set_return()
--SKIPIF--
<?php if (!extension_loaded("gearman")) print "skip"; ?>
--FILE--
<?php 

$job = new GearmanJob();
print "GearmanJob::returnCode (OO): " . $job->returnCode() . PHP_EOL;
print "GearmanJob::setReturn with value GEARMAN_TIMEOUT (OO): "
	. ($job->setReturn(GEARMAN_TIMEOUT) ? 'Success' : 'Failure')
	. PHP_EOL;
print "GearmanJob::returnCode matches GEARMAN_TIMEOUT (OO): "
	. ($job->returnCode() == GEARMAN_TIMEOUT ? 'Success' : 'Failure') . PHP_EOL;


$job2 = new GearmanJob();
print "gearman_job_return_code (Procedural): " . gearman_job_return_code($job2) . PHP_EOL;
print "gearman_job_set_return with value GEARMAN_TIMEOUT (Procedural): "
	. (gearman_job_set_return($job2, GEARMAN_TIMEOUT) ? 'Success' : 'Failure')
	. PHP_EOL;
print "gearman_job_return_code matches GEARMAN_TIMEOUT (OO): "
	. (gearman_job_return_code($job2) == GEARMAN_TIMEOUT ? 'Success' : 'Failure') . PHP_EOL;

print "OK";
?>
--EXPECT--
GearmanJob::returnCode (OO): 0
GearmanJob::setReturn with value GEARMAN_TIMEOUT (OO): Success
GearmanJob::returnCode matches GEARMAN_TIMEOUT (OO): Success
gearman_job_return_code (Procedural): 0
gearman_job_set_return with value GEARMAN_TIMEOUT (Procedural): Success
gearman_job_return_code matches GEARMAN_TIMEOUT (OO): Success
OK
