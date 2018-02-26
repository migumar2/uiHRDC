/* RePairSLPIndex.cpp
 * Copyright (C) 2010, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * SLP-based index on Re-Pair compression based on:
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

#include "RePairSLPIndex.h"

#include <sys/time.h>
#include <time.h>

namespace cds_static
{
	RePairSLPIndex::RePairSLPIndex()
	{
		mapper = NULL;
	}
	
	void 
	RePairSLPIndex::build(uchar *text, uint length, uint sampling_length, char *filename)
	{
		this->filename = string(filename);
		textlength = length;
		
		// Variables for the compressed sequence
		int * ctext; uint clength;
		// Variables for symbols used in the text
		unsigned char *symbols;
		// Variables for the dictionary rules
		Tdiccarray *dicc; uint cdicc;
		
		RePair compressor;
		compressor.compress(text, length, &ctext, &clength, &symbols, &terminals, &dicc, &cdicc);		
		uint rules = terminals+cdicc;
		
		// *************************************************************
		// Lexicographic dictionary ordering
		//  - The sa_inv vector stores the lexicographic order of each
		//    in the position (it uses the text suffix array)
		//  - A new auxiliar vector (of Rules) joins symbol and pair
		//    dictionaries. Their first ocurrence is update by using the
		//	  information in sa_inv.
		// *************************************************************	
		uint maxlenrule = 1;
		vector<Rule> grammar(rules+1);
		
		for (uint i=0;i<terminals;i++) 
			grammar[i+1] = Rule((int)symbols[i], -1, i+1, 1);
			
		for (uint i=0;i<cdicc; i++)
		{
			grammar[i+terminals+1] = Rule(dicc->rules[i].rule.left+1, dicc->rules[i].rule.right+1, i+terminals+1, dicc->rules[i].len);
			if (grammar[i+terminals+1].len > maxlenrule) maxlenrule = grammar[i+terminals+1].len;
		}
			
		free(symbols);
		Dictionary::destroyDicc(dicc);
		
		for (uint i=1; i<=rules; i++)
		{
			char *s = new char[grammar[i].len+1];
			expandRule(grammar[i], s, &grammar);
			s[grammar[i].len] = '\0';			
			grammar[i].sequence = s; 
		}
		
		sort (grammar.begin()+1, grammar.end(), sorting);
		
		// *************************************************************
		// Building an 'inverted' grammar to update dictionary and 
		// sequence with the new ordering. 
		// The lenrules structure (on DAC) and the bitsequences C and Y 
		// are obtained in this process.
		// *************************************************************
		uint *grammar_inv = new uint[rules+1];
		uint *lengths = new uint[rules];
				
		BitString *bsC = new BitString(256);
		Y = new uint[terminals];
		uint cterms = 0;
		
		for (uint i=1; i<=rules; i++)
		{			
			grammar_inv[grammar[i].id] = i;
			grammar[i].id = i;
			
			lengths[i-1] = grammar[i].len;
			
			if (grammar[i].len == 1)
			{
				bsC->setBit(grammar[i].left,true);

				Y[cterms] = i;
				cterms++;
			}
		}
		
		lenrules = new factorization(lengths, terminals+cdicc);
		C = new BitSequenceRG(*bsC,20);
		
		delete [] lengths;
		delete bsC;

		// *************************************************************
		// Updating symbols in the dictionary and the sequence.
		// *************************************************************
		for (uint i=1; i<=rules; i++)
		{
			if (grammar[i].len != 1)
			{
				grammar[i].left = grammar_inv[grammar[i].left];
				grammar[i].right = grammar_inv[grammar[i].right];
			}
		}
		
		for (uint i=0; i<clength; i++)
			ctext[i] = grammar_inv[ctext[i]+1];			
		
		// *************************************************************
		// Reverse dictionary ordering
		// *************************************************************
		vector<Rule> grammar_rev(rules+1);
		char *xrule = new char[maxlenrule+1];
		
		for (uint i=1; i<=rules; i++)
		{
			grammar_rev[i] = grammar[i];
			
			for (uint j=grammar_rev[i].len; j>0; j--)
				xrule[grammar_rev[i].len-j] = grammar_rev[i].sequence[j-1];
			xrule[grammar_rev[i].len] = '\0';
			
			strcpy(grammar_rev[i].sequence, xrule);
		}
		
		sort(grammar_rev.begin()+1, grammar_rev.end(), sorting);
		
		delete [] xrule;
		
		// *************************************************************
		// Obtaining the permutation from reverse to direct ordering
		// *************************************************************
		uint bitsperm = bits(rules+1);
		uint sizeperm = (int)ceil(double(bitsperm*(rules+1))/W);		
		
		uint *perm = new uint[sizeperm];
		for (uint i=0; i<sizeperm; i++) perm[i] = 0;
		for (uint i=1; i<=rules; i++) set_field(perm, bitsperm, i, grammar_rev[i].id);

		PermutationBuilder *pb = new PermutationBuilderMRRR(PERM_EPS, new BitSequenceBuilderRG(20));
		permutation = pb->build(perm, rules+1);
		delete pb;
		
		// *************************************************************
		// Updating the 'inverted' grammar to obtain the final grammar
		// used to build the binary relation.
		// *************************************************************
		for (uint i=1; i<=rules; i++)
			grammar_inv[grammar_rev[i].id] = i;
		for (uint i=1; i<=rules; i++)
			if (grammar[i].len != 1) grammar[i].left = grammar_inv[grammar[i].left];
			
		delete [] grammar_inv;
		for (uint i=1; i<=terminals+cdicc; i++) delete [] grammar[i].sequence; 		
			
		slp = new LabeledBinaryRelation(&grammar, cdicc, rules, rules, false);
		
		// *************************************************************
		// Building structures for the compressed sequence
		//  - Compressed sequence: symbols + start positions
		//  - prefix/Suffix binary relation for the compressed sequence
		// *************************************************************		
		uint *lengthsSeq = new uint[clength];

		uint current = 0;
		for (uint i=0; i<clength; i++)
		{
			lengthsSeq[i] = lenrules->access(ctext[i]);
			current += lengthsSeq[i];
		}

		seqB = new DeltaCodes(lengthsSeq, clength, DELTA_SAMPLING);
		delete [] lengthsSeq;
		
		// Building the text suffix array
		SuffixArray *sa = new SuffixArray();
		int * sa_ord = sa->sort(text, textlength);
		delete sa; delete [] text;
		
		// Retrieving "valid" suffixes for start positions in the 
		// compressed sequence
		int * suffixes = new int[clength];
		uint csuff = 1;
		
		for (uint i=0; i<=textlength; i++)
		{
			if (sa_ord[i] != 0)
			{
				// The first position in the compressed sequence is not
				// considered as suffix for the binary relation
				
				// Binary search
				uint l = 0, r = clength-1, c;
				int val;
				while (l <= r)
				{
					c = (l+r)/2;
					val = seqB->select(c)+1;
					
					if (val <  sa_ord[i]) l = c+1;
					else
					{
						if (val > sa_ord[i]) r = c-1;
						else break;
					}
				}
				
				if (l <= r)
				{
					suffixes[csuff] = sa_ord[i];
					csuff++;
				}
			}
		}
		
		free(sa_ord);
		
		int * suffixes_inv = new int[length];
		for (uint i=1; i<csuff; i++)
		{
			suffixes_inv[suffixes[i]] = i+1;
		}
		delete [] suffixes;
		
		vector<Rule> seqSLP(csuff);
		
		// Inserts the final position with the first suffix: $
		seqSLP[0] = Rule(1, permutation->revpi(ctext[csuff-1]), csuff+1, 0);
		
		for (uint i=1; i<csuff; i++)
		{
			seqSLP[i] = Rule(suffixes_inv[seqB->select(i)+1], permutation->revpi(ctext[i-1]), i+1, 0);
		}

		delete [] suffixes_inv; free(ctext); 
		
		seqR = new LabeledBinaryRelation();
		seqR->buildNoBitSeq(&seqSLP, csuff, csuff+1, rules, false);
		
		buildSamplings(sampling_length);
	}
			
	int 
	RePairSLPIndex::save()
	{
		// Opening the ouptut stream
		ofstream output(string(filename+string(".ind")).c_str());
		
		if (output.good())
		{
			// Length value
			saveValue(output,textlength);
			saveValue(output,terminals);

			// Saving bitsequence which marks terminal symbols in the SLP
			C->save(output);
			// Saving array which marks terminal rules in the SLP
			saveValue(output, Y, terminals);
			// Saving lenrules DAC representation
			lenrules->save(output);		
			// Saving permutation from reverse to direct rule ordering
			permutation->save(output);
			// Saving the SLP for the RePair grammar
			slp->save(output);
			
			// Saving bitsequence which mark start positions in the 
			// original sequence
			seqB->save(output);
			// Saving the binary relation for prefixes/suffices in the 
			// compressed sequence
			seqR->save(output);		

			output.close();
		}
		else
		{
			return -1;
		}

		// Opening the ouptut stream for samplings
		output.open(string(filename+string(".samp")).c_str());

		if (output.good())
		{
			// Saving the samplings
			grammarSamp->save(output);
			reverseSamp->save(output);
			sequenceSamp->save(output);
			
			output.close();
			return 0;
		}
		else
		{
			return -1;
		}
	}
				
	int
	RePairSLPIndex::load(char *filename, RePairSLPIndex **rpi)
	{
		*rpi = new RePairSLPIndex();
		(*rpi)->filename = string(filename);

		// Opening the input stream
		ifstream input(string((*rpi)->filename+string(".ind")).c_str());
		
		if (input.good())
		{
			(*rpi)->textlength = loadValue<uint>(input);
			(*rpi)->terminals = loadValue<uint>(input);
			
			// Loading all structures in the index
			(*rpi)->C = BitSequence::load(input);
			(*rpi)->Y = loadValue<uint>(input, (*rpi)->terminals);
			(*rpi)->lenrules = factorization::load(input);
			(*rpi)->permutation = Permutation::load(input);
			(*rpi)->slp = LabeledBinaryRelation::load(input);	
			(*rpi)->seqB = DeltaCodes::load(input);
			(*rpi)->seqlength = ((*rpi)->seqB)->elements();
			(*rpi)->seqR = LabeledBinaryRelation::load(input);

			input.close();	

			// Loading samplings
			input.open(string((*rpi)->filename+string(".samp")).c_str());

			if (input.good())
			{
				(*rpi)->grammarSamp = Sampling::load(input);
				(*rpi)->reverseSamp = Sampling::load(input);
				(*rpi)->sequenceSamp = Sampling::load(input);
				input.close();

				return (*rpi)->grammarSamp->getMaxLength();
			}
			else
			{
				delete *rpi; return 0;
			}
		}
		else
		{
			delete *rpi; return 0;
		}
	}
				
	unsigned int 
	RePairSLPIndex::size()
	{
		uint size = 0;

		size += C->getSize();
		size += terminals*sizeof(uint);
		size += lenrules->getSize();
		size += permutation->getSize();
		size += slp->getSize();
		size += seqB->size();
		size += seqR->getSize();
		size += grammarSamp->getSize();
		size += reverseSamp->getSize();
		size += sequenceSamp->getSize();
		size += sizeof(RePairSLPIndex);

		return size;
	}
				
	unsigned int 
	RePairSLPIndex::count(uchar *pattern, uint length)
	{
		FullQuery q(pattern, length, PROOF);
		getPrefixesInSLP(&q);
		getSuffixesInSLP(&q);

		q.FillRangesInSLP();
		RangeQuery *rq = q.rq;
		uint numocc = 0;

		vector<Occ> labels;

		for (uint i=0; i<q.length-1; i++)
		{
			labels.clear();

			if ((rq[i].rows.l != NOTFOUND) && (rq[i].columns.l != NOTFOUND))
			{
				uint occsL = slp->labelsInRange(rq[i], &labels);
				
				for (uint j=0; j<occsL; j++)
				{										
					// Counting rule ocurrences in the compressed sequence
					numocc += seqR->nRowsPerColumnNoBitSeq(permutation->revpi(labels[j].id));
										
					// Track rules using primary ocurrences
					occsL += slp->labelsPerRow(permutation->revpi(labels[j].id), &labels, 0, 0);
					occsL += slp->labelsPerColumn(labels[j].id, &labels, 0, 0);
				}
			}
		}

		getSuffixesInSequence(&q);
		q.FillRangesInSequence();

		for (uint i=0; i<q.length-1; i++)
		{
			if ((rq[i].rows.l != NOTFOUND) && (rq[i].columns.l != NOTFOUND)) 
				numocc += seqR->labelsInRangeNoBitSeq(rq[i], &labels);
		}

		return numocc;
	}

	vector<unsigned int>* 
	RePairSLPIndex::locate(uchar *pattern, uint length, uint *numocc)
	{
		FullQuery q(pattern, length, PROOF);
		getPrefixesInSLP(&q);
		getSuffixesInSLP(&q);

		q.FillRangesInSLP();
		RangeQuery *rq = q.rq;

		vector<Occ> labels;
		vector <uint> *occs = new vector<unsigned int>();
		*numocc = 0;

		for (uint i=0; i<q.length-1; i++)
		{
			if ((rq[i].rows.l != NOTFOUND) && (rq[i].columns.l != NOTFOUND))
			{
				labels.clear();
				uint primary = slp->labelsInRange(rq[i], &labels);
				uint occsL = primary;

				for (uint j=0; j<occsL; j++)
				{
					// Obtaining the offset within the rule (primary ocurrences)
					labels[j].len = (lenrules->access(labels[j].id) - lenrules->access(labels[j].right))-(i+1);
				}

				for (uint j=0; j<occsL; j++)
				{		
					uint lenid = lenrules->access(labels[j].id);

					// Updating offset value (if necessary)
					if (labels[j].right < 0) labels[j].len = lenid + labels[j].right + labels[j].len;
								
					// Counting and locating rule ocurrences in the compressed sequence
					uint revid = permutation->revpi(labels[j].id);
					uint occsR = seqR->nRowsPerColumnNoBitSeq(revid);

					if (occsR > 0)
					{						
						for (uint i=1; i<=occsR; i++)
						{
							uint posseq = seqR->labelForColumnNoBitSeq(revid, i)-2;
							uint pos = seqB->select(posseq)+labels[j].len+2;
							occs->push_back(pos);
						}
						
						*numocc += occsR;
					}
										
					// Track rules using primary ocurrences
					occsL += slp->labelsPerRow(revid, &labels, lenid, labels[j].len);
					occsL += slp->labelsPerColumn(labels[j].id, &labels, lenid, labels[j].len);
				}
			}
		}

		getSuffixesInSequence(&q);
		q.FillRangesInSequence();

		for (uint i=0; i<q.length-1; i++)
		{
			if ((rq[i].rows.l != NOTFOUND) && (rq[i].columns.l != NOTFOUND))
			{ 
				labels.clear();
				uint occsR = seqR->labelsInRangeNoBitSeq(rq[i], &labels);

				for (uint j=0; j<occsR; j++)
				{
					uint pos = seqB->select(labels[j].id-1)+1-i;
					occs->push_back(pos);
				}

				*numocc += occsR;
			}
		}

		return occs;
	}

	unsigned char* 
	RePairSLPIndex::extract(unsigned int from, unsigned int to)
	{
		if ((from <= to) && (from > 0))
		{		
			from--; 
			to--;

			if (to >= textlength) to = textlength-1;
			
			// Locating the position in the compressed sequence
			uint l = 0, r = seqlength-1, c;
			uint beginR;
			
			while (l <= r)
			{				
				c = (l+r)/2;
				beginR = seqB->select(c)+1;
				
				if (beginR < from) l = c+1;
				else if (beginR > from) r = c - 1;
				else break;
			}
			
			if (beginR > from) { c--; beginR = seqB->select(c)+1; }
			if (to > textlength) to = textlength;

			uint snippet_length = to-from+1;
			uchar *snippet = new uchar[snippet_length+grammarSamp->getMaxLength()+1];
			uint read = 0;

			Point p = seqR->firstPointForLabelNoBitSeq(c+2);
			uint rule = permutation->pi(p.right);

			uint len = lenrules->access(rule);
			uint position = from-beginR;

			if (position > 0)
			{
				// Extracts from an internal position
				if ((len-position) > snippet_length)
				{
					// The snippet is fully stored within the current rule				
					extractCharsDirect(rule, len, position, snippet_length, snippet);
					return snippet;
				}
				else
				{
					extractCharsDirect(rule, len, position, len-position, snippet);
					read += len-position;
				}
				
				c++;
			}

			RuleTrav rt(PROOF);

			while (read < snippet_length)
			{					
				// Extracting next rule
				p = seqR->firstPointForLabelNoBitSeq(c+2);
				rule = permutation->pi(p.right);
				len = lenrules->access(rule);

				if (len >= (snippet_length-read)) break;

				rt.ResetRT(rule, len);

				// Extracting the rule 
				while (true)
				{
					if (rt.r[rt.level].len <= grammarSamp->getMaxLength())
					{
						grammarSamp->extract(rt.r[rt.level].id, snippet+read);
						rt.read += rt.r[rt.level].len;
						read += rt.r[rt.level].len;

						if (rt.read == rt.r[0].len) break;	

						do rt.level--; while (rt.d[rt.level]);
				
						rt.r[rt.level].right = slp->firstPointForLabel_Y(rt.r[rt.level].id, rt.p[rt.level]);
						rt.d[rt.level] = true;
		
						rt.level++;
						rt.r[rt.level].id = rt.r[rt.level-1].right;
					}
					else
					{
						rt.r[rt.level].left = permutation->pi(slp->firstPointForLabel_X(rt.r[rt.level].id, &(rt.p[rt.level])));
						rt.d[rt.level] = false;
				
						rt.level++;
						rt.r[rt.level].id = rt.r[rt.level-1].left;
					}
				
					rt.r[rt.level].len = lenrules->access(rt.r[rt.level].id);
				}

				c++;
			}

			if (read != snippet_length)
				extractCharsDirect(rule, len, 0, snippet_length-read, snippet+read);

			return snippet;
		}
		else
		{
			return NULL;
		}
	}
								
	unsigned int 
	RePairSLPIndex::length()
	{
		return textlength;
	}

	void 
	RePairSLPIndex::expandRule(Rule r, char* expanded, vector<Rule> *grammar)
	{
		if (r.len == 1)
		{
			expanded[0] = r.left;
		}
		else
		{			
			expandRule((*grammar)[r.left], expanded, grammar);
			expandRule((*grammar)[r.right], expanded+((*grammar)[r.left]).len, grammar);
		}
	}


	void
	RePairSLPIndex::buildSamplings(uint qsampling)
	{
		{
			// *** GRAMMAR SAMPLING
			// *******************
			uint DELTA_SAMP = 4;
			uint rules = slp->getNColumns();
			uchar *prefix = new uchar[qsampling+1];
			uchar *current = new uchar[qsampling+1];
			current[0] = '\0';

			uint csamp = 0;
			uint *pointers = new uint[rules+1];
			uchar *samples = new uchar[rules*(qsampling+1)];
			pointers[csamp] = 0; csamp++;
			uchar *ptr = samples;

			uint terminals = C->rank1(256);
			uint *posterms = new uint[terminals+1];
			terminals = 0;

			for (uint i=1; i<rules; i++)
			{
				uint plen = lenrules->access(i);

				if (plen > qsampling)
				{
					extractCharsDirectNoSamples(i, plen, qsampling, prefix);
					prefix[qsampling] = '\0';
				}
				else
				{
					extractCharsDirectNoSamples(i, plen, plen, prefix);
					prefix[plen] = '\0';
				}

				if (strcmp((char*)current, (char*)prefix) != 0)
				{
					strcpy((char*)ptr, (char*)prefix);
					ptr += strlen((char*)prefix);
					*ptr = '\0'; ptr++;
				
					if (current[0] != prefix[0])
					{
						posterms[terminals] = ((csamp-1)/BLOCK_SAMPLING);
						terminals++;
					}

					pointers[csamp] = i;
					csamp++;
	
					strcpy((char*)current, (char*)prefix);
				}
			}

			pointers[csamp] = rules+1;
			pointers[csamp+1] = rules+2;

			posterms[terminals] = ((csamp-2)/BLOCK_SAMPLING);
			terminals++;

			grammarSamp = new Sampling(samples, ptr-samples, pointers, csamp, BLOCK_SAMPLING, terminals, posterms, DELTA_SAMP);

			delete [] pointers;
			delete [] samples;
			delete [] prefix; delete [] current;
		}

		{
			// *** REVERSE SAMPLING
			// ********************
			uint DELTA_SAMP = 4;
			uint rules = slp->getNColumns();
			uchar *prefix = new uchar[qsampling+1];
			uchar *suffix = new uchar[qsampling+1];
			uchar *current = new uchar[qsampling+1];
			current[0] = '\0';

			uint csamp = 0;
			uint *pointers = new uint[rules+1];
			uchar *samples = new uchar[rules*(qsampling+1)];
			pointers[csamp] = 0; csamp++;
			uchar *ptr = samples;

			uint terminals = C->rank1(256);
			uint *posterms = new uint[terminals+1];
			terminals = 0;

			for (uint i=1; i<rules; i++)
			{
				uint crule = permutation->pi(i);
				uint plen = lenrules->access(crule);

				if (plen > qsampling)
				{
					extractCharsReverseNoSamples(crule, plen, qsampling, suffix);
					suffix[qsampling] = '\0';
					for (int k=qsampling-1; k>=0; k--) { prefix[qsampling-k-1] = suffix[k]; }
					prefix[qsampling] = '\0';
				}
				else
				{
					extractCharsReverseNoSamples(crule, plen, plen, suffix);
					suffix[plen] = '\0';
					for (int k=plen-1; k>=0; k--) { prefix[plen-k-1] = suffix[k]; }
					prefix[plen] = '\0';
				}

				if (strcmp((char*)current, (char*)prefix) != 0)
				{
					strcpy((char*)ptr, (char*)prefix);
					ptr += strlen((char*)prefix);
					*ptr = '\0'; ptr++;

					if (current[0] != prefix[0])
					{
						posterms[terminals] = ((csamp-1)/BLOCK_SAMPLING);
						terminals++;
					}

					pointers[csamp] = i;
					csamp++;

					strcpy((char*)current, (char*)prefix);
				}
			}

			pointers[csamp] = rules+1;
			pointers[csamp+1] = rules+2;

			posterms[terminals] = ((csamp-2)/BLOCK_SAMPLING);
			terminals++;

			reverseSamp = new Sampling(samples, ptr-samples, pointers, csamp, BLOCK_SAMPLING, terminals, posterms, DELTA_SAMP);

			delete [] pointers;
			delete [] samples;
			delete [] prefix; delete [] suffix; delete [] current;
		}

		{
			// *** SEQUENCE SAMPLING
			// ********************
			uint DELTA_SAMP = 4;
			uint lenseq = seqR->getNRows();
			uchar *prefix = new uchar[qsampling+1];
			uchar *current = new uchar[qsampling+1];
			current[0] = '\0';
		
			uint csamp = 0;
			uint *pointers = new uint[lenseq+1];
			uchar *samples = new uchar[lenseq*(qsampling+1)];
			pointers[csamp] = 0; csamp++;
			uchar *ptr = samples;

			uint terminals = C->rank1(256);
			uint *posterms = new uint[terminals+1];
			terminals = 0;

			for (uint i=2; i<lenseq; i++)	
			{
				uint pos = seqR->labelForRowNoBitSeq(i)-1;
				uint rule = permutation->pi(seqR->retrieveColumnPerLabelNoBitSeq(pos+2));
				uint len = lenrules->access(rule);

				if (len > qsampling)
				{
					extractCharsDirectNoSamples(rule, len, qsampling, prefix);
					prefix[qsampling] = '\0';
				}
				else
				{
					uint read = 0;

					while (true)
					{
						if (len < qsampling-read)
						{
							extractCharsDirectNoSamples(rule, len, len, prefix+read);
							read += len;
						}
						else
						{
							extractCharsDirectNoSamples(rule, len, qsampling-read, prefix+read);
							read += qsampling-read;
							break;
						}

						pos++;
						
						if (pos == (lenseq-1))  break;

						rule = permutation->pi(seqR->retrieveColumnPerLabelNoBitSeq(pos+2));
						len = lenrules->access(rule);
					}

					 prefix[read] = '\0';
				}

				if (strcmp((char*)current, (char*)prefix) != 0)
				{
					strcpy((char*)ptr, (char*)prefix);
					ptr += strlen((char*)prefix);
					*ptr = '\0'; ptr++;

					if (current[0] != prefix[0])
					{
						if (C->select1(terminals+1) != (size_t)prefix[0])
						{
							size_t t;

							do
							{
								posterms[terminals] = ((csamp-1)/BLOCK_SAMPLING);
								terminals++;
								t = C->select1(terminals+1);
							}
							while (t != (size_t)prefix[0]);
						}

						posterms[terminals] = ((csamp-1)/BLOCK_SAMPLING);
						terminals++;
					}

					pointers[csamp] = i;
					csamp++;

					strcpy((char*)current, (char*)prefix);
				}		
			}

			pointers[csamp] = lenseq;
			pointers[csamp+1] = lenseq+1;

			posterms[terminals] = ((csamp-2)/BLOCK_SAMPLING);
			terminals++;

			sequenceSamp = new Sampling(samples, ptr-samples, pointers, csamp, BLOCK_SAMPLING, terminals, posterms, DELTA_SAMP);

			delete [] pointers;
			delete [] samples;
			delete [] prefix; delete [] current;
		}
	}

	void 
	RePairSLPIndex::extractCharsDirectNoSamples(uint rule, uint len, uint chars, uchar *sample)
	{
		RuleTrav rt(rule, len, PROOF);

		// Extracting the rule 
		while (true)
		{			
			if (rt.r[rt.level].len > 1)
			{
				rt.r[rt.level].left = permutation->pi(slp->firstPointForLabel_X(rt.r[rt.level].id, &(rt.p[rt.level])));
				rt.d[rt.level] = false;
				
				rt.level++;
				rt.r[rt.level].id = rt.r[rt.level-1].left;
			}
			else
			{				
				sample[rt.read] = (uchar)C->select1(rankY(rt.r[rt.level].id));		
				rt.read++;
				if (rt.read == chars) break;
				
				do rt.level--; while (rt.d[rt.level]);
				
				rt.r[rt.level].right = slp->firstPointForLabel_Y(rt.r[rt.level].id, rt.p[rt.level]);
				rt.d[rt.level] = true;
				
				rt.level++;
				rt.r[rt.level].id = rt.r[rt.level-1].right;
			}
				
			rt.r[rt.level].len = lenrules->access(rt.r[rt.level].id);
		}
	}
	
	void 
	RePairSLPIndex::extractCharsReverseNoSamples(uint rule, uint len, uint chars, uchar *sample)
	{
		RuleTrav rt(rule, len, PROOF);		
		rt.read = chars-1;
		
		while (true)
		{			
			if (rt.r[rt.level].len > 1)
			{
				rt.r[rt.level].right = slp->firstPointForLabel_Y(rt.r[rt.level].id, &(rt.p[rt.level]));
				rt.d[rt.level] = false;
				
				rt.level++;
				rt.r[rt.level].id = rt.r[rt.level-1].right;
			}
			else
			{
				sample[rt.read] = (uchar)C->select1(rankY(rt.r[rt.level].id));
				
				if (rt.read == 0) break;
				rt.read--;
				
				do rt.level--; while (rt.d[rt.level]);
				
				rt.r[rt.level].left = permutation->pi(slp->firstPointForLabel_X(rt.r[rt.level].id, rt.p[rt.level]));
				rt.d[rt.level] = true;
				
				rt.level++;
				rt.r[rt.level].id = rt.r[rt.level-1].left;
			}
				
			rt.r[rt.level].len = lenrules->access(rt.r[rt.level].id);
		}
	}

	void
	RePairSLPIndex::extractCharsDirect(uint rule, uint len, uint position, uint chars, uchar *snippet)
	{
		RuleTrav rt(rule, lenrules->access(rule), PROOF);
		rt.discard = position;
		if (rt.discard > 0) jumpCharsDirect(&rt);

		position -= rt.read;
		rt.read = 0;

		if (position > 0)
		{
			// The current rule contains chars which must be extracted
			grammarSamp->extract(rt.r[rt.level].id, rt.xseq);
			strcpy((char*)(snippet), (char*)(rt.xseq+position));
			rt.read += rt.r[rt.level].len-position;

			if (rt.read >= chars)
			{
				snippet[chars] = '\0';
				return;
			}
			do rt.level--; while (rt.d[rt.level]);
				
			rt.r[rt.level].right = slp->firstPointForLabel_Y(rt.r[rt.level].id, rt.p[rt.level]);
			rt.d[rt.level] = true;
				
			rt.level++;
			rt.r[rt.level].id = rt.r[rt.level-1].right;
			rt.r[rt.level].len = lenrules->access(rt.r[rt.level].id);
		}

		// Extracting the rule 
		while (true)
		{		
			if (rt.r[rt.level].len <= grammarSamp->getMaxLength())
			{
				grammarSamp->extract(rt.r[rt.level].id, rt.xseq);
				strcpy((char*)(snippet+rt.read), (char*)rt.xseq);
				rt.read += rt.r[rt.level].len;

				if (rt.read >= chars)
				{
					snippet[chars] = '\0';
					break;	
				}
				do rt.level--; while (rt.d[rt.level]);
				
				rt.r[rt.level].right = slp->firstPointForLabel_Y(rt.r[rt.level].id, rt.p[rt.level]);
				rt.d[rt.level] = true;
				
				rt.level++;
				rt.r[rt.level].id = rt.r[rt.level-1].right;
			}
			else
			{
				rt.r[rt.level].left = permutation->pi(slp->firstPointForLabel_X(rt.r[rt.level].id, &(rt.p[rt.level])));
				rt.d[rt.level] = false;
				
				rt.level++;
				rt.r[rt.level].id = rt.r[rt.level-1].left;
			}
				
			rt.r[rt.level].len = lenrules->access(rt.r[rt.level].id);
		}
	}


	void
	RePairSLPIndex::getPrefixesInSLP(FullQuery *q)
	{
		uchar *xseq = new uchar[q->length+reverseSamp->getMaxLength()+1];
		PSpattern *prefix;

		// Initialishing with a previous char...
		uchar first = q->prefixes[0].spattern[0];
		uint pfirst = C->rank1(first);
		q->prefixes[0].lcp = 0;

		q->prefixes[0].blocks.l = reverseSamp->leftBlockForTerminal(pfirst);
		q->prefixes[0].blocks.r = reverseSamp->rightBlockForTerminal(pfirst);

		for (uint curr=0; curr<q->nprefixes; curr++)
		{
			prefix = &(q->prefixes[curr]);

			if (prefix->length == 1)
			{
				// The range for the prefix with length=1 is obtained in C
				uint pos = C->rank1((uint)prefix->spattern[0]);
				
				if (pos == 1)
				{
					prefix->limits.l = 1;
					prefix->limits.r = permutation->revpi(Y[pos])-1;
				}
				else
				{
					prefix->limits.l = permutation->revpi(Y[pos-1]);
					if (pos == terminals) prefix->limits.r = slp->getNColumns();
					else prefix->limits.r = permutation->revpi(Y[pos])-1;
				}
			}
			else
			{
				if (prefix->length < reverseSamp->getMaxLength())
				{	
					// Direct sampling searching
					reverseSamp->locatePrefix(prefix->spattern, 
								  prefix->length, 
								  &(prefix->limits.l), &(prefix->limits.r), 
								  &(prefix->blocks.l), &(prefix->blocks.r));
				}
				else
				{
					// Using the sampling for searching the front of the pattern...
					strncpy((char*)xseq, (char*)prefix->spattern, reverseSamp->getMaxLength());
					xseq[reverseSamp->getMaxLength()] = '\0';
					reverseSamp->locatePattern(xseq, reverseSamp->getMaxLength(), 
								   &(prefix->limits.l), &(prefix->limits.r), &(prefix->blocks.l));
					prefix->blocks.r = prefix->blocks.l;

					if (prefix->limits.l > 0)
					{
						prefix->eqchars.l = reverseSamp->getMaxLength();
						prefix->eqchars.r = reverseSamp->getMaxLength();

						if (prefix->length > reverseSamp->getMaxLength()) expandAndCompareReverse(prefix, q);
					}
				}
			}

			if (curr < q->nprefixes-1)
			{
				if (q->prefixes[curr+1].spattern[0] != first)
				{
					first = q->prefixes[curr+1].spattern[0];
					q->prefixes[curr+1].lcp = 0;
				}
				else
				{
					q->prefixes[curr+1].lcp = lcp(prefix->spattern, q->prefixes[curr+1].spattern, 
								      prefix->length, prefix->length);
				}

				if (prefix->length > 1)
				{
					if (prefix->limits.l > 0)
					{
						if (q->prefixes[curr+1].lcp == prefix->length)
						{
							q->prefixes[curr+1].blocks.l = prefix->blocks.l;
							q->prefixes[curr+1].blocks.r = prefix->blocks.r; 
						}
						else
						{
							pfirst = C->rank1(q->prefixes[curr+1].spattern[0]);
							q->prefixes[curr+1].blocks.l = prefix->blocks.r;
							q->prefixes[curr+1].blocks.r = reverseSamp->rightBlockForTerminal(pfirst);
						}
					}
					else
					{
						pfirst = C->rank1(q->prefixes[curr+1].spattern[0]);
						q->prefixes[curr+1].blocks.l = prefix->blocks.r;
						q->prefixes[curr+1].blocks.r = reverseSamp->rightBlockForTerminal(pfirst);
					}	
				}
				else
				{
					pfirst = C->rank1(q->prefixes[curr+1].spattern[0]);
					q->prefixes[curr+1].blocks.l = reverseSamp->leftBlockForTerminal(pfirst);
					q->prefixes[curr+1].blocks.r = reverseSamp->rightBlockForTerminal(pfirst);
				}				
			}
		}

		delete [] xseq;
	}

	void
	RePairSLPIndex::getSuffixesInSLP(FullQuery *q)
	{
		uchar *xseq = new uchar[q->length+grammarSamp->getMaxLength()+1];
		PSpattern *suffix;

		q->FillSuffixesG();

		// Initialishing with a previous char...
		uchar first = q->suffixesG[0].spattern[0];
		uint pfirst = C->rank1(first);
		q->suffixesG[0].lcp = 0;

		q->suffixesG[0].blocks.l = grammarSamp->leftBlockForTerminal(pfirst);
		q->suffixesG[0].blocks.r = grammarSamp->rightBlockForTerminal(pfirst);

		for (uint curr=0; curr<q->nsuffixesG; curr++)
		{
			suffix = &(q->suffixesG[curr]);

			if (suffix->length == 1)
			{
				// The range for the suffix with length=1 is obtained in C
				uint pos = C->rank1((uint)suffix->spattern[0]);
				
				if (pos == 1)
				{
					suffix->limits.l = 1;
					suffix->limits.r = Y[pos]-1;
				}
				else
				{
					suffix->limits.l = Y[pos-1];
					if (pos == terminals) suffix->limits.r = slp->getNColumns();
					else suffix->limits.r = Y[pos]-1;
				}
			}
			else
			{
				if (suffix->length < grammarSamp->getMaxLength())
				{
					// Direct sampling searching
					grammarSamp->locatePrefix(suffix->spattern, 
								  suffix->length, 
								  &(suffix->limits.l), &(suffix->limits.r), 
								  &(suffix->blocks.l), &(suffix->blocks.r));
				}
				else
				{
					// Using the sampling for searching the front of the pattern...
					strncpy((char*)xseq, (char*)suffix->spattern, grammarSamp->getMaxLength());
					xseq[grammarSamp->getMaxLength()] = '\0';
					grammarSamp->locatePattern(xseq, grammarSamp->getMaxLength(), 
								   &(suffix->limits.l), &(suffix->limits.r), &(suffix->blocks.l));
					suffix->blocks.r = suffix->blocks.l;

					if (suffix->limits.l > 0)
					{
						suffix->eqchars.l = grammarSamp->getMaxLength();
						suffix->eqchars.r = grammarSamp->getMaxLength();

						if (suffix->length > grammarSamp->getMaxLength()) expandAndCompareDirectInG(suffix, q);
					}					
				}
			}

			if (curr < q->nsuffixesG-1)
			{
				if (q->suffixesG[curr+1].spattern[0] != first)
				{
					first = q->suffixesG[curr+1].spattern[0];
					q->suffixesG[curr+1].lcp = 0;
				}
				else
				{
					q->suffixesG[curr+1].lcp = lcp(suffix->spattern, q->suffixesG[curr+1].spattern, 
								       suffix->length, suffix->length);


				}

				if (suffix->length > 1)
				{
					if (suffix->limits.l > 0)
					{
						if (q->suffixesG[curr+1].lcp == suffix->length)
						{
							q->suffixesG[curr+1].blocks.l = suffix->blocks.l;
							q->suffixesG[curr+1].blocks.r = suffix->blocks.r; 
						}
						else
						{
							pfirst = C->rank1(q->suffixesG[curr+1].spattern[0]);
							q->suffixesG[curr+1].blocks.l = suffix->blocks.r;
							q->suffixesG[curr+1].blocks.r = grammarSamp->rightBlockForTerminal(pfirst);
						}
					}
					else
					{
						pfirst = C->rank1(q->suffixesG[curr+1].spattern[0]);
						q->suffixesG[curr+1].blocks.l = suffix->blocks.r;
						q->suffixesG[curr+1].blocks.r = grammarSamp->rightBlockForTerminal(pfirst);
					}	
				}
				else
				{
					pfirst = C->rank1(q->suffixesG[curr+1].spattern[0]);
					q->suffixesG[curr+1].blocks.l = grammarSamp->leftBlockForTerminal(pfirst);
					q->suffixesG[curr+1].blocks.r = grammarSamp->rightBlockForTerminal(pfirst);
				}				
			}
		}

		delete [] xseq;
	}

	void 
	RePairSLPIndex::getSuffixesInSequence(FullQuery *q)
	{
		uchar *xseq = new uchar[q->length+sequenceSamp->getMaxLength()+1];
		PSpattern *suffix;

		q->FillSuffixesS();

		// Initialishing with a previous char...
		uchar first = q->suffixesS[0].spattern[0];
		uint pfirst = C->rank1(first);
		q->suffixesS[0].lcp = 0;

		q->suffixesS[0].blocks.l = sequenceSamp->leftBlockForTerminal(pfirst);
		q->suffixesS[0].blocks.r = sequenceSamp->rightBlockForTerminal(pfirst);

		for (uint curr=0; curr<q->nsuffixesS; curr++)
		{
			suffix = &(q->suffixesS[curr]);

			if (suffix->length < sequenceSamp->getMaxLength())
			{
				// Direct sampling searching
				sequenceSamp->locatePrefix(suffix->spattern, 
							   suffix->length, 
							   &(suffix->limits.l), &(suffix->limits.r), 
							   &(suffix->blocks.l), &(suffix->blocks.r));
			}
			else
			{
				// Using the sampling for searching the front of the pattern...
				strncpy((char*)xseq, (char*)suffix->spattern, sequenceSamp->getMaxLength());
				xseq[sequenceSamp->getMaxLength()] = '\0';
				sequenceSamp->locatePattern(xseq, sequenceSamp->getMaxLength(), 
								   &(suffix->limits.l), &(suffix->limits.r), &(suffix->blocks.l));
				suffix->blocks.r = suffix->blocks.l;

				if (suffix->limits.l > 0)
				{
					suffix->eqchars.l = sequenceSamp->getMaxLength();
					suffix->eqchars.r = sequenceSamp->getMaxLength();

					if (suffix->length > sequenceSamp->getMaxLength()) expandAndCompareDirectInS(suffix, q);
				}					
			}

			if (curr < q->nsuffixesS-1)
			{
				if (q->suffixesS[curr+1].spattern[0] != first)
				{
					first = q->suffixesS[curr+1].spattern[0];
					q->suffixesS[curr+1].lcp = 0;
				}
				else
				{
					q->suffixesS[curr+1].lcp = lcp(suffix->spattern, q->suffixesS[curr+1].spattern, 
								       suffix->length, suffix->length);
				}

				if (suffix->length > 1)
				{
					if (suffix->limits.l > 0)
					{
						if (q->suffixesS[curr+1].lcp == suffix->length)
						{
							q->suffixesS[curr+1].blocks.l = suffix->blocks.l;
							q->suffixesS[curr+1].blocks.r = suffix->blocks.r; 
						}
						else
						{
							pfirst = C->rank1(q->suffixesS[curr+1].spattern[0]);
							q->suffixesS[curr+1].blocks.l = suffix->blocks.r;
							q->suffixesS[curr+1].blocks.r = sequenceSamp->rightBlockForTerminal(pfirst);
						}
					}
					else
					{
						pfirst = C->rank1(q->suffixesS[curr+1].spattern[0]);
						q->suffixesS[curr+1].blocks.l = suffix->blocks.r;
						q->suffixesS[curr+1].blocks.r = sequenceSamp->rightBlockForTerminal(pfirst);
					}	
				}
				else
				{
					pfirst = C->rank1(q->suffixesS[curr+1].spattern[0]);
					q->suffixesS[curr+1].blocks.l = sequenceSamp->leftBlockForTerminal(pfirst);
					q->suffixesS[curr+1].blocks.r = sequenceSamp->rightBlockForTerminal(pfirst);
				}				
			}
		}

		delete [] xseq;
	}

	void
	RePairSLPIndex::jumpCharsReverse(RuleTrav *rt)
	{
		while (rt->read <= rt->discard)
		{
			if ((rt->r[rt->level].len+rt->read > rt->discard) && 
	 		    (rt->r[rt->level].len <= grammarSamp->getMaxLength()))
			{

				break;
			}
			else
			{
				rt->r[rt->level].right = slp->firstPointForLabel_Y(rt->r[rt->level].id, &(rt->p[rt->level]));
				rt->d[rt->level] = false;

				rt->level++;
				rt->r[rt->level].len = lenrules->access(rt->r[rt->level-1].right);

				if ((rt->read + rt->r[rt->level].len) <= rt->discard)
				{
					rt->read += rt->r[rt->level].len;

					// Traversing the left child
					rt->r[rt->level-1].left = permutation->pi(slp->firstPointForLabel_X(rt->r[rt->level-1].id, rt->p[rt->level-1]));
					rt->r[rt->level].id = rt->r[rt->level-1].left;
					rt->r[rt->level].len = rt->r[rt->level-1].len-rt->r[rt->level].len;
					rt->d[rt->level-1] = true;

					if (rt->r[rt->level].len == 1) break;
				}
				else
				{
					// Traversing the right child
					rt->r[rt->level].id = rt->r[rt->level-1].right;
					if (rt->r[rt->level].len == 1) break;
				}
			}
		}
	}

	void
	RePairSLPIndex::jumpCharsDirect(RuleTrav *rt)
	{
		while (rt->read <= rt->discard)
		{
			if ((rt->r[rt->level].len+rt->read > rt->discard) && 
	 		    (rt->r[rt->level].len <= grammarSamp->getMaxLength()))
			{
				break;
			}
			else
			{
				rt->r[rt->level].right = slp->firstPointForLabel_Y(rt->r[rt->level].id, &(rt->p[rt->level]));
				rt->d[rt->level] = true;

				rt->level++;
				rt->r[rt->level].len = lenrules->access(rt->r[rt->level-1].right);
				uint llen = rt->r[rt->level-1].len-rt->r[rt->level].len;

				if ((rt->read + llen) <= rt->discard)
				{
					rt->read += llen;

					// Traversing the right child
					rt->r[rt->level].id = rt->r[rt->level-1].right;
					if (rt->r[rt->level].len == 1) break;
				}
				else
				{
					// Traversing the left child
					rt->r[rt->level-1].left = permutation->pi(slp->firstPointForLabel_X(rt->r[rt->level-1].id, rt->p[rt->level-1]));
					rt->r[rt->level].id = rt->r[rt->level-1].left;
					rt->r[rt->level].len = rt->r[rt->level-1].len-rt->r[rt->level].len;
					rt->d[rt->level-1] = false;

					if (rt->r[rt->level].len == 1) break;
				}
			}
		}
	}

	void 
	RePairSLPIndex::expandAndCompareReverse(PSpattern *prefix, FullQuery *q)
	{
		if (prefix->limits.l > prefix->limits.r)
		{		
			// The pattern is not in the grammar
			prefix->limits.l = NOTFOUND;
			prefix->limits.r = NOTFOUND;						
			return;
		}

		bool found = false;

		RuleTrav *rt = q->rt;

		// Updating the stucture for rule traversing
		uint pivot = (prefix->limits.l + prefix->limits.r)/2;
		uint idp = permutation->pi(pivot);
		rt->ResetRT(idp, lenrules->access(idp));

		// Calculating the number of characters to be discarded
		rt->discard = prefix->eqchars.l;
		if (prefix->eqchars.r < rt->discard) rt->discard = prefix->eqchars.r;
		
		if (rt->r[rt->level].len <= rt->discard)
		{
			prefix->limits.l = pivot+1;
			prefix->eqchars.l = rt->r[rt->level].len;
		}
		else
		{
			jumpCharsReverse(q->rt);
			int cmp = expandRuleAndCompareReverse(prefix, q);

			if (cmp < 0)
			{
				prefix->limits.l = pivot+1;
				prefix->eqchars.l = rt->discard;
			}
			else
			{
				if (cmp > 0)
				{
					prefix->limits.r = pivot-1;
					prefix->eqchars.r = rt->discard;
				}
				else
				{
					found = true;
				}
			}
		}

		if (found) obtainRangeReverse(pivot, prefix, q);
		else expandAndCompareReverse(prefix, q);
	}

	void 
	RePairSLPIndex::expandAndCompareDirectInG(PSpattern *suffix, FullQuery *q)
	{
		if (suffix->limits.l > suffix->limits.r)
		{		
			// The pattern is not in the grammar
			suffix->limits.l = NOTFOUND;
			suffix->limits.r = NOTFOUND;						
			return;
		}

		bool found = false;

		RuleTrav *rt = q->rt;

		// Updating the stucture for rule traversing
		uint pivot = (suffix->limits.l + suffix->limits.r)/2;
		rt->ResetRT(pivot, lenrules->access(pivot));

		// Calculating the number of characters to be discarded
		rt->discard = suffix->eqchars.l;
		if (suffix->eqchars.r < rt->discard) rt->discard = suffix->eqchars.r;

		if (rt->r[rt->level].len <= rt->discard)
		{
			suffix->limits.l = pivot+1;
			suffix->eqchars.l = rt->r[rt->level].len;
		}
		else
		{
			jumpCharsDirect(q->rt);
			int cmp = expandRuleAndCompareDirectInG(suffix, q);

			if (cmp < 0)
			{
				suffix->limits.l = pivot+1;
				suffix->eqchars.l = rt->discard;
			}
			else
			{
				if (cmp > 0)
				{
					suffix->limits.r = pivot-1;
					suffix->eqchars.r = rt->discard;
				}
				else
				{
					found = true;
				}
			}
		}		

		if (found) obtainRangeDirectInG(pivot, suffix, q);
		else expandAndCompareDirectInG(suffix, q);
	}

	void 
	RePairSLPIndex::expandAndCompareDirectInS(PSpattern *suffix, FullQuery *q)
	{
		if (suffix->limits.l > suffix->limits.r)
		{		
			// The pattern is not in the grammar
			suffix->limits.l = NOTFOUND;
			suffix->limits.r = NOTFOUND;						
			return;
		}

		bool found = false;

		RuleTrav *rt;

		// Calculating the number of characters to be discarded
		uint xdiscard = suffix->eqchars.l;	
		if (suffix->eqchars.r < xdiscard) xdiscard = suffix->eqchars.r;
		uint x2discard = xdiscard;	// This variable stores the number of original chars to be discarded
		uint xread = 0;

		// Updating the stucture for rule traversing
		uint pivot = (suffix->limits.l + suffix->limits.r)/2;
		uint pos = seqR->labelForRowNoBitSeq(pivot)-1;

		while (true)
		{
			uint id = permutation->pi(seqR->retrieveColumnPerLabelNoBitSeq(pos+2));
			q->rt->ResetRT(id, lenrules->access(id));
			rt = q->rt;

			if (xdiscard < rt->r[rt->level].len) break;

			xdiscard -= rt->r[rt->level].len;
			xread += rt->r[rt->level].len;
			pos++;
		}

		
		rt->discard = xdiscard;
		if (rt->discard > 0) jumpCharsDirect(q->rt);
		rt->discard = x2discard;
		uint jumped = rt->read; 	// This variable is for counting the number of chars jumped in the current rule
		rt->read += xread;

		int cmp = expandRuleAndCompareDirectInS(suffix, pos, jumped, q);

		if (cmp < 0)
		{
			suffix->limits.l = pivot+1;
			suffix->eqchars.l = rt->discard;
		}
		else
		{
			if (cmp > 0)
			{
				suffix->limits.r = pivot-1;
				suffix->eqchars.r = rt->discard;
			}
			else
			{
				found = true;
			}
		}

		
		if (found) obtainRangeDirectInS(pivot, suffix, q);
		else expandAndCompareDirectInS(suffix, q);
	}

	void 
	RePairSLPIndex::obtainRangeReverse(uint rule, PSpattern *prefix, FullQuery *q)
	{
		RuleTrav *rt = q->rt;

		uint rl = prefix->limits.l, rr = rule, rc;
		uint pivot;
		
		if (rl != rr)
		{
			// Get LEFT limit
			do
			{
				rt->ResetRT(0 ,0);
				pivot = (rl+rr)/2;
				rc = permutation->pi(pivot);
		
				rt->r[rt->level].id = rc;
				rt->r[rt->level].len = lenrules->access(rt->r[rt->level].id);

				if (rt->r[rt->level].len > prefix->eqchars.l)
				{
					rt->discard = prefix->eqchars.l;
					jumpCharsReverse(q->rt);
					uint xread = prefix->eqchars.l;
					int cmp = expandRuleAndCompareReverse(prefix, q);					

					if (cmp < 0)
					{
						rl = pivot+1;
						prefix->eqchars.l = xread;		
					}
					else
					{
						rr = pivot;
					}
				}
				else
				{
					rl = pivot+1;
				}
			}
			while (rl < rr);
			
			prefix->limits.l = rl;
		}

		rl = rule; rr = prefix->limits.r;

		if (rl != rr)
		{
			// Get RIGHT limit
			do
			{
				rt->ResetRT(0 ,0);
				pivot = 1+(rl+rr)/2;
				rc = permutation->pi(pivot);
		
				rt->r[rt->level].id = rc;
				rt->r[rt->level].len = lenrules->access(rt->r[rt->level].id);

				if (rt->r[rt->level].len > prefix->eqchars.r)
				{
					rt->discard = prefix->eqchars.r;
					jumpCharsReverse(q->rt);
					uint xread = prefix->eqchars.r;
					int cmp = expandRuleAndCompareReverse(prefix, q);
				
					if (cmp > 0)
					{
						rr = pivot-1;
						prefix->eqchars.r = xread;
					}
					else
					{
						rl = pivot;
					}
				}
				else
				{
					rr = pivot-1;
				}
			}
			while (rl < rr);
			
			prefix->limits.r = rl;
		}
	}

	void 
	RePairSLPIndex::obtainRangeDirectInG(uint rule, PSpattern *suffix, FullQuery *q)
	{
		RuleTrav *rt = q->rt;
		uint rl = suffix->limits.l, rr = rule, rc;
		
		if (rl != rr)
		{
			// Get LEFT limit	
			do
			{
				rt->ResetRT(0 ,0);
				rc = (rl+rr)/2;
		
				rt->r[rt->level].id = rc;
				rt->r[rt->level].len = lenrules->access(rt->r[rt->level].id);

				if (rt->r[rt->level].len > suffix->eqchars.l)
				{
					rt->discard = suffix->eqchars.l;
					jumpCharsDirect(q->rt);
					uint xread = suffix->eqchars.l;
					int cmp = expandRuleAndCompareDirectInG(suffix, q);
					
					if (cmp < 0)
					{
						rl = rc+1;
						suffix->eqchars.l = xread;
					}
					else
					{
						rr = rc;
					}
				}
				else
				{
					rl = rc+1;					
				}
			}
			while (rl < rr);
			
			suffix->limits.l = rl;
		}
		
		rl = rule; rr = suffix->limits.r;

		if (rl != rr)
		{				
			// Get RIGHT limit
			do
			{
				rt->ResetRT(0 ,0);
				rc = 1+((rl+rr)/2);
		
				rt->r[rt->level].id = rc;
				rt->r[rt->level].len = lenrules->access(rt->r[rt->level].id);
				
				if (rt->r[rt->level].len > suffix->eqchars.r)
				{
					rt->discard = suffix->eqchars.r;
					jumpCharsDirect(q->rt);
					uint xread = suffix->eqchars.r;
					int cmp = expandRuleAndCompareDirectInG(suffix, q);
					
					if (cmp > 0)
					{
						rr = rc-1;
						suffix->eqchars.r = xread;
					}
					else
					{
						rl = rc;
					}
				}
				else
				{
					rr = rc-1;
				}
			}
			while (rl < rr);
			
			suffix->limits.r = rl;
		}
	}

	void 
	RePairSLPIndex::obtainRangeDirectInS(uint rule, PSpattern *suffix, FullQuery *q)
	{
		RuleTrav *rt = q->rt;

		uint rl = suffix->limits.l, rr = rule, rc;
		uint xdiscard, x2discard, xread;
		uint pos;
		
		if (rl != rr)
		{
			// Get LEFT limit	
			do
			{
				xdiscard = suffix->eqchars.l;	
				x2discard = xdiscard;
				xread = 0;

				rc = (rl+rr)/2;
				pos = seqR->labelForRowNoBitSeq(rc)-1;

				while (true)
				{
					uint id = permutation->pi(seqR->retrieveColumnPerLabelNoBitSeq(pos+2));
					rt->ResetRT(id, lenrules->access(id));
			
					if (xdiscard < rt->r[rt->level].len) break;

					xdiscard -= rt->r[rt->level].len;
					xread += rt->r[rt->level].len;
					pos++;
				}


				rt->discard = xdiscard;
				if (rt->discard > 0) jumpCharsDirect(q->rt);
				rt->discard = x2discard;
				uint jumped = rt->read;
				rt->read += xread;

				int cmp = expandRuleAndCompareDirectInS(suffix, pos, jumped, q);

				if (cmp < 0)
				{
					rl = rc+1;
					suffix->eqchars.l = rt->discard;
				}
				else
				{
					rr = rc;
				}
			}
			while (rl < rr);
			
			suffix->limits.l = rl;
		}
		
		rl = rule; rr = suffix->limits.r;

		if (rl != rr)
		{				
			// Get RIGHT limit
			do
			{
				xdiscard = suffix->eqchars.r;	
				x2discard = xdiscard;
				xread = 0;

				rc = 1+((rl+rr)/2);
				pos = seqR->labelForRowNoBitSeq(rc)-1;

				while (true)
				{
					uint id = permutation->pi(seqR->retrieveColumnPerLabelNoBitSeq(pos+2));
					rt->ResetRT(id, lenrules->access(id));
			
					if (xdiscard < rt->r[rt->level].len) break;

					xdiscard -= rt->r[rt->level].len;
					xread += rt->r[rt->level].len;
					pos++;
				}

				rt->discard = xdiscard;
				if (rt->discard > 0) jumpCharsDirect(q->rt);
				rt->discard = x2discard;
				uint jumped = rt->read;
				rt->read += xread;

				int cmp = expandRuleAndCompareDirectInS(suffix, pos, jumped, q);
		
				if (cmp > 0)
				{
					rr = rc-1;
					suffix->eqchars.l = xread;
				}
				else
				{
					rl = rc;
				}
			}
			while (rl < rr);
			
			suffix->limits.r = rl;
		}
	}

	int 
	RePairSLPIndex::expandRuleAndCompareReverse(PSpattern *prefix, FullQuery *q)
	{
		RuleTrav *rt = q->rt;
		int cmp;
		bool found = false;

		while (true)
		{
			grammarSamp->extract(rt->r[rt->level].id, rt->xseq);

			// The current rule is fully sampled
			if (rt->r[rt->level].len <= grammarSamp->getMaxLength())
			{
				grammarSamp->extract(rt->r[rt->level].id, rt->xseq);
				int remaining = rt->r[rt->level].len - (rt->discard - rt->read) - 1;
				rt->read += rt->r[rt->level].len;

				for (;remaining>=0; remaining--)
				{
					if (rt->xseq[remaining] != prefix->spattern[rt->discard])
					{
						cmp = rt->xseq[remaining] - prefix->spattern[rt->discard];
						break;
					}
					else
					{
						rt->discard++;
						if (rt->discard == prefix->length)
						{
							cmp = 0;
							found = true;
							break;
						}
					}
				}

				if ((remaining >= 0) || (found == true)) break;

				if (rt->read == rt->r[0].len)
				{
					cmp = -rt->xseq[remaining+1];
					break;					
				}
				
				do rt->level--; while (rt->d[rt->level]);
				
				rt->r[rt->level].left = permutation->pi(slp->firstPointForLabel_X(rt->r[rt->level].id, rt->p[rt->level]));
				rt->d[rt->level] = true;
				
				rt->level++;
				rt->r[rt->level].id = rt->r[rt->level-1].left;
			}
			else
			{
				rt->r[rt->level].right = slp->firstPointForLabel_Y(rt->r[rt->level].id, &(rt->p[rt->level]));
				rt->d[rt->level] = false;
				
				rt->level++;
				rt->r[rt->level].id = rt->r[rt->level-1].right;
			}

			rt->r[rt->level].len = lenrules->access(rt->r[rt->level].id);
		}

		return cmp;
	}

	int 
	RePairSLPIndex::expandRuleAndCompareDirectInG(PSpattern *suffix, FullQuery *q)
	{
		RuleTrav *rt = q->rt;
		int cmp = 0;
		bool found = false;

		while (true)
		{
			// The current rule is fully sampled
			if (rt->r[rt->level].len <= grammarSamp->getMaxLength())
			{
				grammarSamp->extract(rt->r[rt->level].id, rt->xseq);
				int remaining = rt->discard - rt->read;
				rt->read += rt->r[rt->level].len;

				for (; remaining<(int)rt->r[rt->level].len; remaining++)
				{
					if (rt->xseq[remaining] != suffix->spattern[rt->discard])
					{
						cmp = rt->xseq[remaining] - suffix->spattern[rt->discard];
						break;
					}
					else
					{
						rt->discard++;
						if (rt->discard == suffix->length)
						{
							cmp = 0;
							found = true;
							break;
						}
					}
				}

				if ((remaining < (int)rt->r[rt->level].len) || (found == true)) break;

				if (rt->read == rt->r[0].len)
				{
					cmp = -rt->xseq[rt->r[rt->level].len-1];
					break;					
				}
				
				do rt->level--; while (rt->d[rt->level]);

				rt->r[rt->level].right = slp->firstPointForLabel_Y(rt->r[rt->level].id, rt->p[rt->level]);
				rt->d[rt->level] = true;
				
				rt->level++;
				rt->r[rt->level].id = rt->r[rt->level-1].right;
			}
			else
			{
				rt->r[rt->level].left = permutation->pi(slp->firstPointForLabel_X(rt->r[rt->level].id, &(rt->p[rt->level])));
				rt->d[rt->level] = false;
				
				rt->level++;
				rt->r[rt->level].id = rt->r[rt->level-1].left;
			}

			rt->r[rt->level].len = lenrules->access(rt->r[rt->level].id);
		}

		return cmp;
	}

	int 
	RePairSLPIndex::expandRuleAndCompareDirectInS(PSpattern *suffix, uint pos, uint jumped, FullQuery *q)
	{
		RuleTrav *rt = q->rt;
		int cmp = 0;
		bool found = false;

		while (true)
		{
			// The current rule is fully sampled
			if (rt->r[rt->level].len <= grammarSamp->getMaxLength())
			{
				grammarSamp->extract(rt->r[rt->level].id, rt->xseq);
				int remaining = rt->discard - rt->read;
				int xread = rt->r[rt->level].len;

				for (; remaining<(int)rt->r[rt->level].len; remaining++)
				{
					if (rt->xseq[remaining] != suffix->spattern[rt->discard])
					{
						cmp = rt->xseq[remaining] - suffix->spattern[rt->discard];
						break;
					}
					else
					{
						rt->discard++;
						if (rt->discard == suffix->length)
						{
							cmp = 0;
							found = true;
							break;
						}
					}
				}

				if ((remaining < (int)rt->r[rt->level].len) || (found == true)) break;

				rt->read = rt->discard;
		
				if ((jumped+xread) == rt->r[0].len)
				{
					// Extracting the next position in the compressed sequence
					xread = rt->read;
					jumped = 0;
					uint xdiscard = rt->discard;

					pos++;
					if (pos == (seqR->getNRows()-1))
					{
						// The previous was the final position in the compressed sequence
						cmp = 1;
						break;
					}
					uint id = permutation->pi(seqR->retrieveColumnPerLabelNoBitSeq(pos+2));
					rt->ResetRT(id, lenrules->access(id));

					rt->read = xread;
					rt->discard = xdiscard;
				}
				else
				{		
					jumped += xread;
					do rt->level--; while (rt->d[rt->level]);

					rt->r[rt->level].right = slp->firstPointForLabel_Y(rt->r[rt->level].id, rt->p[rt->level]);
					rt->d[rt->level] = true;
				
					rt->level++;
					rt->r[rt->level].id = rt->r[rt->level-1].right;
				}
			}
			else
			{
				rt->r[rt->level].left = permutation->pi(slp->firstPointForLabel_X(rt->r[rt->level].id, &(rt->p[rt->level])));
				rt->d[rt->level] = false;
				
				rt->level++;
				rt->r[rt->level].id = rt->r[rt->level-1].left;
			}

			rt->r[rt->level].len = lenrules->access(rt->r[rt->level].id);
		}

		return cmp;
	}

	RePairSLPIndex::~RePairSLPIndex()
	{
		delete C; delete [] Y;
		delete lenrules;
		delete permutation;
		delete slp;
		
		delete seqB;
		delete seqR;
		
		if (mapper != NULL) delete mapper;

		delete grammarSamp;
		delete reverseSamp;
		delete sequenceSamp;
	}
};

