noinst_LIBRARIES = lib7zip.a

AM_CPPFLAGS = -I$(srcdir)/../includes 


lib7zip_a_SOURCES = \
	7ZipArchive.cpp  7ZipArchiveItem.cpp  7zipLibrary.cpp  HelperFuncs.cpp  HelperFuncs.h  lib7zip.h 

EXTRA_DIST = ReadMe.txt Lib7Zip.vcproj 

dist-hook:
	cp -rf $(srcdir)/Lib7Zip\ -\ VS2008.vcproj $(distdir)


all-local:
	cp lib7zip.a $(srcdir)/../lib

