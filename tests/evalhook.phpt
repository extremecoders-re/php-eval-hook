--TEST--
evalhook: simple eval
--EXTENSIONS--
evalhook
--FILE--
<?php
require __DIR__ . '/evalhook_test_functions.inc';

eval("echo 'Hello there!';");
?>
--EXPECT--
===== Dump of eval'd code =====
echo 'Hello there!';
===== End of dumped code =====
Hello there!
