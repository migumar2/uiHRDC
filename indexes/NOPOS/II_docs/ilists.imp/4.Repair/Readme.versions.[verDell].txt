What you need from here.

[A] ------------------
There are 4 directories where you can find the implementations for Repair/Repair-skip/Repair-skip-CM/Repair-skip-ST
from our Journal [Inf. Systems] paper.

 ** 2.integradoBB.ClaudeSolucion2013   
    --> Repair: can use approximate (Claude's) repair or exact (Gonzalo's) repair.

 ** 3.integradoSkippingBB.ClaudeSolucion2013_v2(sinwarningscompilacion)
    --> Repair-skip: can use approximate (Claude's) repair or exact (Gonzalo's) repair.

** 4.integradoSkippingBB.Moffat.2015[ver.leeme]
   --> Repair-skip-CM [sampling] : can use approximate (Claude's) repair or exact (Gonzalo's) repair.

** 5.integradoSkippingBB.Sanders.2015[ver.leeme]
   --> Repair-skip-ST [sampling] : can use approximate (Claude's) repair or exact (Gonzalo's) repair.

In each of those directories you can enter and $make, so that you will obtain the corresponding library "xx.a" file (the one you will link with the indexer/searcher main program. Those files are named respectively as:

	- il_4.2.repair_64bit.a 
	- il_4.3.repair_skipping_64bit.a
	- il_4.3.2.repair_skipping_sanders_64bit.a
	- il_4.3.1.repair_skipping_moffat_64bit.a

[B] ------------------
Apart from that, to obtain a repair-compressed representation of a set of posting lists you will need the 
following binary executables that will be called from the indexer.


  path2convert=./srcREPAIR/convert; 
  path2build_bitmap=./srcREPAIR/build_bitmap; 
  path2build_index=./srcREPAIR/build_index;
  path2irepairgonzalo=srcREPAIR/repairGonzalo.srcs/irepair; 
  path2irepairgonzalo=srcREPAIR/repairGonzalo.srcs/irepair_bal; 

  path2build_index=./srcREPAIR/build_index_gonzalo; 
  path2compress_dictionary=./srcREPAIR/compressDictionary

  You can obtain those 7 exes by:
	cd 2.integradoBB.ClaudeSolucion2013 ; make clean ; make ; cd ..
	cd "2.integradoBB.ClaudeSolucion2013/srcREPAIR"  ;  make clean; make ;  cd ../..
	cd "2.integradoBB.ClaudeSolucion2013/srcREPAIR/repairGonzalo.srcs"  ;  rm -f irepair ; make  ;  cd ../../..
	cd "2.integradoBB.ClaudeSolucion2013/srcREPAIR/repairGonzalo.srcs/bal" ; rm -f irepair ; make ; cd ../../../..

	## THIS ARE THE FINAL FILES YOU WILL NEED 
	cp 2.integradoBB.ClaudeSolucion2013/srcREPAIR/convert .
	cp 2.integradoBB.ClaudeSolucion2013/srcREPAIR/build_bitmap .
	cp 2.integradoBB.ClaudeSolucion2013/srcREPAIR/build_index .
	cp 2.integradoBB.ClaudeSolucion2013/srcREPAIR/repairGonzalo.srcs/irepair .
	cp 2.integradoBB.ClaudeSolucion2013/srcREPAIR/repairGonzalo.srcs/irepair ./irepair_bal 
	cp 2.integradoBB.ClaudeSolucion2013/srcREPAIR/build_index_gonzalo .
	cp 2.integradoBB.ClaudeSolucion2013/srcREPAIR/compressDictionary .


[C] ------------------------
Copy files:
	- il_4.2.repair_64bit.a 
	- il_4.3.repair_skipping_64bit.a
	- il_4.3.2.repair_skipping_sanders_64bit.a
	- il_4.3.1.repair_skipping_moffat_64bit.a	

and the 7 exe-binaries to the indexer/lib directory, and that's it!!





======================================================================
============ FURTHER NOTES, IN SPANISH ===============================
======================================================================

**NOTAS CIKM.
  - Las versiones actualizadas del código fuente están en 
    * DELL y COMPOSTELA2.
    * Compilar en COMPOSTELA2 y copiar los BUILD* y SEARCH* a compostela para los test.	

------------------------------------------------
Versións Abril 2015.
Repair Gonzalo --> compilado irepair_bal irepair_nobal
Non hai cambios
------------------------------------------------

------------------------------------------------
Versións Marzo 2015.  <Repair.skip.moffat y Repair.skip.Sanders>
------------------------------------------------
	5.integradoSkippingBB.Sanders.2015[ver.leeme]
	 - Partiendo de esta versión - skipping- : 3.integradoSkippingBB.ClaudeSolucion2013
	Está adaptado para que se use Sampling tipo Sanders, con skipping en Repair.

	Las opciones de búsqueda son: 
		- "int2=lookup; intn=lookup"  --> búsqueda tipo lookup, usando skip info.
		- "int2=merge; intn=lookup"	  --> también sirve, pero el merge es tipo 
                                           extract+intersect y más lento ;)
	  **PROBLEMA BÁSICO encontrado ... en el RepairPost::createAux había overflows en fich
          grandes debido a que SAMPLEPERIOD(val, k, B, len, maxvalue)  y  CEILLOG_2(x,v) 
          tenían overflows debido a usar ints en lugar longs en las operaciones intermedias.
          (solucionado 2015.03.11).


	4.integradoSkippingBB.Moffat.2015[ver.leeme]
	 - Partiendo de esta versión - skipping- : 3.integradoSkippingBB.ClaudeSolucion2013
	Está adaptado para que se use Sampling tipo Moffat, con skipping en Repair.

	Las opciones de búsqueda son: 
		- "int2=skipE; intn=skipE"  --> búsqueda exponencial en Samples
		- "int2=skipS; intn=skipS"  --> búsqueda secuencial en Samples
		- "int2=skip; intn=skip"   -- igual que previamente (skipping sin samples)

  
	** El parámetro de construcción "samplek" (o B en sanders) se le pasa al search_il/
	o search_re a través del fichero idx.constants.samplek, que contiene una línea 
    "sampleK=valor". Y el RepairPost llama a la función read_sampleK para ver con qué 
	sampling queremos usar la búsqueda. **Así sólo es necesario indexar con Repair 1 vez**
    (la estructura de sampling se construye en tiempo de carga en el buscador).


------------------------------------------------
Versións Maio 2014 <cambios mínimos>
------------------------------------------------
	2.integradoBB.ClaudeSolucion2013 ... é o mesmo de 2013
	3.integradoSkippingBB.ClaudeSolucion2013_v2_shortfiles.normal  
       ** metín x= read()... para que non proteste o compilador... o resto é o mesmo de 2013.

------------------------------------------------
Versións Agosto 2013.
------------------------------------------------
	2.integradoBB.ClaudeSolucion2013
	3.integradoSkippingBB.ClaudeSolucion2013_v1
	
	Funcionan Integrado
	Admiten chamalos con 2 parámetros:
	[a]	"path2irepairgonzalo=srcREPAIR/repairGonzalo.srcs/irepair; 
		 path2build_index=./srcREPAIR/build_index_gonzalo"
    [b] "path2build_index=./srcREPAIR/build_index"

	En caso [a] (se se especifica "path2irepairgonzalo") --> chama a "irepair" para comprimir as 
    listas co repair-exacto de gonzalo. Neste caso debense incluir os executables:
	"build_index_gonzalo" e "irepair", encargados de comprimir con repair.
 
	Se non se especifica  "path2irepairgonzalo" (caso [b]) débese pasar a ruta do 
	"build_index", que fará a compresión das listas co Repair aproximado de Francisco Claude.

	*Exes utilizados:
		-convert, build_bitmap, build_index, build_index_gonzalo, irepair, compressDictionary

------------------------------------------------
Versións Maio 2011.
------------------------------------------------
2.integradoBB.ClaudeSolucion
3.integradoSkippingBB.ClaudeSolucion
	
	Eran as usadas para os experimentos de CIKM 2011. 
	Para a construcción usábase o parámetro "path2build_index=./srcREPAIR/build_index"

	*Exes utilizados (sacados do directorio 2.integradoBB.ClaudeSolucion/srcREPAIR):
		-convert, build_bitmap, build_index, build_index_gonzalo, irepair, compressDictionary

	* No repair había un "bug", pois non permitía especificar un "corte_repair < 0.00005", o
	que podía levar a que Repair non fixese todos os cambios posibles:
		- Isto era malo para o 2.Repair_sen_skipping.
		- Beneficioso para o 3.Repair_con_skipping (pois non metía tantísimas reglas).


------------------------------------------------
Versións Maio 2011 (antes de que Claude permitise comprimir ficheiros grandes)
------------------------------------------------
	2.integradoBB.Fallaba
	3.integradoSkippingBB.Fallaba

	Con problemas ao tratar de comprimir ficheiros grandes (??), que Claude solucionou (ver arriba)

------------------------------------------------
Versións Maio 2011 (sen funcionar correctamente ??)
------------------------------------------------
4.integradoskipSampling[Moffat]


------------------------------------------------
Versións Prev Maio 2011 (preliminares)
------------------------------------------------
	**prev**
	Conten versións previas (a que se sumase 1 para o desplazamento dos IDs [0..n-1] ==> [1..n]

