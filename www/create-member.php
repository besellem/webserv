<?php

//!proper way to declare variables obtained from POST.
// Data from form "register.php"
if ( isset($_POST['username']) ) {
    $postUsername = $_POST['username'];
}


echo $postUsername;    // <-- this is line 10