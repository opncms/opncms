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

if [ "$(id -u)" -ne "0" ]; then
	echo "You should run this script as root or install CppCMS by instructions: http://cppcms.com/wikipp/en/page/apt";
	exit 1
fi

if [ $CN ]; then
	case $CN in
    squeeze|wheezy|precise|trusty|utopic) apt-add-repository 'deb http://apt.cppcms.com/ ${CN} main' && aptitude update && apt-get download libcppcms1;;
    *) apt-add-repository 'deb-src http://apt.cppcms.com/ ${CN} main' && apt-get source cppcms && apt-get build-dep cppcms && cd cppcms-1.0.5 && dpkg-buildpackage;;
	esac
	
	#if [ $ARCH="32" ]; then
		#wget http://apt.cppcms.com/dists/$CN/main/binary-i386/libcppcms1_1.0.5-1_i386.deb
		#wget http://apt.cppcms.com/dists/$CN/main/binary-i386/cppcms-tools_1.0.5-1_i386.deb
		#wget http://apt.cppcms.com/dists/$CN/main/binary-i386/Packages.gz
	#else
		#wget http://apt.cppcms.com/dists/$CN/main/binary-amd64/libcppcms1_1.0.5-1_amd64.deb
		#wget http://apt.cppcms.com/dists/$CN/main/binary-amd64/cppcms-tools_1.0.5-1_amd64.deb
		#wget http://apt.cppcms.com/dists/$CN/main/binary-amd64/Packages.gz
	#fi
fi