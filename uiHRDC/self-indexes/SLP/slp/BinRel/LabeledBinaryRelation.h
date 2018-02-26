/* LabeledBinaryRelation.h
 * Copyright (C) 2010, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Labeled Binary Relation based on:
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


#ifndef _LABELEDBINARYRELATION_H
#define _LABELEDBINARYRELATION_H

#include <algorithm>
#include <cstring>
#include "BinaryRelation.h"
#include "Occ.h"

using namespace cds_utils;
using namespace std;

namespace cds_static
{
	/** LabeledBinaryRelation
	 *   Extends the original binary relation to support labeled points
	 *
	 *   @author Miguel A. Martinez-Prieto
	 */
	class LabeledBinaryRelation : public BinaryRelation
	{	
		public:
			/** Generic constructor */
			LabeledBinaryRelation();
			/** Constructor for building (equivalent to BinaryRelation constructor) */
			LabeledBinaryRelation(vector<Rule> *matrix, uint npoints, uint nrows, uint ncolumns, bool contiguous);

			/** Build a labeled binary relation without bitsequences */
			void buildNoBitSeq(vector<Rule> *matrix, uint npoints, uint nrows, uint ncolumns, bool contiguous);

			/** Returns the structure size */
			size_t getSize();
			/** Loads a LabeledBinaryRelation */
			static LabeledBinaryRelation * load(ifstream & fp);
			/** Saves the LabeledBinaryRelation */
			void save(ofstream & fp);

			uint labelsPerColumn(uint column, vector<Occ> *labels, uint lenright, uint offset);
			uint labelsPerRow(uint row, vector<Occ> *labels, uint lenright, uint offset);

			// **************************
			// QUERY OPERATIONS ON LABELS
			// **************************
			/** L(a,b) returns the label for the point (a,b) */
			uint labelForPoint(uint row, uint column);

			/** Counts the ocurrences of a rule in the compressed sequence */
			uint nRowsPerColumnNoBitSeq(uint column);
			/** Retrieves the rule in a given position of the sequence */
			uint retrieveColumnPerLabelNoBitSeq(uint label);

			/** L(a) returns the label for the row 'a' in a relation without bitsequences */
			uint labelForRowNoBitSeq(uint row);
			/** L(a) returns the label for the i-th ocurrence of the column 'a' in a relation without bitsequences */
			uint labelForColumnNoBitSeq(uint column, uint i);


			/** |L(l)| returns the number of points labeled as 'l' */
			uint nPointsForLabel(uint label);
			/** L(l) returns the points for the label (l) */
			uint pointsForLabel(uint label, Point **points);
			/** L(l) returns the first point for the label (l) 
			    This is recommended for binary relations in where
			    unique labels are used. */
			Point firstPointForLabel(uint label);
			Point firstPointForLabelNoBitSeq(uint label);

			uint labelsInRange(RangeQuery rq, vector<Occ> *labels);
			uint labelsInRangeNoBitSeq(RangeQuery rq, vector<Occ> *labels);

			// *** Auxiliar operations on labels ***
			uint firstPointForLabel_X(uint label, uint *pos);
			uint firstPointForLabel_X(uint label, uint pos);
			uint firstPointForLabel_Y(uint label, uint *pos);
			uint firstPointForLabel_Y(uint label, uint pos);

			~LabeledBinaryRelation();

		protected:
			Sequence *Sl;

			uint Range(RangeQuery rq, uint t1, uint t2, int left, int right, uint h, vector<Occ> *labels);
	};
};
#endif  /* _LABELEDBINARYRELATION_H */
