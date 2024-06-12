<?php
# SPDX-FileCopyrightText: 2017 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

$data = file_get_contents('php://input');
$obj = json_decode($data);

if($obj != null) {
	$fileName = 'performanceruns/data' . (new DateTime)->getTimeStamp() . '.json';
	file_put_contents($fileName, $data, FILE_APPEND | LOCK_EX);
}
?>
