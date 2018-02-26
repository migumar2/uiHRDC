/* wSLPIndex.cpp
 * Copyright (C) 2012, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Main program for indexing and querying highly repetitive collections
 * compressed with the wSLP (Word-oriented SLP).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the author:
 *   Miguel A Martinez-Prieto:  migumar2@infor.uva.es
 */

#include <fstream> 
#include <iostream>
#include <string>
#include <sys/stat.h> 
#include <sys/time.h>
#include <sys/types.h> 

#include "wRePairSLPIndex.h"

#define GET_TIME_DIVIDER ((double) 1.0 )	 //getTime deals with "seconds" as time unit.
#define GET_TIME_UNIT "sec"
#define MSEC_TIME_DIVIDER  ((double) 1000.0) //1 sec = 1000 msec
#define MSEC_TIME_UNIT 	  "msec"
#define SEC_TIME_DIVIDER  ((double) 1.0	)	 //1 sec = 1 sec
#define SEC_TIME_UNIT 	  "sec"
#define MCSEC_TIME_DIVIDER  ((double)1000000)	//1 microsec = 0.000001 sec
#define MCSEC_TIME_UNIT 	  "microsec"


typedef struct
{ string pattern;
  uint length;
} Pattern;
typedef struct
{ uint left;
  uint right;
} Range;

double
getTime(void)
{
	struct rusage buffer;
	struct timeval tp;
	struct timezone tzp;

	getrusage( RUSAGE_SELF, &buffer );
	gettimeofday( &tp, &tzp );

	return (double) buffer.ru_utime.tv_sec + 1.0e-6 * buffer.ru_utime.tv_usec;
}

using namespace cds_static;
using namespace std;

void
usage()
{
	cerr << endl;
	cerr << "*** INDEXING ***" << endl;
	cerr << "./wSLPIndex -i <source_file> <dest> " << endl;
	cerr << "=================================================" << endl;
	cerr << "It indexes the repetitive collection from 'source_file'" << endl;
	cerr << " and stores it 'dest'." << endl << endl;
	cerr << " NOTE: the source directory must contains an additional file " << endl;
	cerr << "  which stores initial position for each document in the collection." << endl;
	cerr << "  This file must be called as the 'source_file' with extension '.DOCBOUNDARIES'." << endl;
	cerr << "=================================================" << endl << endl;

	cerr << "*** LOCATING ***" << endl;
	cerr << "./wSLPIndex -l <dest> <number_of_patterns>" << endl;
	cerr << "=================================================" << endl;
	cerr << "It reads, one to one, the provided patterns and locates of all their " << endl;
	cerr << "occurrences in the collection indexed in <dest>." << endl;
	cerr << "=================================================" << endl << endl;

	cerr << "*** EXTRACTING ***" << endl;
	//cerr << "./wSLPIndex -e <dest> <number_of_ranges>" << endl;
	cerr << "./wSLPIndex -e <dest> " << endl;//@@fari@@
	cerr << "=================================================" << endl;
	cerr << "It reads, one to one, the provided ranges (p1,p2) and extracts the " << endl;
	cerr << "sequences comprised in them within the collection indexed in <dest>." << endl;
	cerr << "=================================================" << endl;
	cerr << endl;
}
 
int 
main(int argc, char* argv[])
{	
	unsigned int RUNS = 10;

	if ((argc > 1) && (argv[1][0] == '-'))
	{
		char mode = argv[1][1];

		switch(mode)
		{
			case 'i':
			{
				if (argc == 4)
				{
					double time = getTime ();

					ifstream in(argv[2], ifstream::in);

					in.seekg(0,ios_base::end);
					uint tsize = in.tellg();
					in.seekg(0,ios_base::beg);

					ifstream docs((string(argv[2])+".DOCBOUNDARIES").c_str());
					docs.seekg(0,ios_base::end);
					uint ndocs = docs.tellg()/sizeof(int);
					docs.seekg(0,ios_base::beg);

					if (in.good() && docs.good())
					{
						// Reading the input text
						uchar *text = loadValue<uchar>(in, tsize);
						in.close();

						// Reading the doc boundaries
						uint *doc_array = loadValue<uint>(docs, ndocs);
						docs.close();

						cout << endl;
						cout << " >> Building the wSLP Index for '" << argv[2] << "' (" << ndocs << ") documents:" << endl;
						cout << "    # Input file size: " << tsize << " bytes" << endl;

						// New RePairSLPIndex
						wRePairSLPIndex *indexer = new wRePairSLPIndex();
						indexer->build(text, tsize, doc_array, ndocs, (char*)argv[3]);
						delete [] doc_array;
						indexer->save();

						double tot_time = (getTime () - time);
						
						cout << "    # >> '" << argv[3] << "' indexed in " << (tot_time*SEC_TIME_DIVIDER) << " " << SEC_TIME_UNIT << endl;
						delete indexer;

					}
 					else cerr << endl << "Please, check your input files" << endl << endl;
				}
				else usage();

				break;
			}

			case 'l':
			{
				if (argc == 4)
				{
					// LOADING THE SLP INDEX
					wRePairSLPIndex *index;
					int q = wRePairSLPIndex::load(argv[2], &index);

					if (q)
					{
						// LOADING THE PATTERNS TO BE SEARCHED
						int npatterns = atoi(argv[3]);

						Pattern *patterns = new Pattern[npatterns];
						int aux;

						unsigned int ordinal=0;
    						unsigned char* pattern[1000];
						string header;
						getline(cin, header);

						for (int i=0; i<npatterns; i++)
						{
							aux = fscanf(stdin, "%u %u ", &ordinal, &(patterns[i].length));
							aux = fread(pattern, sizeof(uchar), patterns[i].length, stdin);

							pattern[patterns[i].length] = 0;
							patterns[i].pattern = string((char*)pattern);
						}

						fprintf(stderr, "%u;;", index->size());
						
						double aggregated_time = 0;
						uint *docs;

						for (uint j=0; j<RUNS; j++)
						{ 
							/* @@ */ fprintf(stdout,"Run %d of %d (%d patterns each)\r",j,RUNS,npatterns); fflush(stdout);
							uint tot_occs = 0;
							double time = getTime ();

							for (int i=0; i<npatterns; i++)
							{   
						//	/* @@ */ fprintf(stdout,"pattern %d of %d\r",i,npatterns); fflush(stdout);

						//		/* @@ */ fprintf(stdout,"pattern %d = %s, len = %d",i, (uchar*)(patterns[i].pattern).c_str(), patterns[i].length); fflush(stdout);
								uint occs = 0;
								index->wRePairSLPIndex::locate((uchar*)(patterns[i].pattern).c_str(), patterns[i].length, &docs, &occs);
						//		/* @@ */ fprintf(stdout, "%u;;", occs); fflush(stdout);  
								
								tot_occs += occs;
		
								if (docs) delete [] docs;
							}

							double tot_time = (getTime () - time);

							// PRINTF Occurrences
							if (j==0) fprintf(stderr, "%u;;", tot_occs);

							// PRINTF Run locate time
							fprintf(stderr, "%.2f;", tot_time);
							fflush(stderr);

							aggregated_time += tot_time;
							sleep(1);
						}
						fprintf(stdout,"\n finished!\n");

						// PRINTF Averaged locate time
						fprintf(stderr, ";%.2f \n", aggregated_time/RUNS);
						fflush(stderr);

						delete [] patterns;
						delete index;
					}
					else
					{
						cerr << "    # CHECK INDEX FILES!!" << endl;
					}
				}
				else usage();

				break;
			}

			case 'e':
			{
				if (argc == 3)    //@@fari@@
				{
					// LOADING THE SLP INDEX
					wRePairSLPIndex *index;
					int q = wRePairSLPIndex::load(argv[2], &index);

					if (q)
					{
						// LOADING THE RANGES TO BE EXTRACTED
						unsigned int num_pos, numwords;
						unsigned char orig_file[257];
						int aux;

						aux = fscanf(stdin, "# number=%u numwords=%u file=%s\n", &num_pos, &numwords, orig_file);
						unsigned int tot_ext = num_pos*numwords;

						// PRINTF Index Size
						//fprintf(stderr, "%u;;%u;;", index->size(), tot_ext);
						fflush(stderr);

						Range *ranges = new Range[num_pos];
						unsigned int from, to;

						for (uint i=0; i<num_pos; i++)
						{
							aux = fscanf(stdin,"%u,%u\n", &from, &to);
							ranges[i].left = from;
							ranges[i].right = to;
						}
	

						double timeFARI = getTime ();
					

						
						double aggregated_time = 0;
//						for (unsigned int j=0; j<RUNS*100; j++)    //@@fari@@ INITIALLY J<RUNS, 2018 J<RUN*100
						for (unsigned int j=0; j<RUNS; j++)    //@@fari@@ INITIALLY J<RUNS, 2018 J<RUN*100
						{
						//	/* @@ */ fprintf(stdout,"Run %d of %d (%d SNIPPETS each)\r",j,RUNS,num_pos); fflush(stdout);
							
							double time = getTime ();
							uint chars = 0;
							uint length = 0;

							for (uint i=0; i<num_pos; i++)
							{
								uchar *fragment;
								length = index->wRePairSLPIndex::extract(ranges[i].left, ranges[i].right, &fragment);
								free(fragment);

								chars += length;
							}

							double tot_time = (getTime () - time);


							if (j==0) {
								fprintf(stderr, "%u;;%u;;", index->size(), chars);								
							}
							
							aggregated_time += tot_time;
							// PRINTF Run extract time
							if (j<RUNS)
								fprintf(stderr, "%.2f;", tot_time); fflush(stderr);

							tot_time = 0;
							//sleep(1);
						}
						fprintf(stdout,"\n finished!\n");
						// PRINTF Averaged extract time
						
						timeFARI= getTime() - timeFARI;
						//aggregated_time = timeFARI;    //@fari 2018 @//
						
						fprintf(stderr, ";%.2f \n", aggregated_time/RUNS);
						fflush(stderr);

						delete [] ranges;
						delete index;
					}
					else
					{
						cerr << "    # CHECK INDEX FILES!!" << endl;
					}
				}
				else usage();

				break;
			}


			default:
				usage();
		}
	}
	else
	{
		usage();
	}
}
