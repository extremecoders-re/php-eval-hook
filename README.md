# php-eval-hook

A PHP extension for hooking `eval()`. Useful for dumping eval-obfuscated code. The extension is expected to work with PHP 7.x in general. May work with PHP 5.x but is untested.

## Compiling steps

A precompiled extension can be downloaded from [releases](https://github.com/extremecoders-re/php-eval-hook/releases). It has been compiled against PHP 7.2.24 available on Ubuntu 18.04 repos. The precompiled extension is not guaranteed to work on your system. Hence it's always recommended to compile from source as shown below.

1. Install php and php-dev packages. Here we are using the packages available on the official Ubuntu 18.04 repos.

    ```
    $ sudo apt install php7.2 php7.2-dev
    ```

2. Clone the repository

    ```
    $ git clone https://github.com/extremecoders-re/php-eval-hook
    ```

3. Run `phpize`. This will generate the `Makefile` and other files needed for buidling the extension.

    ```
    $ cd php-eval-hook
    $ phpize
    ```

4. Build the extension. `make install` copies the `.so` to the appropriate location.

    ```
    $ ./configure --enable-evalhook

    $ make

    $ make install
    Installing shared extensions:     /usr/lib/php/20170718/
    ```

## Registering the extension with PHP

1. Find the location  of *php.ini*.

    ```
    $ php -r 'phpinfo();' | grep php.ini
    Configuration File (php.ini) Path => /etc/php/7.2/cli
    Loaded Configuration File => /etc/php/7.2/cli/php.ini
    ```

2. Edit *php.ini* and add the line `extension=evalhook.so` at the end.
    ```
    $ echo "extension=evalhook.so" >> /etc/php/7.2/cli/php.ini
    ```

3. Ensure that the extension is properly loaded.
    ```
    $ php -r 'print_r(get_loaded_extensions());' | grep evalhook
        [14] => evalhook
    ```

    ```
    $ php -r 'phpinfo();' | grep eval
    evalhook
    eval() hooking => enabled
    callback function => __eval
    ```

## Usage

You must define a callback function named `__eval` in your PHP code. The extension will call this function whenever it encounters an `eval`. Inside your callback you can print the code to stdout or dump to a file or whatever you want.

### Example

**Original Code**
```php
<?php
function test_obfuscated()
{
	echo("This is an obfuscated function\n");
}

test_obfuscated();
?>
```
**obfuscated.php** [Generated from [Simple online PHP obfuscator](https://www.mobilefish.com/services/php_obfuscator/php_obfuscator.php)]
```
<?php
eval(str_rot13(gzinflate(str_rot13(base64_decode('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')))));
?>
```

**harness.php**
```php
<?php
function __eval($code, $file) {
  echo "eval() @ {$file}:\n{$code}\n\n";

  // return FALSE if you want to prevent the eval()
  // return false;

  // return a string if you want to replace the eval-d code
  // return 'echo 2;';

  // return nothing to continue execution normally
}

include("obfuscated.php");
?>
```

```
$ php obfuscated.php
This is an obfuscated function
```

```
$ php harness.php | tail

eval() @ /workspace/php7/obfuscated.php(2) : eval()'d code(1) : eval()'d code(1) : eval()'d code(1) : eval()'d code(1) : eval()'
d code(1) : eval()'d code(1) : eval()'d code(1) : eval()'d code(1) : eval()'d code(1) : eval()'d code(1) : eval()'d code(1) : ev
al()'d code(1) : eval()'d code(1) : eval()'d code(1) : eval()'d code(1) : eval()'d code:
function test_obfuscated()
{
        echo("This is an obfuscated function\n");
}

test_obfuscated();

This is an obfuscated function
```

## Credits

The extension is based on [php-eval](https://github.com/mfmans/php-eval).

## License

MIT
