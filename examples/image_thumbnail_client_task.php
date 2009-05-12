<?php
/*
 * Gearman PHP Extension
 *
 * Copyright (C) 2008 James M. Luedke (jluedke@jamesluedke.com)
 *                           Eric Day (eday@oddments.org)
 * All rights reserved.
 *
 * Use and distribution licensed under the PHP license.  See
 * the LICENSE file in this directory for full text.
 */

/* create our object */
$gmc= new gearman_client();

/* add the default server */
$gmc->add_server();

/* set a few callbacks */
$gmc->set_created_fn("thumb_created");
$gmc->set_complete_fn("thumb_complete");
$gmc->set_fail_fn("thumb_fail");

for ($x= 0; $x<20; $x++)
{
    $data[$x]['src']= $_SERVER['argv'][1];
    $data[$x]['dest']= "$x.jpg";
    $data[$x]['x']= ((80+1)*($x+1));
    $data[$x]['y']= NULL;
}

/* fire off each job */
foreach ($data as $img)
{
    /* NOTE: if you want to asynchronously queue jobs use
    ** $task= $gmc->add_task_background("shrink_image", serialize($img));
    ** however keep in mind that your complete callback will not get called */
    if (! $gmc->add_task("shrink_image", serialize($img)))
    {
        echo "ERROR RET: " . $gmc->error() . "\n";
        exit;
    }
}

if (! $gmc->run_tasks())
{
    echo "ERROR RET:" . $gmc->error() . "\n";
    exit;
}
echo "DONE\n";
exit;

function thumb_created($task)
{
    echo "CREATED -> job: " . $task->job_handle() . "\n";
}

function thumb_complete($task)
{
    echo "COMPLETE -> job: " . $task->job_handle() . 
         " new_file: " . $task->data() . "\n";
}

function thumb_fail($task)
{
    echo "FAIL job: " . $task->job_handle() . "\n";
}

?>
