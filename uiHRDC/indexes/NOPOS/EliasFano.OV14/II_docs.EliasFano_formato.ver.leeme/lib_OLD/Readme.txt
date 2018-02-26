this directory (formerly ../lib) is no longer needed, since
during compilation (../compile.sh script) a link is created in 
the parent directory as:

   ln -s ../../II_docs/lib/ lib

So that the path to ./lib/il_2.1.vbyte_bitmap_nosampling_64bit.a
is available during compilation

That library is compiled in ../../II_docs/ilists.imp directory

