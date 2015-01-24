found_bin=""

checkbin() {
  echo -n "Checking for $1 ... "
  found_bin=`which $1`

  if [ "x$found_bin" != "x" ] ; then
    echo $found_bin
    return 0
  fi

  echo "not found !"
  return 1
}

# check for libtoolize

checkbin libtoolize || {
  checkbin glibtoolize || {
    echo "neither \"libtoolize\" nor \"glibtoolize\" found !"
    exit 1
  }
}
LIBTOOLIZE=$found_bin

# check for aclocal

checkbin aclocal || {
  exit 1
}
ACLOCAL=$found_bin

# check for automake

checkbin automake || {
  exit 1
}
AUTOMAKE=$found_bin

# check for autoconf

checkbin autoconf || {
  exit 1
}
AUTOCONF=$found_bin

echo

echo "Generating build information ..."
cd `dirname $0`
aclocalinclude="$ACLOCAL_FLAGS"
mkdir -p autotools

echo "Running libtoolize ..."
$LIBTOOLIZE --copy --force --automake || exit 1

echo "Running aclocal $aclocalinclude ..."
$ACLOCAL $aclocalinclude || {
    echo
    echo "**Error**: aclocal failed. This may mean that you have not"
    echo "installed all of the packages you need, or you may need to"
    echo "set ACLOCAL_FLAGS to include \"-I \$prefix/share/aclocal\""
    echo "for the prefix where you installed the packages whose"
    echo "macros were not found"
    exit 1
}

echo "Running automake ..."
$AUTOMAKE -c -a --foreign || ( echo "***ERROR*** automake failed." ; exit 1 )

echo "Running autoconf ..."
$AUTOCONF || ( echo "***ERROR*** autoconf failed." ; exit 1 )

echo
echo "Please run ./configure now."
