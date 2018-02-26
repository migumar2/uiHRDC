#include <iostream>
#include <cassert>
#include "basic.h"
#include "bitrankw32int.h"
#include "RePair.h"
using namespace std;

#define uint unsigned int

#include "RepairGonzaloFrontend.c"




int main(int argc, char ** argv) {	  
  if(argc!=6) {
    cout << "Usage: " << argv[0] << " <GRAPH> <MEM> <K> <CORTE_REPAIR> <path2iRepair>" << endl;
    cout << "..... Mem and K are not actually used" << endl;
    return 0;
  }
 
  
  char * fname = new char[strlen(argv[1])+20];
  FILE * fp = fopen64(argv[1],"r");       // **   INPUT FILE FORMAT...
  assert(fp!=NULL);                       // **   #nodes #edges nodeId ed ed ed ed nodeId ed ed ...  
                                          // **     5000  23400     -1  2  3  4 76     -2 23 34 ... 
  uint mem = atoi(argv[2]);
  uint K = atoi(argv[3]);

  uint nodes; uint edges;
  size_t rf;
  rf= fread(&nodes,sizeof(uint),1,fp);
  printf("\n\n\n ***** nodos = %u ",nodes); //** reads the number of nodes
  rf= fread(&edges,sizeof(uint),1,fp);          //** reads the number of edges
  printf("\n\n\n *****ejes = %u\n ",edges);
fflush(stdout);

  mem = nodes + edges +1;   // *PUESTO QUE NO HAREMOS REPAIR, sino que se delega en el IRepair de Gonzalo, no se necesita buffer extra! 
  
	int * text = new int[mem];//(int*) calloc(mem,sizeof(uint));						  
	uint * offsets = new uint[nodes+1];        //**  ej real. offsets= [0][13][17][19][19][20][20][23][28][33]

  int min = 0; uint nodes_read=0;
  bool fst = false; int last=0;
  for(uint i=0;i<nodes+edges;i++) {
    int k;                             
    rf= fread(&k,sizeof(uint),1,fp);   // ** reads either [nodeId <0] or [edgeId>0].
    if(k<0) {   //** a node
      //bitset(bitmap,i);
      offsets[-k-1]=i+k+1;
      if(k<min) min=k;
      nodes_read++;
      text[i]=k;
      fst = true;
    }
    else {    //** an edge
    	
//    //**fari
//    	if (k==0) {printf("k es 0!! "); exit(0);}
//    //**fari	
    	
//      //**fari
//      k++;
//      //**fari
      	
      if(fst) {
        text[i]=k;
        last = k;      //** value of the first edge of the current node.
        fst = false;
      }
      else {
        text[i] = k-last;  //** computes dgaps from the fst value.
        last = k;
      }
    }
  }
  
  fclose(fp); ///**** fari 2013
  
  

/** FARI */
//printf("\n offsets:: "); fflush(stdout);
//for(uint i=0;i<10;i++) {
//	printf("[%d]::",offsets[i]);
//}
//printf("\n text:: ");
//for(uint i=0;i<33;i++) {
//	printf("[%d]::",text[i]);
//}
//
//  uint xx=0;
//// for(uint i=0;i<nodes+edges;i++) {
//  for(uint i=0;i<33;i++) {
//  	if ((offsets[xx]+xx) == i) {
//   		printf("\n node[%d]::",text[i]);xx++;
//   	}
//   	else {
//   		printf("<%d>",text[i]);
//   	}
//  }  exit(0);
/** FARI */

  min--;  //** min = "min k_i" - 1;    //** los nodos se quedan en el intervalo [0-->nodes]
  text[nodes+edges]=0;   //** virtual  //** los ejes toman los valores  [nodes+valor anterior en text[i]]  
  offsets[nodes]=edges;  //** virtual  //**                      ==> ahora text[i]>nodes.
  for(uint i=0;i<nodes+edges;i++)
    text[i] -= min;
  //bitset(bitmap,nodes+edges);


////** fari **/
//  uint xx=0;
//  uint minedge=99999999;
//  uint minnode=99999999;
//  int maxnode = -1;
//  int maxedge = -1;
//  
// for(uint i=0;i<nodes+edges;i++) {
////  for(uint i=0;i<33;i++) {
//  	if ((offsets[xx]+xx) == i) {
//   		printf("\n node[%d]::",text[i]);xx++;
//   		if (maxnode < text[i]) maxnode = text[i];
//   		if (text[i] < minnode ) minnode=text[i];
//   	}
//   	else {
//   		printf("<%d>",text[i]);
//   		if (text[i] < minedge ) minedge=text[i];
//   		if (maxedge < text[i]) maxedge = text[i];
//   	}
//  }
//   		printf("\n\n MinEdge value<%7d>,MaxEdge value<%7d>, edges value = %d",minedge,maxedge,edges);
//   		printf("\n\n MinNode value<%7d>,MaxNode value<%7d>, nodes value = %d",minnode,maxnode,nodes);
// 		printf("\n\n min value<%7d>",min);
//    


///** FARI */
  
  {  sprintf(fname,"%s.ptr.ints",argv[1]);   ////// Este fichero no se usa, pero es m�s facil ver el .ptr.ints que el .ptr ;)
  	FILE * f_out = fopen64(fname,"w");
  	size_t rf;
	rf= fwrite(offsets,sizeof(uint),nodes,f_out);
	fclose(f_out);
  }
  
  
  uint plen = bits(edges);    //** ptr a los lugares donde est�n los ejes para cada nodo. en el texto T, no en el comprimido (por ahora) **/
  uint offsets_len = (nodes+1)*plen/WW32+1;
  uint * off = new uint[offsets_len];   //** offsets codificados como "k-bit intergers"
  off[offsets_len-1]=0000; //* evita  que proteste valgrind*/
  for(uint i=0;i<=nodes;i++)
    SetField(off,plen,i,offsets[i]);
  delete [] offsets;
  offsets=off;
  
  cout << "Nodes: " << nodes << endl;
  cout << "Edges: " << edges << endl;
  cout << "Nodes read: " << nodes_read << endl;
  





  /***********************************************************************************************/ 
  /***********************************************************************************************/ 
  /**********************************************************************************/
  //see RepairGonzaloFrontend.c
  //   it calls Gonzalo's iRepair externally and rebuilds the files .rp and .dict as needed by 
  //   CompressDictionary program and RepairPost class
  
  double CORTE_REPAIR = atof(argv[4]);
  char *path2iRepair = argv[5]; // = "./repairGonzalo.srcs/irepair"
  
  printf("\n @@@@@@@@@@@@@CORTE REPAIR......... EN BUILD_INDEX_GONZALO = %3.15f\n", CORTE_REPAIR);
  
  gonzalosRepair(text, nodes,edges, fname, argv, path2iRepair, CORTE_REPAIR);
  /***********************************************************************************************/ 
  /***********************************************************************************************/ 

	/**old Francisco's call to Repair()
	{	sprintf(fname,"%s",argv[1]);
		RePair * rc = new RePair((uint*)text,(edges+nodes),mem,fname,
		rc->compress(nodes,CORTE_REPAIR);   //** nodes es el n�mero de nodos, y tambi�n indica que los ejes toman "valores > nodes"
		rc->save(nodes);
		delete rc;
	}
	**/



  sprintf(fname,"%s.ptr",argv[1]);
  FILE * f_out = fopen64(fname,"w");
  
  rf= fwrite(&min,sizeof(int),1,f_out);
  rf= fwrite(&nodes,sizeof(uint),1,f_out);
  rf= fwrite(&edges,sizeof(uint),1,f_out);
  rf= fwrite(offsets,sizeof(uint),offsets_len,f_out);   //codificados con K-bits (plen bits de hecho)
  fclose(f_out);


  delete [] fname;
  delete [] offsets;
  delete [] text;
  cout << "\n ================== build_index_gonzalo program ends ==================" << endl << endl;  
  
  return 0;
}


