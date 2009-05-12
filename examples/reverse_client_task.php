<?php
$gmc= new gearman_client();
$gmc->add_server();

$gmc->set_created_fn("reverse_created");
$gmc->set_status_fn("reverse_status");
$gmc->set_complete_fn("reverse_complete");
$gmc->set_fail_fn("reverse_fail");
$task= $gmc->add_task("reverse", "this is a test", NULL);

if (! $gmc->run_tasks())
{
    echo "ERROR " . $gmc->error() . "\n";
    exit;
}
echo "DONE\n";

function reverse_created($task)
{
    echo "CREATED: " . $task->job_handle() . "\n";

function reverse_status($task)
{
    echo "STATUS: " . $task->job_handle() . " - " . $task->task_numerator() . 
         "/" . $task->task_denominator() . "\n";
}

function reverse_complete($task)
{
    echo "COMPLETE: " . $task->job_handle() . "\n";
}

function reverse_fail($task)
{
    echo "FAILED: " . $task->job_handle() . "\n";
}
?>
