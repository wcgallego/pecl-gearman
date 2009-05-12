<?php
/*
 * Gearman PHP Extension
 *
 * Copyright (C) 2008 James M. Luedke (jluedke@jamesluedke.com)
 *                           Eric Day (eday@oddments.org)
 *
 * Use and distribution licensed under the PHP license.  See
 * the LICENSE file in this directory for full text.
 */

/* create our object */
$gmc= new gearman_client();

/* add the default server */
$gmc->add_server();

$data['src']= $_SERVER['argv'][1];
$data['dest']= "small_" . $_SERVER['argv'][1];
$data['x']= 200;
$data['y']= NULL;

/* run reverse client */
do
{
    $value = $gmc->do("shrink_image", serialize($data));
    switch ($gmc->return_code())
    {
        case GEARMAN_WORK_DATA:
            echo "DATA: $value\n";
            break;
        case GEARMAN_SUCCESS:
            echo "SUCCESS: $value\n";
            break;
        case GEARMAN_WORK_FAIL:
            echo "FAILED\n";
            break;
        case GEARMAN_WORK_STATUS:
            list($numerator, $denominator)= $gmc->do_status();
            echo "Status: $numerator/$denominator\n";
            break;
        default:
            echo "ERR: " . $gmc->error() . "\n";
    }
}
while($gmc->return_code() != GEARMAN_SUCCESS);

echo "DONE: $value\n";

