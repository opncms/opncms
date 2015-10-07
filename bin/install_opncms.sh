#!/bin/sh

ARCH=$(uname -m | sed 's/x86_//;s/i[3-6]86/32/')

if [ -f /etc/lsb-release -o $(which lsb_release) ]; then
    OS=$(lsb_release -si)
    VER=$(lsb_release -sr)
	CN=$(lsb_release -sc)
elif [ -f /etc/debian_version ]; then
    OS=Debian  #/Ubuntu?
    VER=$(cat /etc/debian_version)
elif [ -f /etc/redhat-release ]; then
    #RedHat/CentOS
	OS=$(cat /etc/redhat-release | awk '{print $1;}')
	VER=$(cat /etc/redhat-release | awk '{print $3;}')
else
    OS=$(uname -s)
    VER=$(uname -r)
fi

echo "Here we just get the CppCMS packages, you should install it yourself"
echo "Or you could install them by instructions: http://cppcms.com/wikipp/en/page/apt"

while true; do
    read -p "Do you wish to install CppCMS by script?" yn
    case $yn in
        [Yy]* ) install_cppcms(); break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done

install_cppcms() {
if [ "$(id -u)" -ne "0" ]; then
	echo "Have no root - just download packages"
        if [ $ARCH="32" ]; then
                wget http://apt.cppcms.com/dists/$CN/main/binary-i386/libcppcms1_1.0.5-1_i386.deb
                wget http://apt.cppcms.com/dists/$CN/main/binary-i386/cppcms-tools_1.0.5-1_i386.deb
                wget http://apt.cppcms.com/dists/$CN/main/binary-i386/Packages.gz
        else
                wget http://apt.cppcms.com/dists/$CN/main/binary-amd64/libcppcms1_1.0.5-1_amd64.deb
                wget http://apt.cppcms.com/dists/$CN/main/binary-amd64/cppcms-tools_1.0.5-1_amd64.deb
                wget http://apt.cppcms.com/dists/$CN/main/binary-amd64/Packages.gz
        fi
else
	echo "Have the root - try to install packages"
	echo "Currently there is no keys on server, so you can use"
	echo "apt-get --allow-unauthenticated for you risk"
	if [ $CN ]; then
		case $CN in
		squeeze|wheezy|precise|trusty|utopic) apt-add-repository 'deb http://apt.cppcms.com/ ${CN} main' && aptitude update && aptitude install libcppcms1 cppcms-tools;;
		*) apt-add-repository 'deb-src http://apt.cppcms.com/ ${CN} main' && apt-get source cppcms && apt-get build-dep cppcms && cd cppcms-1.0.5 && dpkg-buildpackage;;
		esac
	fi	
fi

}

install_opncms() {
# Get the opnCMS sources
	wget http://download.opncms.com/
# Compile
	tar -jxvf opncms-0.0.1.tar.bz2
	cd opncms-0.0.1
	mkdir build
	cd build
	cmake ..
	make
# Install
	cp opncms /usr/bin
	cp ../bin/opncms /usr/bin
	cp ../bin/opncms-install /usr/bin
}