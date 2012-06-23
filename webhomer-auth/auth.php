<?php

defined( '_HOMEREXEC' ) or die( 'Restricted access' );

require("class/auth/index.php");
require("class/auth/none/settings.php");

class HomerAuthentication extends Authentication {
	                        
    function login($username, $password) {
      $m = new Mongo(SIPX_DB_URL, array("replicaSet" => "sipxecs"));
      $db = $m->imdb;
      $u = $db->entity->findOne(array("uid" => $username, "pntk" => $password));
      if ($u) {
	if (in_array(SIPX_ADMIN_PERM, $u['prm'])) {
          $_SESSION['loggedin'] = $username;
          $_SESSION['userlevel'] = 1;
	}
	return true;
      }

      return true;
    }
}

?>