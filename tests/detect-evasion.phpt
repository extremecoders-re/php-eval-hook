--TEST--
evalhook: detect evasion
--EXTENSIONS--
evalhook
--FILE--
<?php
require __DIR__ . '/evalhook_test_functions.inc';

if (extension_loaded('evalhook')
  || extension_loaded('Evalhook')
  || extension_loaded('EvalHook')
  || extension_loaded('EVALHOOK')
  || extension_loaded('eval_hook')
  || extension_loaded('Eval_hook')
  || extension_loaded('Eval_Hook')
  || extension_loaded('EVAL_HOOK'))
{
  echo "evalhook extension detected! Exiting." . PHP_EOL;
  exit;
}

eval("echo 'Hello there!';");
?>
--EXPECT--
===== Dump of eval'd code =====
echo 'Hello there!';
===== End of dumped code =====
Hello there!
