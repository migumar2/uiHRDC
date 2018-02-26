bin_PROGRAMS = test7zip

test7zip_SOURCES = \
	stdafx.cpp  stdafx.h  Test7Zip.cpp  

test7zip_LDADD = ../Lib7Zip/lib7zip.a \
	../includes/CPP/Common/libcppcommon.a \
	../includes/CPP/Windows/libcppwindows.a \
	-ldl

EXTRA_DIST = ReadMe.txt Test7Zip.vcproj
