#!/bin/sh
#
# build_lumiera-build-dependencies.sh  - prepare/build on a pristine Debian/Ubuntu system (Docker)
#
#
#------------------------------------------------------Setup
# The Debian DEB Depot to retrieve the source packages
DEBIAN_REPO="http://lumiera.org/debian/"
SRC_DISTRO="stretch"
REPO_SECTION="experimental"
#
# GPG pubkey to trust for signed packages
TRUSTED_GPG=$(dirname $0)/deb.lumiera.org.PUB.gpg
#
# Directory for building DEB packages
PACK_DIR=./pack
#
#------------------------------------------------------Setup(End)
#
fail() {
    echo "\nFAIL: $1\n\n"
    exit 1
}

# Sanity-Checks
[ -f $TRUSTED_GPG ] || fail "GPG Key not found: $TRUSTED_GPG"
mkdir -p $PACK_DIR
[ -w $PACK_DIR ]    || fail "Unable to build. Can not write to $PACK_DIR"


# prepare the host system with Lumiera package sources
cp $TRUSTED_GPG /etc/apt/trusted.gpg.d/
chmod a+r-x /etc/apt/trusted.gpg.d/$(basename $TRUSTED_GPG)
echo "deb-src $DEBIAN_REPO $SRC_DISTRO $REPO_SECTION" > /etc/apt/sources.list.d/55-lumiera.list

apt update
apt install -y build-essential ca-certificates
apt update

# build some special build dependencies from source
WORKDIR=$(pwd -P)
cd $PACK_DIR

echo "\n\n======= building NoBug =======\n"
apt-get -y build-dep nobug
apt-get source --compile nobug
dpkg -i nobug-dev*deb libnobug*deb            || fail "installing NoBug"
echo "\n\n======= building GDLmm =======\n"
apt-get -y build-dep libgdlmm-3-dev
apt-get source --compile libgdlmm-3-dev       || fail "installing GDLmm"
dpkg -i libgdlmm-3*deb

# install the build dependencies of Lumiera from DEB
echo "\n\nInstalling Lumiera build dependencies...\n"
apt-get -y build-dep lumiera

# return to the working directory
cd $WORKDIR

echo "Build-Dependencies of Lumiera successfully installed.\n\n"

