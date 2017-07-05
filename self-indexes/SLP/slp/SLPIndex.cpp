/* SLPIndex.cpp
 * Copyright (C) 2012, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Main program for indexing and querying biological collections
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

#include "RePairSLPIndex.h"
#include "Utils/offsets.h"

#define GET_TIME_DIVIDER ((double) 1.0 )	 //getTime deals with "seconds" as time unit.
#define GET_TIME_UNIT "sec"
#define MSEC_TIME_DIVIDER  ((double) 1000.0) //1 sec = 1000 msec
#define MSEC_TIME_UNIT 	  "msec"
#define SEC_TIME_DIVIDER  ((double) 1.0	)	 //1 sec = 1 sec
#define SEC_TIME_UNIT 	  "sec"
#define MCSEC_TIME_DIVIDER  ((double)1000000)	//1 microsec = 0.000001 sec
#define MCSEC_TIME_UNIT 	  "microsec"

#define QGRAM_DEF 8

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

/* Open patterns file and read header */
void pfile_info(unsigned int* length, unsigned int* numpatt){
	int error;
	unsigned char c;
	unsigned char origfilename[257];

	error = fscanf(stdin, "# number=%u length=%u file=%s forbidden=", numpatt, length, origfilename);
	if (error != 3)	{
		fprintf (stderr, "Error: Patterns file header not correct\n");
		perror ("run_queries");
		exit (1);
	}
	//fprintf (stderr, "# patterns = %lu length = %lu file = %s forbidden chars = ", *numpatt, *length, origfilename);
	while ( (c = fgetc(stdin)) != 0) {
		if (c == '\n') break;
		//fprintf (stderr, "%d",c);
	}
	//fprintf(stderr, "\n");
}

void
usage()
{
	cerr << endl;
	cerr << "*** INDEXING ***" << endl;
	cerr << "./SLPIndex -i <source_file> <dest> <q>" << endl;
	cerr << "=================================================" << endl;
	cerr << "It indexes the repetitive collection stored in 'source_file'." << endl;
	cerr << "The resulting index comprises two files: " << endl;
	cerr << "  - <dest>.ind: stores the SLP index." << endl;
	cerr << "  - <dest>.samp: stores the samples structure of size <q>." << endl << endl;
	cerr << "=================================================" << endl << endl;

	cerr << "*** LOCATING ***" << endl;
	cerr << "./SLPIndex -l <index>" << endl;
	cerr << "=================================================" << endl;
	cerr << "It reads, one to one, the provided patterns and locates of all their" << endl;
	cerr << " occurrences in the collection indexed in <dest>." << endl;
	cerr << "=================================================" << endl << endl;

	cerr << "*** EXTRACTING ***" << endl;
	cerr << "./SLPIndex -e <dest> <number_of_ranges>" << endl;
	cerr << "=================================================" << endl;
	cerr << "It reads, one to one, the provided ranges p1,p2 and extracts the " << endl;
	cerr << " sequences comprised in them within the collection indexed in <dest>." << endl;
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
				if (argc == 5)
				{
					double time = getTime ();

					ifstream in(argv[2], ifstream::in);
		
					in.seekg(0,ios_base::end);
					uint tsize = in.tellg();
					in.seekg(0,ios_base::beg);

					if (in.good())
					{
						// Reading the input text
						uchar *text = loadValue<uchar>(in, tsize);
						in.close();					

						cout << endl;
						cout << " >> Building the SLP Index for '" << argv[2] << "':" << endl;
						cout << "    # Input file size: " << tsize << " bytes" << endl;
				
						int qgram = atoi(argv[4]);
						if (qgram < 0) qgram = QGRAM_DEF;
						cout << "    # " << qgram << "-grams will be additionally indexed in the grammar" << endl;

						// New RePairSLPIndex
						RePairSLPIndex *indexer = new RePairSLPIndex();
						indexer->build(text, tsize, qgram, argv[3]);
						indexer->save();

						double tot_time = (getTime () - time);

						cout << "    # >> '" << argv[3] << "' indexed in " << (tot_time*SEC_TIME_DIVIDER) << " " << SEC_TIME_UNIT << endl;
						delete indexer;
					}
					else cerr << endl << "Please, check your input file" << endl << endl;
				}
				else usage();

				break;
			}

			case 'l':
			{
				if (argc == 3)
				{

				

					// LOADING THE SLP INDEX
					RePairSLPIndex *index;
					int q = RePairSLPIndex::load(argv[2], &index);

					if (q != 0)
					{

						unsigned int length=0;
						unsigned int numpatt=0;

						pfile_info(&length, &numpatt);

						// LOADING THE PATTERNS TO BE SEARCHED
						



						uchar *pattern = new uchar[length+1];
					    	pattern[length] = '\0';

						if (pattern == NULL){
							std::cerr<<"Error: cannot allocate pattern"<<std::endl;
							exit(EXIT_FAILURE);
						}

						while(numpatt){

							if(fread(pattern, sizeof(uchar), length, stdin) != length){
								fprintf(stderr, "Error: cannot read patterns file\n");
								exit(EXIT_FAILURE);
							}

							pattern[length] = '\0';
							string ss = string((char*)pattern);

//							cout << "(" << numpatt << ") searching " << ss << flush;
//							cout << "/STARTING SEARCH " << endl;

							{
								uint occs = 0;
								vector<uint> *pos = index->RePairSLPIndex::locate((uchar*)ss.c_str(), ss.length(), &occs);
								delete pos;

							}

							numpatt--;

						}

						delete [] pattern;
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
				if (argc == 4)
				{
					// LOADING THE SLP INDEX
					RePairSLPIndex *index;
					int q = RePairSLPIndex::load(argv[2], &index);

					if (q != 0)
					{
						// LOADING THE RANGES TO BE EXTRACTED
						unsigned int num_pos, numchars;
						unsigned char orig_file[257];
						int aux;

						aux = fscanf(stdin, "# number=%u length=%u file=%s\n", &num_pos, &numchars, orig_file);
						unsigned int tot_ext = num_pos*numchars;

						// PRINTF Index Size
						fprintf(stderr, "%u;;%u;;", index->size(), tot_ext);
						fflush(stderr);

						Range *ranges = new Range[num_pos];
						unsigned int from, to;

						for (uint i=0; i<num_pos; i++)
						{
							aux = fscanf(stdin,"%u,%u\n", &from, &to);
							ranges[i].left = from;
							ranges[i].right = to;
						}
	
						double aggregated_time = 0;

						for (unsigned int j=0; j<RUNS; j++)
						{
							double time = getTime ();

							for (uint i=0; i<num_pos; i++)
							{
								uchar *fragment = index->RePairSLPIndex::extract(ranges[i].left, ranges[i].right);
								delete [] fragment;
							}

							double tot_time = (getTime () - time);

							// PRINTF Run extract time
							fprintf(stderr, "%.2f;", tot_time);
							fflush(stderr);

							aggregated_time += tot_time;
							tot_time = 0;
							sleep(5);
						}

						// PRINTF Averaged extract time
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
