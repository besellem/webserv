#!/bin/shc
REQUEST_DATA="name=val_1&email=val_2"
export GATEWAY_INTERFACE="CGI/1.1"
export SERVER_PROTOCOL="HTTP/1.1"
export QUERY_STRING=""
export REDIRECT_STATUS="200"
export SCRIPT_FILENAME=/Users/yek/42/42_Projects/webserv/www/test_cgi/post/action.php
export REQUEST_METHOD="POST"
export CONTENT_LENGTH=${#REQUEST_DATA}
export CONTENT_TYPE="application/x-www-form-urlencoded;charset=utf-8"
echo $REQUEST_DATA | /Users/yek/42/42_Projects/webserv/www/bin/php-cgi