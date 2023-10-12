/* $Id$ */

#include "php.h"
#include "ext/standard/info.h"


#define EVAL_CALLBACK_FUNCTION  "__eval"

static const char module_name[] = "evalhook";

static zend_op_array* (*old_compile_string)(zend_string *source_string, const char *filename);


static zend_op_array* evalhook_compile_string(zend_string *source_string, const char *filename)
{
	zend_op_array *op_array = NULL;
	int op_compiled = 0;

	if(strstr(filename, "eval()'d code")) {
		if(zend_hash_str_exists(CG(function_table), EVAL_CALLBACK_FUNCTION, strlen(EVAL_CALLBACK_FUNCTION))) {
			zval function;
			zval retval;
			zval parameter[2];

			ZVAL_STR(&parameter[0], source_string);
			ZVAL_STRING(&function, EVAL_CALLBACK_FUNCTION);
			ZVAL_STRING(&parameter[1], filename);

			if(call_user_function(CG(function_table), NULL, &function, &retval, 2, parameter) == SUCCESS) {
				switch(Z_TYPE(retval)) {
					case IS_STRING:
						op_array = old_compile_string(Z_STR(retval), filename);
					case IS_FALSE:
						op_compiled = 1;
						break;
				}
			}

			zval_dtor(&function);
			zval_dtor(&retval);
			zval_dtor(&parameter[1]);
		}
	}

	if(op_compiled) {
		return op_array;
	} else {
		return old_compile_string(source_string, filename);
	}
}

/* Evasion protection
 * ==================
 *
 * Some code try to evade analysis by checking if the evalhook extension is
 * loaded before calling eval(). So we override the extension_loaded function
 * so it will return false if the module name is the same as our module name.
 */
zif_handler original_handler_extension_loaded;

ZEND_NAMED_FUNCTION(evalhook_extension_loaded)
{
	zend_string * module;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(module)
		ZEND_PARSE_PARAMETERS_END();

	if (zend_string_equals_cstr(module, module_name, sizeof module_name) == 0) {
		RETURN_FALSE;
	}

	// Pass control on to the original handler
	original_handler_extension_loaded(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_MINIT_FUNCTION(evalhook)
{
	// If the ZEND_TSRMLS_CACHE_UPDATE() is in RINIT, move it
	// to MINIT to ensure access to the compiler globals
#if defined(COMPILE_DL_MY_EXTENSION) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	zend_function * original =
		zend_hash_str_find_ptr( CG(function_table), "extension_loaded", sizeof("extension_loaded") - 1);

	if (original) {
		original_handler_extension_loaded = original->internal_function.handler;
		original->internal_function.handler = evalhook_extension_loaded;
	}

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(evalhook)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(evalhook)
{
	old_compile_string = zend_compile_string;
	zend_compile_string = evalhook_compile_string;

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(evalhook)
{
	zend_compile_string = old_compile_string;
	return SUCCESS;
}

PHP_MINFO_FUNCTION(evalhook)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "eval() hooking", "enabled");
	php_info_print_table_row(2, "callback function", EVAL_CALLBACK_FUNCTION);
	php_info_print_table_end();
}


zend_function_entry evalhook_functions[] = {
	ZEND_FE_END
};

zend_module_entry evalhook_module_entry = {
	STANDARD_MODULE_HEADER,
	module_name,
	evalhook_functions,
	PHP_MINIT(evalhook),
	PHP_MSHUTDOWN(evalhook),
	PHP_RINIT(evalhook),
	PHP_RSHUTDOWN(evalhook),
	PHP_MINFO(evalhook),
	"0.0.1-dev",
	STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(evalhook)

