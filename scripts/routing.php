<?php
/**
 * @file
 * The router.php for clean-urls when use PHP 5.4.0 built in webserver.
 *
 * Usage:
 *
 * php -S localhost:8888 routing.php
 *
 */

//Set base_url.
$base_url="http://localhost:8080";

//Parse url.
$url = parse_url($_SERVER["REQUEST_URI"]);
if (file_exists('.' . $url['path'])) {
  // Serve the requested resource as-is.
  return FALSE;
}
// Remove opener slash and fix url encoding.
$_GET['q'] =  $_REQUEST['q'] = str_replace("%20"," ", substr($url['path'], 1));
include 'index.php';
