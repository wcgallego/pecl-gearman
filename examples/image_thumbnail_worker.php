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

echo "Starting Thumbnail creator\n";

$gmw= new gearman_worker();
$gmw->add_server();

# optional config paramsj
$args;

$gmw->add_function("shrink_image", "resize_image", $args);

while($gmw->work())
{
    switch ($gmw->return_code())
    {
        case GEARMAN_SUCCESS:
            break;
        default:
            echo "ERROR RET: " . $gmc->return_code() . "\n";
            exit;
    }
}
echo "DONE\n";


/* simple function to resize an image
 * Requires the Imagick extension */
function resize_image($job, $args)
{
    $wrk= $job->workload();
    $data= unserialize($wrk);
    if (! $data['src'] || ! $data['dest'] || ! $data['x']) 
    { $job->fail(); print_r($data); return; }
    echo $job->handle() . " - creating: $data[dest] x:$data[x] y:$data[y]\n";
    $im= new Imagick();
    $im->readimage($data['src']);
    $im->thumbnailImage($data['x'], $data['y']);
    $im->writeImage($data['dest']);
    $im->destroy();
    $job->status(1, 1);

    return $data['dest'];
}
?>
