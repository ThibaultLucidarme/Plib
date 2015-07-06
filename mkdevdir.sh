#!/bin/sh

for dir in "$@"
do
	mkdir $dir
	mkdir $dir/src
	mkdir $dir/bin
	mkdir $dir/build
	mkdir $dir/doc
	mkdir $dir/data
	
	
	# CMakeLists.txt for compile
	# .version.in needed by CMakeLists.txt to generate version.hpp used by CommandLineParser.hpp
	cd $dir
	wget https://raw.githubusercontent.com/ThibaultLucidarme/Plib/master/CMakeLists.txt.template --no-check-certificate
	wget https://raw.githubusercontent.com/ThibaultLucidarme/Plib/master/.version.in --no-check-certificate
	mv CMakeLists.txt.template CMakeLists.txt
	
	
	# basic Plib (commandline inputs and progress bar)
	cd src
	wget https://raw.githubusercontent.com/ThibaultLucidarme/Plib/master/core/CommandLineParser.hpp --no-check-certificate
	wget https://raw.githubusercontent.com/ThibaultLucidarme/Plib/master/core/ProgressBar.hpp --no-check-certificate
	cd ..
	
	# .gitignore build directory content
	echo '*
	!.gitignore
	'>build/.gitignore

	
	
done



