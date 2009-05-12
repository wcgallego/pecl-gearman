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

echo "Starting\n";

# Create our client object.
$gmclient= new gearman_client();

# Add default server (localhost).
$gmclient->add_server();

echo "Sending job\n";

# Send reverse job
do
{
  $result= $gmclient->do("reverse", "Hello!");
  # Check for various return packets and errors.
  switch($gmclient->return_code())
  {
    case GEARMAN_WORK_DATA:
      echo "Data: $result\n";
      break;
    case GEARMAN_WORK_STATUS:
      list($numerator, $denominator)= $gmclient->do_status();
      echo "Status: $numerator/$denominator complete\n";
      break;
    case GEARMAN_WORK_FAIL:
      echo "Failed\n";
      exit;
    case GEARMAN_SUCCESS:
      break;
    default:
      echo "RET: " . $gmclient->return_code() . "\n";
      break;
  }
}
while($gmclient->return_code() != GEARMAN_SUCCESS);
echo "Success: $result\n";

?>
