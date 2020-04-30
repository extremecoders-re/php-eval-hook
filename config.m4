PHP_ARG_ENABLE(evalhook, whether to enable evalhook support,
[ --enable-evalhook   Enable PHP eval hook support])
if test "$PHP_EVALHOOK" = "yes"; then
  AC_DEFINE(HAVE_EVALHOOK, 1, [Whether you have PHP EVALHOOK])
  PHP_NEW_EXTENSION(evalhook, evalhook.c, $ext_shared)
fi