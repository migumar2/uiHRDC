/* BinaryRelation.h
 * Copyright (C) 2010, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Compact Binary Relation based on:
 *    Francisco Claude and Gonzalo Navarro.
 *    Self-Indexed Grammar-Based Compression.
 *    Fundamenta Informaticae
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


#ifndef _BINARYRELATION_H
#define _BINARYRELATION_H

#include <libcdsBasics.h>
#include <BitString.h>
#include <BitSequenceBuilder.h>
#include <Mapper.h>
#include <WaveletTreeNoptrs.h>

#include "Point.h"
#include "UtilsBinRel.h"

using namespace cds_utils;

namespace cds_static
{
	static const uint BBR = 1;   // Boolean Binary Relation
	static const uint LBR = 2;   // Labeled Binary Relation
	static const uint LBRNB = 3; // Labeled Binary Relation without bitsequences

	static const uint NOTFOUND = 0;

	/** BinaryRelation
	 *   Class to implement the Compact Binary Relation approached by Claude&Navarro
	 *
	 *   @author Miguel A. Martinez-Prieto
	 */
	class BinaryRelation
	{	
		public:
			/** Generic constructor */
			BinaryRelation();
			/** Constructor for building 
			 *  @matrix: the matrix of points for the relation.
			 *  @npoints: number of points in the matrix.
			 *  @nrows: number of rows in the matrix.
			 *  @ncolumns: number of columns in the matrix.
			 *  @contiguous: is true if the matrix covers all values in ranges.
			 */
			BinaryRelation(vector<Point> *matrix, uint npoints, uint nrows, uint ncolumns, bool contiguous);

			/** Returns the structure size */
			size_t getSize();
			/** Loads a BinaryRelation */
			static BinaryRelation * load(ifstream & fp);
			/** Saves the BinaryRelation */
			void save(ofstream & fp);

			// ****************
			// QUERY OPERATIONS
			// ****************
			/** |A(b)| returns the number of rows related to the column 'b' */
			uint nRowsPerColumn(uint column);
			/** |B(a)| returns the number of columns related to the row 'a' */
			uint nColumnsPerRow(uint row);
			/** A(b) returns the list of row IDs related to the column 'b' */
			uint rowsPerColumn(uint column, uint **rows);
			/** B(a) returns the list of columns IDs related to the row 'a' */
			uint columnsPerRow(uint row, uint **columns);
			/** R(a1,a2,b1,b2) returns all points in range [a1,a2] x [b1,b2] */
			uint pointsInRange(uint a1, uint a2, uint b1, uint b2, Point **points);

			uint getNPoints();
			uint getNRows();
			uint getNColumns();

			~BinaryRelation();

		protected:
			BitSequence *Xa, *Xb;
			Sequence *Sb;

			Mapper *mapperB;

			// Binary relation type
			uint type;
			// Number of points in the relation
			uint npoints;
			// Number of rows in [0, nrows-1], but row 0 is never used.
			uint nrows;
			// Number of columns in [0, ncolumns-1], but columns 0 is never used.
			uint ncolumns;
			
			// Additional values
			uint sigma;
			uint height;

			/** Implements the Range Query Algorithm by Mäkinen and Navarro:
			      Veli Mäkinen and Gonzalo Navarro
			      Rank and Select Revisited and Extended
			      Theoretical Computer Science 387(3), 2007, 332-347
			    @i1, @i2, @j1, @j2, @t1, @t2: like in the paper
			    @left, @right: are the left/right limits of the range in the h-th bitsequence in Sb
			    @h: references the h-th bitsequence in Sb
 			    @points: stores the point results
			*/
			uint Range(uint i1, uint i2, uint j1, uint j2, uint t1, uint t2, int left, int right, uint h, Point **points);

			void buildBitString(BitString **bs, vector<uint> *v, uint elems);
	};
};

#include "LabeledBinaryRelation.h"

#endif  /* _BINARYRELATION_H */
