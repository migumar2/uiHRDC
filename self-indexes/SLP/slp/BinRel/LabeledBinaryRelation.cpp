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

#include "LabeledBinaryRelation.h"

namespace cds_static
{
	LabeledBinaryRelation::LabeledBinaryRelation()
	{
		mapperB = NULL;
	}

	void
	LabeledBinaryRelation::buildNoBitSeq(vector<Rule> *matrix, uint npoints, uint nrows, uint ncolumns, bool contiguous)
	{
		this->type = LBRNB;
		
		this->npoints = npoints;		
		this->nrows = nrows;
		this->ncolumns = ncolumns;

		// Sorting points by column
		sort(matrix->begin(), matrix->end(), Point::sortPointsByRow);

		uint *columns = new uint[this->npoints];
		uint *labels = new uint[this->npoints];
		
		// Filling column and label vectors
		for (uint i=0; i<nrows-1; i++)
		{			
			columns[i] = (*matrix)[i].right;
			labels[i] = (*matrix)[i].id;
		}

		// Building the sequence for columns
		if (contiguous) mapperB = new MapperNone();
		else mapperB = new MapperCont(columns, npoints, BitSequenceBuilderRG(20));
		mapperB->use();
		Sb = new WaveletTreeNoptrs(columns, npoints, new BitSequenceBuilder375(), mapperB);
		
		// Building the sequence for labels
		Sl = new SequenceGMRChunk(labels, npoints, new BitSequenceBuilderRG(20), new PermutationBuilderMRRR(4, new BitSequenceBuilderRG(20)));

		delete[] columns;
		delete[] labels;
	}

	LabeledBinaryRelation::LabeledBinaryRelation(vector<Rule> *matrix, uint npoints, uint nrows, uint ncolumns, bool contiguous)
	{
		this->type = LBR;

		this->npoints = npoints;		
		this->nrows = nrows;
		this->ncolumns = ncolumns;
		
		// Sorting points by column
		sort(matrix->begin()+1, matrix->end(), Point::sortPointsByColumn);

		vector<uint> elemsByRows(this->nrows+1,0);
		vector<uint> ptrsToRows(this->nrows+1,0);
		vector<uint> elemsByColumns(this->ncolumns+1,0);		
		uint jump = nrows-npoints;
		
		// Gathering row and column cardinalities
		for (uint i=jump+1; i<=nrows; i++)
		{
			elemsByRows[(*matrix)[i].left]++;
			elemsByColumns[(*matrix)[i].right]++;
		}
		
		// Storing pointers to row lists in columns and labels
		for (uint i=2; i<=nrows; i++)
			ptrsToRows[i] = ptrsToRows[i-1]+elemsByRows[i-1];
			
		uint *columns = new uint[this->npoints];
		uint *labels = new uint[this->npoints];
			
		// Filling column and label vectors
		for (uint i=jump+1; i<=nrows; i++)
		{
			columns[ptrsToRows[(*matrix)[i].left]] = (*matrix)[i].right;
			labels[ptrsToRows[(*matrix)[i].left]] = (*matrix)[i].id;
			ptrsToRows[(*matrix)[i].left]++;
		}		
		
		// Obtaininig bitstrings for rows and columns
		BitString *bsa = new BitString(this->npoints+this->nrows+1);
		buildBitString(&bsa, &elemsByRows, this->nrows+1);

		BitString *bsb = new BitString(this->npoints+this->ncolumns+1);
		buildBitString(&bsb, &elemsByColumns, this->ncolumns+1);
		
		// Building bitsequences
		Xa = new BitSequenceRG(*bsa, 20);
		Xb = new BitSequenceRG(*bsb, 20);
		delete bsa; delete bsb;

		// Building the sequence for columns
		if (contiguous) mapperB = new MapperNone();
		else mapperB = new MapperCont(columns, npoints, BitSequenceBuilderRG(20));
		mapperB->use();
		Sb = new WaveletTreeNoptrs(columns, npoints, new BitSequenceBuilder375(), mapperB);

		// Building the sequence for labels
		Sl = new SequenceGMRChunk(labels, npoints, new BitSequenceBuilderRG(20), new PermutationBuilderMRRR(4, new BitSequenceBuilderRG(20)));
		
		delete[] columns;
		delete[] labels;
	}

	LabeledBinaryRelation* 
	LabeledBinaryRelation::load(ifstream & fp)
	{
		// Binary Relation Type
		uint type = loadValue<uint>(fp);
		if ((type != LBR) && (type != LBRNB)) return NULL;
		
		LabeledBinaryRelation *br = new LabeledBinaryRelation();
		br->type = type;
		
		// Loading stats
		br->npoints = loadValue<uint>(fp);
		br->nrows = loadValue<uint>(fp);
		br->ncolumns = loadValue<uint>(fp);
		
		// Loading Structures (Xa, Xb, Sb, Sl)
		if (type == LBR)
		{
			br->Xa = BitSequence::load(fp);
			br->Xb = BitSequence::load(fp);

			BitSequence *XXa = new BitSequenceRGK(((BitSequenceRG*)br->Xa)->data, ((BitSequenceRG*)br->Xa)->n, 20, 32);
			delete br->Xa;
			br->Xa = XXa;

			BitSequence *XXb = new BitSequenceRGK(((BitSequenceRG*)br->Xb)->data, ((BitSequenceRG*)br->Xb)->n, 20, 32);
			delete br->Xb;
			br->Xb = XXb;
		}

		br->Sb = Sequence::load(fp);
		br->Sl = Sequence::load(fp);

		{
			uint elements = ((SequenceGMRChunk*)(br->Sl))->getN();
			uint *symbols = new uint[elements];

			for (uint i=0; i<elements; i++) symbols[i] = (br->Sl)->access(i);

			SequenceGMRChunk *SSl = new SequenceGMRChunk(symbols, elements, new BitSequenceBuilderRGK(20,32), new PermutationBuilderMRRR(4, new BitSequenceBuilderRG(20)));
			delete [] symbols;

			delete br->Sl;
			br->Sl = SSl;
		}
		
		// Additional Values (for range queries)
		br->sigma = ((WaveletTreeNoptrs*)br->Sb)->getMaxV();
		br->height = bits(br->sigma);

		return br;
	}

	void 
	LabeledBinaryRelation::save(ofstream & fp)
	{
		BinaryRelation::save(fp);
		Sl->save(fp);
	}	
	
	size_t 
	LabeledBinaryRelation::getSize()
	{
		return BinaryRelation::getSize()+Sl->getSize();
	}
	
	uint 
	LabeledBinaryRelation::labelsPerColumn(uint column, vector<Occ> *labels, uint lenright, uint offset)
	{
		uint elements = nRowsPerColumn(column);
		
		if (elements > 0)
		{
			Occ xocc;
			for (uint i=0; i<elements; i++)
			{
				uint pos = Sb->select(column, i+1);
				
				xocc.id = Sl->access(pos);
				xocc.right = -lenright;
				xocc.len = offset;
				labels->push_back(xocc);
			}
		}
		
		return elements;
	}
	
	uint 
	LabeledBinaryRelation::labelsPerRow(uint row, vector<Occ> *labels, uint lenright, uint offset)
	{
		int pos = Xa->select1(row);

		int begin = pos-row+1;
		int end = Xa->selectNext1(pos+1)-(row+1);
		uint elements = end-begin+1;
		
		if (elements > 0)
		{
			Occ xocc;
			for (uint i=0; i<elements; i++)
			{
				xocc.id = Sl->access(begin+i);
				xocc.right = lenright;
				xocc.len = offset;
				labels->push_back(xocc);
			}
		}
		
		return elements;
	}
	
	uint 
	LabeledBinaryRelation::labelForPoint(uint row, uint column)
	{
		// Check the existence of the point (row, column)
		int pos = Xa->select1(row);
		uint bRow = pos-row;
		uint eRow = Xa->selectNext1(pos+1)-(row+1);		
		
		uint occsBColumn = Sb->rank(column, bRow);
		uint occsEColumn = Sb->rank(column, eRow);
			
		if (occsEColumn-occsBColumn > 0) return Sl->access(Sb->select(column, occsEColumn));
		else return NOTFOUND;
	}

	uint 
	LabeledBinaryRelation::labelForRowNoBitSeq(uint row)
	{
		return Sl->access(row-1);
	}

	uint 
	LabeledBinaryRelation::labelForColumnNoBitSeq(uint column, uint i)
	{
		uint pos = Sb->select(column, i);
		return Sl->access(pos);
	}

	uint
	LabeledBinaryRelation::nRowsPerColumnNoBitSeq(uint column)
	{
		if ((((WaveletTreeNoptrs*)Sb)->check(column)) == true) 
		{
			return Sb->rank(column, nrows-1);
		}

		return 0;
	}

	uint 
	LabeledBinaryRelation::retrieveColumnPerLabelNoBitSeq(uint label)
	{
		uint pos = Sl->select(label, 1);
		return Sb->access(pos);
	}

	uint 
	LabeledBinaryRelation::nPointsForLabel(uint label)
	{
		return Sl->rank(label, npoints-1);
	}
	
	uint 
	LabeledBinaryRelation::pointsForLabel(uint label, Point **points)
	{
		uint elements = nPointsForLabel(label);
		
		if (elements > 0)
		{
			Point *results = new Point[elements];	
			for (uint i=1; i<=elements; i++)
			{
				uint pos = Sl->select(label, i);				
				uint x = Xa->select0(pos+1) - pos;
				uint y = Sb->access(pos);				
				
				results[i-1] = Point(x, y);
			}
			*points = results;
		}
		else
		{
			*points = NULL;
		}
				
		return elements;
	}
	
	Point
	LabeledBinaryRelation::firstPointForLabel(uint label)
	{
		uint pos = Sl->select(label, 1);
		uint x = Xa->select0(pos+1) - pos;
		uint y = Sb->access(pos);		
		
		return Point(x,y);
	}
	
	Point
	LabeledBinaryRelation::firstPointForLabelNoBitSeq(uint label)
	{
		uint pos = Sl->select(label, 1);
		uint y = Sb->access(pos);		
		
		return Point(pos+1,y);
	}
	
	uint
	LabeledBinaryRelation::firstPointForLabel_X(uint label, uint *pos)
	{
		*pos = Sl->select(label, 1);
		return Xa->select0(*pos+1) - *pos;
	}
	
	uint
	LabeledBinaryRelation::firstPointForLabel_X(uint label, uint pos)
	{
		return Xa->select0(pos+1) - pos;
	}
	
	uint
	LabeledBinaryRelation::firstPointForLabel_Y(uint label, uint *pos)
	{
		*pos = Sl->select(label, 1);
		return Sb->access(*pos);	
	}
	
	uint
	LabeledBinaryRelation::firstPointForLabel_Y(uint label, uint pos)
	{
		return Sb->access(pos);		
	}
	
	uint
	LabeledBinaryRelation::labelsInRange(RangeQuery rq, vector<Occ> *labels)
	{				
		RangeQuery trq;

		// The WT is stored in [0,n-1], but the algorithm perform on
		// [0,n], so row values are incremented in 1
		trq.rows.l = 1+(Xa->select1(rq.rows.l)-rq.rows.l+1);
		trq.rows.r = 1+(Xa->select1(rq.rows.r+1)-(rq.rows.r+1));
		trq.results = 0;
		
		if (trq.rows.r >= trq.rows.l)
		{				
			// Mapping values
			trq.columns.l = ((WaveletTreeNoptrs*)Sb)->map(rq.columns.l);
			if ((((WaveletTreeNoptrs*)Sb)->check(rq.columns.l)) == false) trq.columns.l++;
			trq.columns.r = ((WaveletTreeNoptrs*)Sb)->map(rq.columns.r);

			if (trq.columns.r >= trq.columns.l)
				return Range(trq, 0, sigma, 0, npoints, 0, labels);
			else
				return 0;
		}
		else
		{
			return 0;
		}
	}
	
	uint
	LabeledBinaryRelation::Range(RangeQuery trq, uint t1, uint t2, int left, int right, uint h, vector<Occ> *labels)
	{							
		if ((trq.rows.l > trq.rows.r) || (t1 > trq.columns.r) || (t2 < trq.columns.l))
		{
			return 0;
		}
		else
		{	
			if ((t1 >= trq.columns.l) && (t2 <= trq.columns.r) && (t1 == t2))
			{			
				uint tt1 = ((WaveletTreeNoptrs*)Sb)->unmap(t1);
				uint elements = trq.rows.r-trq.rows.l+1;

				Occ xocc;
				
				for (uint i=trq.rows.l; i<=trq.rows.r; i++)
				{
					xocc.id = Sl->access(Sb->select(tt1, i));
					xocc.right = tt1;
					labels->push_back(xocc);
				}
				
				return elements;
			}
			else
			{			
				RangeQuery trq2 = {{trq.rows.l, trq.rows.r}, {trq.columns.l, trq.columns.r}};

				uint tm = (pow((float)2,(int)(height-h)))/2;
				int x = ((WaveletTreeNoptrs*)Sb)->rangeQuery(h, &trq2.rows.l, &trq2.rows.r, left, right);			
				
				uint nleft = Range(trq2, t1, t1+tm-1, left, left+x, h+1, labels);

				trq2.rows.l = trq.rows.l-trq2.rows.l+1;
				trq2.rows.r = trq.rows.r-trq2.rows.r;
				uint nright = Range(trq2, t1+tm, t2, left+x+1, right, h+1, labels);
				
				return nleft + nright;
			}
		}
	}

	uint 
	LabeledBinaryRelation::labelsInRangeNoBitSeq(RangeQuery rq, vector<Occ> *labels)
	{
		RangeQuery trq;

		// Note that the binary relation for the sequence is inverted
		// with respect to the original approach: suffixes are in rows and
		// prefixes in columns.
		trq.rows.l = rq.columns.l;
		trq.rows.r = rq.columns.r;
		trq.results = 0;
		
		if (trq.rows.r >= trq.rows.l)
		{	
			if (rq.rows.l <= ((WaveletTreeNoptrs*)Sb)->getMaxMappedV())
			{
				// Mapping values
				trq.columns.l = ((WaveletTreeNoptrs*)Sb)->map(rq.rows.l);
				if ((((WaveletTreeNoptrs*)Sb)->check(rq.rows.l)) == false) trq.columns.l++;
				trq.columns.r = ((WaveletTreeNoptrs*)Sb)->map(rq.rows.r);

				if (trq.columns.r >= trq.columns.l)
					return Range(trq, 0, sigma, 0, npoints, 0, labels);
				else
					return 0;
			} else  {
				return 0;
			}
		}
		else
		{
			return 0;
		}

		return 0;
	}

	LabeledBinaryRelation::~LabeledBinaryRelation()
	{
		delete ((SequenceGMRChunk*)Sl);
	}
};

