<?php

$ini_content = file("/etc/sipxpbx/mongo-client.ini");
$line = $ini_content[0];
$value = substr($line,strpos($line,'=') + 1, strlen($line));
$rs = substr($value,0,strpos($value,'?'));
define('SIPX_DB_URL', $rs);
define('SIPX_ADMIN_PERM',"superadmin");

?>
