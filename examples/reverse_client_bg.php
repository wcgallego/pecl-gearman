<?php
/*
 * Gearman PHP Extension
 *
 * Copyright (C) 2008 James M. Luedke <contact@jamesluedke.com>,
 *                    Eric Day <eday@oddments.org>
 * All rights reserved.
 *
 * Use and distribution licensed under the PHP license.  See
 * the LICENSE file in this directory for full text.
 */

/* create our object */
$gmclient= new gearman_client();

/* add the default server */
$gmclient->add_server();

/* run reverse client */
$job_handle = $gmclient->do_background("reverse", "this is a test");

if ($gmclient->return_code() != GEARMAN_SUCCESS)
{
  echo "bad return code\n";
  exit;
}

echo "JOB HANDLE: $job_handle\n";
while (1)
{
  list($is_known, $is_running, $numerator, $denominator)= $gmclient->job_status($job_handle);
  if (! $is_known)
  {
      echo "NOT KNOWN!\n";
      break;
  }
  sleep(1);

  echo "Known= $is_known Running= $is_running - $numerator/$denominator\n";
}

?>
