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

#include "BinaryRelation.h"

namespace cds_static
{
	BinaryRelation::BinaryRelation()
	{
		mapperB = NULL;
	}

	BinaryRelation::BinaryRelation(vector<Point> *matrix, uint npoints, uint nrows, uint ncolumns, bool contiguous)
	{
		string output;
		
		this->type = BBR;

		this->npoints = npoints;		
		this->nrows = nrows;
		this->ncolumns = ncolumns;
		
		vector<uint> elemsByRows(this->nrows+1,0);
		vector<uint> elemsByColumns(this->ncolumns+1,0);
		uint *columns = new uint[this->npoints];
		
		uint pos = 0, element = (*matrix)[0].left;

		// Gathering rows and columns statistics.
		// Building the vector of column IDs.
		for (uint i=0; i<this->npoints; i++)
		{
			if ((*matrix)[i].left != (int)element)
			{
				elemsByRows[element] = i-pos;
				
				pos = i;
				element = (*matrix)[i].left;
			}
			
			elemsByColumns[(*matrix)[i].right]++;
			columns[i] = (*matrix)[i].right;
		}
		
		// Statistics for the last row
		elemsByRows[element] = this->npoints-pos;
		
		// Obtaininig bitstring for rows and columns
		BitString *bsa = new BitString(this->npoints+this->nrows+1);
		buildBitString(&bsa, &elemsByRows, this->nrows+1);

		BitString *bsb = new BitString(this->npoints+this->ncolumns+1);
		buildBitString(&bsb, &elemsByColumns, this->ncolumns+1);
		
		// Building bitsequences
		Xa = new BitSequenceRGK(*bsa, 20, 32);
		Xb = new BitSequenceRGK(*bsb, 20, 32);
		delete bsa; delete bsb;

		// Building the sequence 
		if (contiguous) mapperB = new MapperNone();
		else mapperB = new MapperCont(columns, npoints, BitSequenceBuilderRG(20));
			
		mapperB->use();
		Sb = new WaveletTreeNoptrs(columns, npoints, new BitSequenceBuilder375(), mapperB);
		
		delete[] columns;
	}
	
	BinaryRelation* 
	BinaryRelation::load(ifstream & fp)
	{
		// Binary Relation Type
		uint type = loadValue<uint>(fp);
		if(type != BBR) return NULL;
		
		BinaryRelation *br = new BinaryRelation();
		br->type = type;

		// Loading stats
		br->npoints = loadValue<uint>(fp);
		br->nrows = loadValue<uint>(fp);
		br->ncolumns = loadValue<uint>(fp);
		
		// Loading Structures (Xa, Xb, Sb)
		br->Xa = BitSequence::load(fp);
		br->Xb = BitSequence::load(fp);
		br->Sb = Sequence::load(fp);
		
		// Additional Values (for range queries)
		br->sigma = ((WaveletTreeNoptrs*)br->Sb)->map(br->ncolumns);
		br->height = bits(br->sigma);
		
		return br;
	}

	void 
	BinaryRelation::save(ofstream & fp)
	{
		// Binary Relation Type (BBR or LBR)
	        saveValue(fp,type);
		
		// Stats values: npoints, nrows, ncolumns
		saveValue(fp,npoints);
		saveValue(fp,nrows);
		saveValue(fp,ncolumns);
		
		// Saving Structures (Xa, Xb, Sb)
		// Xa and Xb does not exist for LBRNB binary relation
		if (type != LBRNB) 
		{
			Xa->save(fp);
			Xb->save(fp);
		}
		Sb->save(fp);
	}
		
	uint 
	BinaryRelation::nRowsPerColumn(uint column)
	{		
		int pos = Xb->select1(column);
		return Xb->selectNext1(pos+1) - pos - 1;
	}

	uint 
	BinaryRelation::nColumnsPerRow(uint row)
	{
		int pos = Xa->select1(row);
		return Xa->selectNext1(pos+1) - pos - 1;
	}
	
	uint 
	BinaryRelation::rowsPerColumn(uint column, uint **rows)
	{
		uint elements = nRowsPerColumn(column);
		
		if (elements > 0)
		{
			uint *results = new uint[elements];	
			for (uint i=1; i<=elements; i++)
			{
				uint pos = Sb->select(column, i);
				results[i-1] = Xa->select0(pos+1)-pos;
			}
			*rows = results;
		}
		else
		{
			*rows = NULL;
		}
				
		return elements;
	}
	
	uint 
	BinaryRelation::columnsPerRow(uint row, uint **columns)
	{		
		int pos = Xa->select1(row);

		int begin = pos-row+1;
		int end = Xa->selectNext1(pos+1)-(row+1);
		uint elements = end-begin+1;
		
		if (elements > 0)
		{
			uint *results = new uint[elements];			
			for (uint i=0; i<elements; i++) results[i] = Sb->access(begin+i);
			*columns = results;
		}
		else
		{
			*columns = NULL;
		}
		
		return elements;
	}
	
	uint
	BinaryRelation::pointsInRange(uint a1, uint a2, uint b1, uint b2, Point **points)
	{							
		// Determining rows range and checks that is not null
		uint i1 = Xa->select1(a1)-a1+1;
		uint i2 = Xa->select1(a2+1)-(a2+1);
		
		if (i2 >= i1)
		{	
			// Allocating memory for points vector
			(*points) = new Point[i2-i1+1];
			
			// Mapping values
			uint j1 = ((WaveletTreeNoptrs*)Sb)->map(b1);
			if ((((WaveletTreeNoptrs*)Sb)->check(b1)) == false) j1++;
			uint j2 = ((WaveletTreeNoptrs*)Sb)->map(b2);
			
			// Range begins form the 1st-level bitstring of the WT
			// The WT is stored in [0,n-1], but the algorithm perform on
			// [0,n] => 'i1' and 'i2' are updated to 'i1+1' and 'i2+1'
			return Range(i1+1, i2+1, j1, j2, 0, sigma, 0, npoints, 0, points);
		}
		else
		{
			return 0;
		}
	}
	
	uint
	BinaryRelation::Range(uint i1, uint i2, uint j1, uint j2, uint t1, uint t2, int left, int right, uint h, Point **points)
	{									
		if ((i1 > i2) || (t1 > j2) || (t2 < j1))
		{
			return 0;
		}
		else
		{	
			if ((t1 >= j1) && (t2 <= j2) && (t1 == t2))
			{			
				uint tt1 = ((WaveletTreeNoptrs*)Sb)->unmap(t1);				
				for (uint i=i1; i<=i2; i++)
					(*points)[i-i1] = Point(Xa->rank1(Xa->select0(Sb->select(tt1, i)+1)),tt1);
				
				return (i2-i1+1);
			}
			else
			{			
				uint il1 = i1, il2 = i2;
				uint tm = (pow((float)2,(int)(height-h)))/2;
				int x = ((WaveletTreeNoptrs*)Sb)->rangeQuery(h, &il1, &il2, left, right);			
				
				uint nleft = Range(il1, il2, j1, j2, t1, t1+tm-1, left, left+x, h+1, points);
				Point *ptr = (*points+nleft);
				uint nright = Range(i1-il1+1, i2-il2, j1, j2, t1+tm, t2, left+x+1, right, h+1, &ptr);
				
				return nleft + nright;
			}
		}
	}
	
	size_t 
	BinaryRelation::getSize()
	{		
		if (type != LBRNB)
 			return Xa->getSize() + Xb->getSize() + Sb->getSize() + sizeof(BinaryRelation);
		else
			return Sb->getSize() + sizeof(BinaryRelation);
	}

	BinaryRelation::~BinaryRelation()
	{				
		if (type != LBRNB)
		{
			delete Xa; 
			delete Xb;
		}
		delete ((WaveletTreeNoptrs*)Sb);
		
		if (mapperB != NULL) delete mapperB;
	}
	
	void 
	BinaryRelation::buildBitString(BitString **bs, vector<uint> *v, uint elems)
	{
		(*bs)->setBit(0, true);
		uint pos = 0;
	
		for (uint i=1; i<elems; i++)
		{
			(*bs)->setBit((*v)[i]+pos+1, true);
			pos += (*v)[i]+1;
		}
	};

	uint 
	BinaryRelation::getNPoints()
	{
		return npoints;
	}
	
	uint 
	BinaryRelation::getNRows()
	{
		return nrows;
	}
	
	uint 
	BinaryRelation::getNColumns()
	{
		return ncolumns;
	}

};
