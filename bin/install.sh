#!/bin/sh

# opnCMS install script
#
# To use it you can open your terminal and type:
#
#    curl https://install.opncms.com/ | sh
#
# opnCMS currently supports:
#   - Mac: OS X 10.7 and above
#   - Linux: x86 and x86_64 systems

run_wrapped()
{

# Clean install of the latest opnCMS version to ~/.opncms (replacing content)
# (~/.opncms contains only dependency packages, there is no personal data here)

RELEASE="0.0.1"

## Using POSIX sh only
PREFIX="/usr/local"

set -u
set -e
exec 1>&2

UNAME=$(uname)
if [ "${UNAME}" != "Linux" -a "${UNAME}" != "Darwin" ]; then
    echo "Sorry, current OS is not supported yet. Please contact to us."
    exit 1
fi

### OSX ###
if [ "${UNAME}" = "Darwin" ] ; then
  if [ "i386" != "$(uname -p)" -o "1" != "$(sysctl -n hw.cpu64bit_capable 2>/dev/null || echo 0)" ] ; then
    echo "Only 64-bit Intel processors are supported at this time."
    exit 1
  fi
  PLATFORM="osx.x86_64"

### Linux ###
elif [ "${UNAME}" = "Linux" ] ; then
  LINUX_ARCH=$(uname -m)
  if [ "${LINUX_ARCH}" = "i686" ] ; then
    PLATFORM="linux.x86_32"
  elif [ "${LINUX_ARCH}" = "x86_64" ] ; then
    PLATFORM="linux.x86_64"
  else
    echo "Unusable architecture: ${LINUX_ARCH}"
    echo "opnCMS only supports i686 and x86_64 for now."
    exit 1
  fi
fi

trap "echo Installation failed." EXIT

if [ -e "$HOME/.opncms" ]; then
  echo "Removing your existing opnCMS installation (there is no personal data here)."
  rm -rf "$HOME/.opncms"
fi

DISTR_URL="https://opncms.com/opncms/${RELEASE}/opncms-${PLATFORM}.tar.gz"

TMPDIR="$HOME/.opncms-install-tmp"
#rm -rf "$TMPDIR"
#mkdir "$TMPDIR"
echo "Downloading opnCMS distribution"
#curl --progress-bar --fail "$DISTR_URL" | tar -xzf - -C "$TMPDIR" -o
test -x "${TMPDIR}/.opncms/opncms"
mv "${TMPDIR}/.opncms" "$HOME"
rm -rf "${TMPDIR}"
test -x "$HOME/.opncms/opncms"

echo
echo "opnCMS ${RELEASE} has been installed in your home directory (~/.opncms) for ${PLATFORM}."

OPNCMS_SYMLINK_TARGET="$(readlink "$HOME/.opncms/opncms")"
OPNCMS_TOOL_DIRECTORY="$(dirname "$OPNCMS_SYMLINK_TARGET")"
STARTER="$HOME/.opncms/$OPNCMS_TOOL_DIRECTORY/scripts/opncms-start"

if cp "$STARTER" "$PREFIX/bin/opncms" >/dev/null 2>&1; then
  echo "Writing a starter script to $PREFIX/bin/opncms for your convenience."
  cat <<"EOF"

To get started fast:

  $ opncms create my_site
  $ cd my_site
  $ opncms

Or see the docs at:

  docs.opncms.com

EOF
elif type sudo >/dev/null 2>&1; then
  echo "Writing a launcher script to $PREFIX/bin/opncms for your convenience."
  echo "This may prompt for your password."

  if [ ! -d "$PREFIX/bin" ] ; then
      sudo mkdir -m 755 "$PREFIX" || true
      sudo mkdir -m 755 "$PREFIX/bin" || true
  fi

  if sudo cp "$LAUNCHER" "$PREFIX/bin/opncms"; then
    cat <<"EOF"

To get started fast:

  $ opncms create my_site
  $ cd my_site
  $ opncms

Or see the docs at:

  docs.opncms.com

EOF
  else
    cat <<EOF

Couldn't write the launcher script. Please either:

  (1) Run the following as root:
        cp "$LAUNCHER" /usr/bin/opncms
  (2) Add "\$HOME/.opncms" to your path, or
  (3) Rerun this command to try again.

Then to get started, take a look at 'opncms help' or see the docs at
docs.opncms.com.
EOF
  fi
else
  cat <<EOF

Now you need to do one of the following:

  (1) Add "\$HOME/.opncms" to your path, or
  (2) Run this command as root:
        cp "$LAUNCHER" /usr/bin/opncms

Then to get started, take a look at 'opncms help' or see the docs at
docs.opncms.com.
EOF
fi

trap - EXIT
}

run_wrapped