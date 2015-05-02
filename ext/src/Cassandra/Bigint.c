#include <errno.h>
#include "php_cassandra.h"

PHP_CASSANDRA_API zend_class_entry* cassandra_ce_Bigint = NULL;

static int
ctype_digit(const char* s, int len)
{
  int i;
  for (i = 0; i < len; i++) {
    if (!isdigit(s[i]))
      return 0;
  }

  return 1;
}

/* {{{ proto Cassandra\Bigint Bigint::__construct(string $number) */
PHP_METHOD(Bigint, __construct)
{
  char* value;
  int value_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &value, &value_len) == FAILURE)
    return;

  cassandra_bigint* number = (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (!php_cassandra_parse_int64(value, value_len, &number->value TSRMLS_CC))
    return;
}
/* }}} */

/* {{{ proto string Bigint::__toString() */
PHP_METHOD(Bigint, __toString)
{
  char* string;
  cassandra_bigint* number = (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
#ifdef WIN32
  spprintf(&string, 0, "%I64d", (long long int) number->value);
#else
  spprintf(&string, 0, "%lld", (long long int) number->value);
#endif
  RETURN_STRING(string, 0);
}
/* }}} */

/* {{{ proto string Bigint::value() */
PHP_METHOD(Bigint, value)
{
  char* string;
  cassandra_bigint* number = (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
#ifdef WIN32
  spprintf(&string, 0, "%I64d", (long long int) number->value);
#else
  spprintf(&string, 0, "%lld", (long long int) number->value);
#endif
  RETURN_STRING(string, 0);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry Bigint_methods[] = {
  PHP_ME(Bigint, __construct, arginfo___construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, value, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_bigint_handlers;

static HashTable*
php_cassandra_bigint_properties(zval *object TSRMLS_DC)
{
  cassandra_bigint* number = (cassandra_bigint*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*        props  = zend_std_get_properties(object TSRMLS_CC);

  zval* value;
  char* string;
  int string_len;

#ifdef WIN32
  string_len = spprintf(&string, 0, "%I64d", (long long int) number->value);
#else
  string_len = spprintf(&string, 0, "%lld", (long long int) number->value);
#endif

  MAKE_STD_ZVAL(value);
  ZVAL_STRINGL(value, string, string_len, 0);

  zend_hash_update(props, "value", sizeof("value"), &value, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_bigint_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  cassandra_bigint* bigint1 = (cassandra_bigint*) zend_object_store_get_object(obj1 TSRMLS_CC);
  cassandra_bigint* bigint2 = (cassandra_bigint*) zend_object_store_get_object(obj2 TSRMLS_CC);

  if (bigint1->value == bigint2->value)
    return 0;
  else if (bigint1->value < bigint2->value)
    return -1;
  else
    return 1;
}

static void
php_cassandra_bigint_free(void *object TSRMLS_DC)
{
  cassandra_bigint* number = (cassandra_bigint*) object;

  zend_object_std_dtor(&number->zval TSRMLS_CC);

  efree(number);
}

static zend_object_value
php_cassandra_bigint_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_bigint *number;

  number = (cassandra_bigint*) ecalloc(1, sizeof(cassandra_bigint));

  zend_object_std_init(&number->zval, class_type TSRMLS_CC);
  object_properties_init(&number->zval, class_type);

  retval.handle   = zend_objects_store_put(number, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_bigint_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_bigint_handlers;

  return retval;
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(Bigint)
{
  zend_class_entry ce;

  memcpy(&cassandra_bigint_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_bigint_handlers.get_properties  = php_cassandra_bigint_properties;
  cassandra_bigint_handlers.compare_objects = php_cassandra_bigint_compare;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Bigint", Bigint_methods);

  cassandra_ce_Bigint = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_ce_Bigint->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_ce_Bigint->create_object = php_cassandra_bigint_new;
}
/* }}} */
