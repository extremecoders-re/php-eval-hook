<?php
function __eval(string $code) {
	echo PHP_EOL . "===== Dump of eval'd code =====" . PHP_EOL;
	echo $code;
	echo PHP_EOL . "===== End of dumped code =====" . PHP_EOL;
}
