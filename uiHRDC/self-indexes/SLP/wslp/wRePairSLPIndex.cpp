/* wRePairSLPIndex.cpp
 * Copyright (C) 2011, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * SLP-based index on word-based Re-Pair compression. It is based on:
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
#include "wRePairSLPIndex.h"

wRePairSLPIndex::wRePairSLPIndex()
{
	mapper = NULL;
	doc_array = NULL;
}

int 
wRePairSLPIndex::build(uchar *text, uint length, uint *docs, uint ndocs, char *filename)
{
	this->filename = string(filename);
	this->ndocs = ndocs;
	this->textlength = length;

	// Obtaining a word-based representation
	int *itext;
	wm = new WordsManager();
	wm->parse(text, length, filename, &itext, &textlength, docs, ndocs);
		
	// Performing word-based RePair
	int * ctext; uint clength;
	// Variables for the dictionary rules
	Tdiccarray *dicc; uint cdicc;		
		
	IRePair::compress(itext, textlength, &ctext, &clength, &terminals, &cdicc, &dicc);
	uint rules = terminals+cdicc;

	uint maxlenrule = 1;
	vector<Rule> grammar(rules+1);
		
	for (uint i=1;i<=terminals;i++) 
	{
		grammar[i] = Rule(i, -1, i, 1);

		grammar[i].sequence = new uint[1];
		grammar[i].sequence[0] = i;
	}
			
	for (uint i=0;i<cdicc; i++)
	{
		uint rl = dicc->rules[i].rule.left;
		uint rr = dicc->rules[i].rule.right;

		grammar[i+terminals+1] = Rule(rl, rr, i+terminals+1, dicc->rules[i].len);

		grammar[i+terminals+1].sequence = new uint[grammar[i+terminals+1].len];
		memcpy(grammar[i+terminals+1].sequence, grammar[rl].sequence, sizeof(uint)*grammar[rl].len);
		memcpy(grammar[i+terminals+1].sequence+grammar[rl].len, grammar[rr].sequence, sizeof(uint)*grammar[rr].len);

		if (grammar[i+terminals+1].len > maxlenrule) maxlenrule = grammar[i+terminals+1].len;
	}


	Dictionary::destroyDicc(dicc);
	sort (grammar.begin()+1, grammar.end(), sorting);

	// *************************************************************
	// Building an 'inverted' grammar to update dictionary and 
	// sequence with the new ordering. 
	// The lenrules structure (on DAC) and the bitsequences C and Y 
	// are obtained in this process.
	// *************************************************************
	uint *grammar_inv = new uint[rules+1];
	uint *lengths = new uint[rules];
	BitString *bsY = new BitString(rules+1);

	for (uint i=1; i<=rules; i++)
	{			
		grammar_inv[grammar[i].id] = i;
		grammar[i].id = i;
			
		lengths[i-1] = grammar[i].len;
			
		if (grammar[i].len == 1) bsY->setBit(i,true);
	}

	lenrules = new factorization(lengths, terminals+cdicc);
	Y = new BitSequenceRRR(*bsY, 20);
		
	delete [] lengths;
	delete bsY;

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
	
	for (uint i=0; i<clength; i++) ctext[i] = grammar_inv[ctext[i]];	

	// *************************************************************
	// Reverse dictionary ordering
	// *************************************************************
	vector<Rule> grammar_rev(rules+1);
	for (uint i=1; i<=rules; i++) grammar_rev[i] = grammar[i];
	sort(grammar_rev.begin()+1, grammar_rev.end(), sortingRev);

	// *************************************************************
	// Obtaining the permutation from reverse to direct ordering
	// *************************************************************
	uint bitsperm = bits(rules+1);
	uint sizeperm = (int)ceil(double(bitsperm*(rules+1))/W);	
		
	uint *perm = new uint[sizeperm];
	for (uint i=0; i<sizeperm; i++) perm[i] = 0;
	for (uint i=1; i<=rules; i++) set_field(perm, bitsperm, i, grammar_rev[i].id);
		
	PermutationBuilder *pb = new PermutationBuilderMRRR(8, new BitSequenceBuilderRG(20));
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
	for (uint i=1;i<=rules;i++) delete [] grammar[i].sequence;

	slp = new LabeledBinaryRelation(&grammar, cdicc, rules, rules, false);

	// *************************************************************
	// Building structures for the compressed sequence
	//  - prefix/Suffix binary relation for the compressed sequence
	//    (this is finally built through the reversed ordering)
	// *************************************************************
	uint bitstext = bits(textlength);
	uint *lengthsSeq = new uint[clength];

	uint current = 0;
	for (uint i=0; i<clength; i++)
	{
		lengthsSeq[i] = lenrules->access(ctext[i]);
		current += lengthsSeq[i];
	}

	seqB = new DeltaCodes(lengthsSeq, clength, DELTA_SAMPLING);
	delete [] lengthsSeq;
		
	// Building the text suffix array (loading the integer sequence again)
	SuffixArray *sa = new SuffixArray();
	int *sa_ord = (int*) malloc(sizeof(int)*(textlength+1));
	sa->suffixsort(itext, sa_ord, textlength-1, terminals+1, 1);
	delete [] itext; delete sa;

	// Retrieving "valid" suffixes for start positions in the 
	// compressed sequence
	int * suffixes = new int[clength];
	uint csuff = 1;
		
	for (uint i=0; i<textlength; i++)
	{
		if (sa_ord[i] != 0)
		{
			// The first position in the compressed sequence is not
			// considered as suffix for the binary relation
			
			// Binary search
			uint l = 0, r = clength-1, c;
			uint val;
			while (l <= r)
			{
				c = (l+r)/2;
				val = seqB->select(c)+1;
				
				if (val < sa_ord[i]) l = c+1;
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
	for (uint i=1; i<clength; i++) suffixes_inv[suffixes[i]] = i+1;
	delete [] suffixes;

	vector<Rule> seqSLP(clength);
	
	// Inserts the final position with the first suffix: $
	seqSLP[0] = Rule(1, permutation->revpi(ctext[clength-1]), clength+1, 0);

	for (uint i=1; i<clength; i++)
		seqSLP[i] = Rule(suffixes_inv[seqB->select(i)+1], permutation->revpi(ctext[i-1]), i+1, 0);

	delete [] suffixes_inv; free(ctext); 

	seqR = new LabeledBinaryRelation();
	seqR->buildNoBitSeq(&seqSLP, clength, clength+1, rules, false);	
}
	
int 
wRePairSLPIndex::save()
{
	// Opening the ouptut stream
	ofstream output(filename.c_str());
		
	if (output.good())
	{
		// Length value
		saveValue(output,textlength);
		saveValue(output,terminals);

		// Words dictionary
		wm->save(output);

		// Saving bitsequence which marks terminal rules in the SLP
		Y->save(output);
		// Saving lenrules DAC representation
		lenrules->save(output);			
		// Saving permutation from reverse to direct rule ordering
		permutation->save(output);
		// Saving the SLP for the RePair grammar
		slp->save(output);
			
		// Saving bitsequence which mark start positions in the 
		// original sequence
		seqB->save(output);
		// Saving the binary relation for prefixes/suffixes in the 
		// compressed sequence
		seqR->save(output);

		output.close();

		return 0;
	}
	else
	{
		return -1;
	}
}
			
int 
wRePairSLPIndex::load(char *filename, wRePairSLPIndex **rpi)
{
	// Opening the input stream
	ifstream input(filename);
		
	if (input.good())
	{
		*rpi = new wRePairSLPIndex();

		(*rpi)->filename = string(filename);

		(*rpi)->textlength = loadValue<uint>(input);
		(*rpi)->terminals = loadValue<uint>(input);

		// Word vocabulary
		(*rpi)->wm = WordsManager::load(input);

		// Loading all structures in the index
		(*rpi)->Y = BitSequence::load(input);
		(*rpi)->lenrules = factorization::load(input);
		(*rpi)->permutation = Permutation::load(input);
		(*rpi)->slp = LabeledBinaryRelation::load(input);

		(*rpi)->seqB = DeltaCodes::load(input);
		(*rpi)->seqlength = ((*rpi)->seqB)->elements();
		(*rpi)->seqR = LabeledBinaryRelation::load(input);

		// Opening the docs file
//@@fari@@//		ifstream fdocs((string(filename)+".DOCBOUNDARIES").c_str());
		ifstream fdocs((string(filename)+".docs").c_str());
		fdocs.seekg(0,ios_base::end);
		(*rpi)->ndocs = fdocs.tellg()/sizeof(int);
		fdocs.seekg(0,ios_base::beg);

		(*rpi)->doc_array = loadValue<uint>(fdocs, (*rpi)->ndocs);
		fdocs.close();		

		return 1;
	}
	else
	{
		return -1;
	}
}

unsigned int 
wRePairSLPIndex::size()
{
	uint size = wm->size()+Y->getSize()+lenrules->getSize()+permutation->getSize()+slp->getSize()+seqB->size()+seqR->getSize()+sizeof(wRePairSLPIndex);
	size += ndocs*sizeof(uint);	
		
	return size;
}

void
wRePairSLPIndex::TrackOccsForCount(uint *numocc, uint j, uint *occsL, Rule **labels, uint *rulesize)
{
	for (; j<*occsL; j++)
	{	
		// Counting rule ocurrences in the compressed sequence
		*numocc += seqR->nRowsPerColumnNoBitSeq(permutation->revpi((*labels)[j].id));
			
		// Track rules using primary ocurrences
		if ((*occsL+(2*slp->getNPoints())) >= *rulesize)
		{
			(*rulesize)*=2;
			*labels = (Rule*)realloc(*labels, sizeof(Rule)*(*rulesize));
		}

		*occsL += slp->labelsPerRow(permutation->revpi((*labels)[j].id), (*labels)+(*occsL), 0, 0);
		*occsL += slp->labelsPerColumn((*labels)[j].id, (*labels)+(*occsL), 0, 0);
	}
}

void
wRePairSLPIndex::TrackOccsForLocate(uint *numocc, uint j, uint *occsL, Rule **labels, uint *occsSize, uint **occs, uint *rulesize)
{
	for (; j<*occsL; j++)
	{
		// Updating offset value (if necessary)
		if ((*labels)[j].right < 0)
			(*labels)[j].len = lenrules->access((*labels)[j].id) + (*labels)[j].right + (*labels)[j].len;

		// Counting and locating rule ocurrences in the compressed sequence
		uint occsRule = seqR->nRowsPerColumnNoBitSeq(permutation->revpi((*labels)[j].id));

		if (occsRule > 0)
		{							
			if (*numocc+occsRule > *occsSize)
			{
				(*occsSize)*=2;
				*occs = (uint*)realloc(*occs, sizeof(uint)*(*occsSize));				
			}

			for (uint i=1; i<=occsRule; i++)
			{
				uint posseq = seqR->labelForColumnNoBitSeq(permutation->revpi((*labels)[j].id), i)-2;
				uint pos = seqB->select(posseq)+(*labels)[j].len+2;
				((*occs)[*numocc+i-1]) = pos;
			}

			*numocc += occsRule;
		}

		// Track rules using primary ocurrences
		if ((*occsL+(2*slp->getNPoints())) >= *rulesize)
		{
			(*rulesize)*=2;
			*labels = (Rule*)realloc(*labels, sizeof(Rule)*(*rulesize));
		}

		*occsL += slp->labelsPerRow(permutation->revpi((*labels)[j].id), (*labels)+(*occsL), lenrules->access((*labels)[j].id), (*labels)[j].len);
		*occsL += slp->labelsPerColumn((*labels)[j].id, (*labels)+(*occsL), lenrules->access((*labels)[j].id), (*labels)[j].len);
	}
}
				
unsigned int 
wRePairSLPIndex::count(uchar *pattern, uint length)
{
	uint *uintPattern, words;
	wm->parsePattern(pattern, length, &uintPattern, &words);

	uint numocc = 0;

	uint occsL = 0;
	uint rulesize = 4*slp->getNPoints();
	Rule *labels = (Rule*)malloc(sizeof(Rule)*rulesize);

	if (words > 1)
	{
		RangeQuery *q = new RangeQuery[words-1];
		int nprefixes = getPrefixesInSLP(uintPattern, words, q);

		PSpattern *suffixes = new PSpattern[words-1];
		int nsuffixes = getSuffixesInSLP(uintPattern, words, q, suffixes);

		for (uint i=0; i<words-1; i++)
		{
			if ((q[i].rows.left != NOTFOUND) && (q[i].columns.left != NOTFOUND))
			{
				if ((occsL+slp->getNPoints()) >= rulesize)
				{
					rulesize*=2;
					labels = (Rule*)realloc(labels, sizeof(Rule)*rulesize);
				}

				uint j = occsL;
				occsL += slp->labelsInRange(q[i].rows.left, q[i].rows.right, q[i].columns.left, q[i].columns.right, labels+occsL);

				TrackOccsForCount(&numocc, j, &occsL, &labels, &rulesize);
			}
		}

		free(labels);
		nsuffixes = getSuffixesInSequence(uintPattern, words, q, suffixes, nsuffixes);
	
		uint occsS = 0;
		rulesize = slp->getNPoints();
		labels = (Rule*)malloc(sizeof(Rule)*rulesize);

		for (uint i=0; i<words-1; i++)
		{
			if ((q[i].rows.left != NOTFOUND) && (q[i].columns.left != NOTFOUND))
			{
				if ((occsS+slp->getNPoints()) >= rulesize)
				{
					rulesize*=2;
					labels = (Rule*)realloc(labels, sizeof(Rule)*rulesize);
				}

				occsS += seqR->labelsInRangeNoBitSeq(q[i].columns.left, q[i].columns.right, q[i].rows.left, q[i].rows.right, labels+occsS);
			}
		}
		
		numocc += occsS;

		free(labels);
		delete [] suffixes;
		delete [] q;
	}
	else
	{
		occsL = 1;
		labels[0].id = Y->select1(uintPattern[0]);
		TrackOccsForCount(&numocc, 0, &occsL, &labels, &rulesize);
		free(labels);
	}
		

	free(uintPattern);
	return numocc;
}

int 
wRePairSLPIndex::locate(uchar *pattern, uint length, uint **docs, uint *numocc)
{
	uint occsSize = LOCATE_SIZE;
	uint *occs = (uint*)malloc(sizeof(uint)*occsSize);

	uint *uintPattern, words;
	wm->parsePattern(pattern, length, &uintPattern, &words);
		
	*numocc = 0;
	uint occsL = 0;
	uint rulesize = 4*slp->getNPoints();
	Rule *labels = (Rule*)malloc(sizeof(Rule)*rulesize);

	if (words > 1)
	{
		RangeQuery *q = new RangeQuery[words-1];
		int nprefixes = getPrefixesInSLP(uintPattern, words, q);
		PSpattern *suffixes = new PSpattern[words-1];
		int nsuffixes = getSuffixesInSLP(uintPattern, words, q, suffixes);

		for (uint i=0; i<words-1; i++)
		{
			if ((q[i].rows.left != NOTFOUND) && (q[i].columns.left != NOTFOUND))
			{
				if ((occsL+slp->getNPoints()) >= rulesize)
				{
					rulesize*=2;
					labels = (Rule*)realloc(labels, sizeof(Rule)*rulesize);
				}

				uint primary = slp->labelsInRange(q[i].rows.left, q[i].rows.right, q[i].columns.left, q[i].columns.right, labels+occsL);
				
				// Obtaining the primary ocurrence offset within the rule
				for (uint j=occsL; j<occsL+primary; j++)
					labels[j].len = (lenrules->access(labels[j].id) - lenrules->access(labels[j].right))-(i+1);
				
				uint j = occsL;
				occsL += primary;

				TrackOccsForLocate(numocc, j, &occsL, &labels, &occsSize, &occs, &rulesize);
			}
		}
		
		free(labels);			
		nsuffixes = getSuffixesInSequence(uintPattern, words, q, suffixes, nsuffixes);
		
		uint occsS = 0;
		rulesize = slp->getNPoints();
		labels = (Rule*)malloc(sizeof(Rule)*rulesize);

		for (uint i=0; i<words-1; i++)
		{
			if ((q[i].rows.left != NOTFOUND) && (q[i].columns.left != NOTFOUND))
			{
				if ((occsS+slp->getNPoints()) >= rulesize)
				{
					rulesize*=2;
					labels = (Rule*)realloc(labels, sizeof(Rule)*rulesize);
				}

				uint prev = occsS;
				occsS += seqR->labelsInRangeNoBitSeq(q[i].columns.left, q[i].columns.right, q[i].rows.left, q[i].rows.right, labels+occsS);
			
				if (*numocc+occsS > occsSize)
				{
					occsSize*=2;
					occs = (uint*)realloc(occs, sizeof(uint)*occsSize);
				}
			
				for (uint j=prev; j<occsS; j++)
				{
					uint pos = seqB->select(labels[j].id-1)+1-i;
					occs[*numocc+occsS-1] = pos;
				}
			}
		}
	
		*numocc += occsS;
	
		free(labels);
		delete [] suffixes;
		delete [] q;
	}
	else
	{
		occsL = 1;

		labels[0].id = Y->select1(uintPattern[0]);
		labels[0].right = 0;
		TrackOccsForLocate(numocc, 0, &occsL, &labels, &occsSize, &occs, &rulesize);
		free(labels);
	}

	free(uintPattern);
	*docs = doc_offset_exp(doc_array, ndocs, occs, *numocc);
	free(occs);
	
	return 0;
}

int 
wRePairSLPIndex::extract(uint from, uint to, uchar **snippet)
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
		uint *snippet_uint = new uint[snippet_length];
		uint count = 0;
		uint length = 0;

		Point p = seqR->firstPointForLabelNoBitSeq(c+2);
		uint rule = permutation->pi(p.right);

		uint len = lenrules->access(rule);
		uint position = from-beginR;
			
		if (position > 0)
		{
			// Extracts from an internal position
			if ((len - position) > snippet_length)
			{
				// The snippet is fully stored within the current rule			
				extractCharsDirect(rule, len, position, snippet_length, snippet_uint);

				uint resultlen = getWords(snippet, snippet_uint, snippet_length);
				snippet_length = resultlen;

				return snippet_length;
			}
			else
			{
				// Extracts from RIGHT to LEFT
				extractCharsReverse(rule, len, 0, len-position, snippet_uint);
				count += len-position;
			}				
				
			c++;
		}
			
		while (count < snippet_length)
		{					
			// Extracting next rule
			p = seqR->firstPointForLabelNoBitSeq(c+2);
			rule = permutation->pi(p.right);
			len = lenrules->access(rule);
				
			if (len >= (snippet_length-count)) break;
				
			// Extract the full rule
			expandRule(rule, len, snippet_uint+count);
				
			count += len;
			c++;
		}

		if (count != snippet_length) extractCharsDirect(rule, len, 0, snippet_length-count, snippet_uint+count);

		// Snippet filling: all words are retrieved from the 'wm'. 
		uint resultlen = getWords(snippet, snippet_uint, snippet_length);
		snippet_length = resultlen;

		return snippet_length;
	}
	else
	{
		return -1;
	}
}
	
unsigned int 
wRePairSLPIndex::length()
{
	return textlength;
}		

uint
wRePairSLPIndex::getWords(uchar **snippet, uint *snippet_uint, uint length)
{
	uint reserved = 1048576;
	uint used = 0;
	*snippet = (uchar*) malloc(reserved*sizeof(uchar));

	bool isword = false;

	for (uint i=0; i<length; i++) 
	{
		used += wm->addWord(snippet_uint[i], *snippet+used, &isword);

		if (used + 2*wm->getAvgLen() >= reserved)
		{
			reserved*=2;
			*snippet = (uchar*) realloc (*snippet, reserved*sizeof(uchar));
		}
	}

	(*snippet)[used] = '\0';
	delete [] snippet_uint;

	return used;
}

int
wRePairSLPIndex::getPrefixesInSLP(uint *pattern, uint length, RangeQuery *q)
{
	PSpattern *prefixes = new PSpattern[length-1];
	uint *rpattern = new uint[length+1];
	
	for (uint i=0; i<length; i++) rpattern[i] = pattern[length-i-1];
		
	for (uint i=0; i<length-1; i++)
	{
		prefixes[i].pattern = rpattern+i+1;
		prefixes[i].length = length-i-1;
	}

	sort (prefixes, prefixes+length-1, sortingP);

	// Initialishing with a previous symbol...
	uint first = prefixes[0].pattern[0]-1;
	uint left, right;
	int prev = 1, curr;
	for (curr=0; curr<length-1; curr++)
	{
		if (prefixes[curr].pattern[0] != first)
		{				
			if (curr-prev > 0) extractAndCompareReverse(prefixes+prev, curr-prev);
			prev = curr;
				
			if (prefixes[curr].length == 1) prev++;
				
			first = prefixes[curr].pattern[0];
			uint pos = first;
				
			if (pos == 1)
			{
				left = 1;
				right = permutation->revpi(Y->select1(pos+1))-1;
			}
			else
			{
				left = permutation->revpi(Y->select1(pos));
				if (pos == terminals) right = slp->getNColumns();
				else right = permutation->revpi(Y->select1(pos+1))-1;
			}
			
			prefixes[curr].left = left;
			prefixes[curr].cleft = 1;
			prefixes[curr].right = right; prefixes[curr].cright = 1;
			prefixes[curr].lcp = 0;
		}
		else
		{
			prefixes[curr].left = left; prefixes[curr].cleft = 1;
			prefixes[curr].right = right; prefixes[curr].cright = 1;
			prefixes[curr].lcp = lcp(prefixes[curr-1].pattern, prefixes[curr].pattern, prefixes[curr-1].length, prefixes[curr].length);
		}
	}
		
	// Last prefixes group
	if (curr-prev > 0) extractAndCompareReverse(prefixes+prev, curr-prev);
		
	for (uint i=0; i<length-1; i++)
	{
		q[prefixes[i].length-1].rows.left = prefixes[i].left;
		q[prefixes[i].length-1].rows.right = prefixes[i].right;
	}		

	delete [] prefixes;
	delete [] rpattern;
		
	return length-1;
}

int
wRePairSLPIndex::getSuffixesInSLP(uint *pattern, uint length, RangeQuery *q, PSpattern *suffixes)
{						
	uint lsuff = 0;
		
	for (uint i=0; i<length-1; i++)
	{
		if (q[i].rows.left != NOTFOUND)
		{
			suffixes[lsuff].pattern = pattern+i+1;
			suffixes[lsuff].length = length-i-1;
				
			lsuff++;
		}
	}

	sort (suffixes, suffixes+lsuff, sortingP);
		
	// Initialishing with a previous symbol...
	uint first = suffixes[0].pattern[0]-1;
	uint left, right;
	int prev = 1, curr;
		
	for (curr=0; curr<lsuff; curr++)
	{
		if (suffixes[curr].pattern[0] != first)
		{				
			if (curr-prev > 0) extractAndCompareDirect(suffixes+prev, curr-prev);
			prev = curr;
			
			if (suffixes[curr].length == 1) prev++;
			
			first = suffixes[curr].pattern[0];
			uint pos = first;
				
			if (pos == 1)
			{
				left = 1;
				right = Y->select1(pos+1)-1;
			}
			else
			{
				left = Y->select1(pos);
				if (pos == terminals) right = slp->getNColumns();
				else right = Y->select1(pos+1)-1;
			}
			
			suffixes[curr].left = left;
			suffixes[curr].cleft = 1;
			suffixes[curr].right = right; suffixes[curr].cright = 1;
			suffixes[curr].lcp = 0;
		}
		else
		{
			suffixes[curr].left = left; suffixes[curr].cleft = 1;
			suffixes[curr].right = right; suffixes[curr].cright = 1;
			suffixes[curr].lcp = lcp(suffixes[curr-1].pattern, suffixes[curr].pattern, suffixes[curr-1].length, suffixes[curr].length);
		}
	}
	
	// Last suffixes group
	if (curr-prev > 0) extractAndCompareDirect(suffixes+prev, curr-prev);
	
	for (uint i=0; i<lsuff; i++)
	{
		q[length-suffixes[i].length-1].columns.left = suffixes[i].left;
		q[length-suffixes[i].length-1].columns.right = suffixes[i].right;
	}

	return lsuff;
}


void 
wRePairSLPIndex::jumpCharsDirect(uint discard, Rule *r, uint *p, bool *d, int *level)
{
	uint jump = 0;
		
	while (true)
	{
		r[*level].right = slp->firstPointForLabel_Y(r[*level].id, &(p[*level]));
		d[*level] = true;
				
		(*level)++;			
		r[*level].len = lenrules->access(r[(*level)-1].right);
		uint llen = r[(*level)-1].len-r[*level].len;
			
		if (jump+llen <= discard)
		{
			jump += llen;
				
			// Traversing the right child
			r[*level].id = r[(*level)-1].right;
				
			if (r[*level].len == 1) break;								
		}
		else
		{
			// Traversing the left child
			r[(*level)-1].left = permutation->pi(slp->firstPointForLabel_X(r[(*level)-1].id, p[(*level)-1]));
			r[*level].id = r[(*level)-1].left;
			r[*level].len = llen;
			d[(*level)-1] = false;
				
			if (r[*level].len == 1) break;				
		}
	}
}

void 
wRePairSLPIndex::extractCharsDirect(uint rule, uint len, uint discard, uint chars, uint *snippet)
{
	Rule *r = new Rule[PROOF];		// Stores traversed rules
	uint *p = new uint[PROOF];		// Stores rule position in the SLP
	bool *d = new bool[PROOF];		// Stores level state
		
	int level = 0;
	uint read = 0;

	r[level].id = rule;
	r[level].len = len;
		
	if (discard > 0) jumpCharsDirect(discard, r, p, d, &level);
	
	// Extracting the rule 
	while (true)
	{			
		if (r[level].len > 1)
		{
			r[level].left = permutation->pi(slp->firstPointForLabel_X(r[level].id, &(p[level])));
			d[level] = false;
			
			level++;
			r[level].id = r[level-1].left;
		}
		else
		{				
			snippet[read] = Y->rank1(r[level].id);			
			read++;
			if (read == chars) break;
			
			do level--; while (d[level]);
			
			r[level].right = slp->firstPointForLabel_Y(r[level].id, p[level]);
			d[level] = true;
			
			level++;
			r[level].id = r[level-1].right;
		}
			
		r[level].len = lenrules->access(r[level].id);
	}
	
	delete [] r;
	delete [] p;
	delete [] d;
}

void 
wRePairSLPIndex::extractAndCompareDirect(PSpattern *suffixes, uint size)
{
	uint current = 0;
	if (suffixes[current].left > suffixes[current].right)
	{
		for (; current < size; current++)
		{
			// The pattern is not in the grammar
			suffixes[current].left = NOTFOUND;
			suffixes[current].right = NOTFOUND;
		}
		
		return;
	}
		
	Rule *r = new Rule[PROOF];		
	uint *p = new uint[PROOF];		
	bool *d = new bool[PROOF];				
		
	int level = 0;

	uint pivot = (suffixes[current].left + suffixes[current].right)/2;
	r[level].id = pivot;
	r[level].len = lenrules->access(r[level].id);
		
	uint discard = suffixes[current].cleft;
	if (suffixes[current].cright < discard) discard = suffixes[current].cright;
		
	int less = -1, equal = -1, greater = -1, pless = -1;
		
	if (r[level].len > discard)
	{
		jumpCharsDirect(discard, r, p, d, &level);
			
		uint c;
		uint read = discard;
			
		// Extracting the rule and comparing with respect to the suffixes
		while (true)
		{			
			if (r[level].len > 1)
			{
				r[level].left = permutation->pi(slp->firstPointForLabel_X(r[level].id, &(p[level])));
				d[level] = false;
				
				level++;
				r[level].id = r[level-1].left;
			}
			else
			{				
				c = Y->rank1(r[level].id);
					
				if (c < suffixes[current].pattern[read])
				{			
					// The current rule is previous to all patterns from 'current'
					suffixes[current].left = pivot+1;
					suffixes[current].cleft = read;
					greater = size-1;
					break;
				}
				else
				{
					if (c > suffixes[current].pattern[read])
					{
						suffixes[current].right = pivot-1;
						suffixes[current].cright = read;
							
						do
						{
							suffixes[current].right = pivot-1;
							suffixes[current].cright = read;
							current++;
						}
						while ((current < size) && (suffixes[current].lcp > read));
						
						less = current-1;
						
						if (current == size) break;
						else
						{							
							if (suffixes[current].lcp < read)
							{
								suffixes[current].left = pivot+1;
								suffixes[current].cleft = suffixes[current].lcp;
								greater = size-1;
								break;
							}
							else
							{
								while ((current < size) && (c > suffixes[current].pattern[read]))
								{
									suffixes[current].right = pivot-1;
									suffixes[current].cright = read;
									current++;
								}
								
								less = current-1;
								
								if (current == size) break;
								
								if (c < suffixes[current].pattern[read])
								{
									suffixes[current].left = pivot+1;
									suffixes[current].cleft = suffixes[current].lcp;
									greater = size-1;
									break;
								}
							}
						}
					}
				}
				
				read++;
				
				if (read == suffixes[current].length) 
				{
					if (less != -1) pless = less;
					
					equal = current;
					current++;
					
					if (current == size) break;
					
					if (suffixes[current].lcp < read)
					{						
						// The current rule is previous to all patterns from 'current'
						suffixes[current].left = pivot+1;
						suffixes[current].cleft = suffixes[current].lcp;
						greater = size-1;
						break;
					}
				}

				if (read == r[0].len)
				{
					// All rule has been extracted: it is previous...
					suffixes[current].left = pivot+1;
					suffixes[current].cleft = read;
					greater = size-1;
					break;					
				}

				do level--; while (d[level]);
				
				r[level].right = slp->firstPointForLabel_Y(r[level].id, p[level]);
				d[level] = true;
				
				level++;
				r[level].id = r[level-1].right;
			}
				
			r[level].len = lenrules->access(r[level].id);
		}
	}
	else
	{
		greater = 0;
		suffixes[current].left = pivot+1;
		suffixes[current].cleft = r[level].len;
	}
	
	delete [] r;
	delete [] p;
	delete [] d;
	
	if (less >= 0)
	{
		if (equal >= 0)
		{								
			if (greater >= 0)
			{
				if ((equal < size-1) && (suffixes[equal].length == suffixes[equal+1].lcp))
				{	
					if (less == pless)
					{
						extractAndCompareDirect(suffixes, less+1);
						obtainRangeDirect(pivot, suffixes+less+1, equal-less);
						
						suffixes[equal+1].left = suffixes[equal].left;
						suffixes[equal+1].cleft = suffixes[equal+1].lcp;
						suffixes[equal+1].right = suffixes[equal].right;
						suffixes[equal+1].cright = suffixes[equal+1].lcp;
						
						extractAndCompareDirect(suffixes+equal+1, greater-equal);
					}
					else
					{
						if (pless > 0)
						{
							extractAndCompareDirect(suffixes, pless+1);
						}
						obtainRangeDirect(pivot, suffixes+pless+1, equal-pless);
						
						suffixes[equal+1].left = suffixes[equal].left;
						suffixes[equal+1].cleft = suffixes[equal+1].lcp;
						suffixes[equal+1].right = suffixes[equal].right;
						suffixes[equal+1].cright = suffixes[equal+1].lcp;
						
						extractAndCompareDirect(suffixes+equal+1, less-equal);						
						extractAndCompareDirect(suffixes+less+1, greater-less);
					}
				}
				else
				{					
					extractAndCompareDirect(suffixes, less+1);
					obtainRangeDirect(pivot, suffixes+less+1, equal-less);
					extractAndCompareDirect(suffixes+equal+1, greater-equal);
				}
			}
			else
			{				
				if ((equal < size-1) && (suffixes[equal].length == suffixes[equal+1].lcp))
				{
					if ((less == pless) || (less == -1))
					{
						extractAndCompareDirect(suffixes, less+1);
						obtainRangeDirect(pivot, suffixes+less+1, equal-less);							
					}
					else
					{
						if (pless >= 0)
						{
							extractAndCompareDirect(suffixes, pless+1);
						}
						obtainRangeDirect(pivot, suffixes+pless+1, equal-pless);

						suffixes[equal+1].left = suffixes[equal].left;
						suffixes[equal+1].cleft = suffixes[equal+1].lcp;
						suffixes[equal+1].right = suffixes[equal].right;
						suffixes[equal+1].cright = suffixes[equal+1].lcp;

						extractAndCompareDirect(suffixes+equal+1, less-equal);													
					}
				}
				else
				{
					extractAndCompareDirect(suffixes, less+1);
					obtainRangeDirect(pivot, suffixes+less+1, equal-less);
				}
			}
		}
		else
		{
			if (greater >= 0)
			{
				extractAndCompareDirect(suffixes, less+1);
				extractAndCompareDirect(suffixes+less+1, greater-less);
			}
			else
			{
				extractAndCompareDirect(suffixes, size);
			}
		}
	}
	else
	{
		if (equal >= 0)
		{
			if (greater >= 0)
			{
				obtainRangeDirect(pivot, suffixes, equal+1);
				extractAndCompareDirect(suffixes+equal+1, greater-equal);
			}
			else
			{
				obtainRangeDirect(pivot, suffixes, size);
			}
		}
		else
		{
			extractAndCompareDirect(suffixes, size);
		}			
	}
}

int 
wRePairSLPIndex::expandRuleAndCompareDirect(uint *pattern, uint length, uint *read, Rule *r, uint *p, bool *d, int level)
{
	uint c;
		
	while (true)
	{					
		if (r[level].len > 1)
		{
			r[level].left = permutation->pi(slp->firstPointForLabel_X(r[level].id, &(p[level])));
			d[level] = false;
					
			level++;
			r[level].id = r[level-1].left;
		}
		else
		{				
			c = Y->rank1(r[level].id);

			if (c != pattern[*read]) return (c - pattern[*read]);
			(*read)++; 					
			if ((*read) == length) return 0;
			if ((*read) == r[0].len) return (-pattern[*read]);
						
			do level--; while (d[level]);
						
			r[level].right = slp->firstPointForLabel_Y(r[level].id, p[level]);
			d[level] = true;
						
			level++;
			r[level].id = r[level-1].right;
		}
				
		r[level].len = lenrules->access(r[level].id);
	}
}

void 
wRePairSLPIndex::obtainRangeDirect(uint rule, PSpattern *suffixes, uint size)
{
	uint rl, rr, rc;
		
	Rule *r = new Rule[PROOF];		
	uint *p = new uint[PROOF];		
	bool *d = new bool[PROOF];
		
	int level;
	
	for (uint current=0; current<size; current++)
	{
		// Get LEFT limit
		rl = suffixes[0].left; rr = rule;
						
		do
		{
			level = 0;
			rc = (rl+rr)/2;
		
			r[level].id = rc;
			r[level].len = lenrules->access(r[level].id);

			if (r[level].len > suffixes[0].cleft)
			{
				jumpCharsDirect(suffixes[0].cleft, r, p, d, &level);
				uint read = suffixes[0].cleft;
					
				int cmp = expandRuleAndCompareDirect(suffixes[current].pattern, suffixes[current].length, &read, r, p, d, level);
				
				if (cmp < 0)
				{
					rl = rc+1;
					suffixes[0].cleft = read;					
				}
				else
				{
					rr = rc;
				}
			}
			else
			{
				// The rule has been previously expanded because
				// 'cleft' is really its length
				rl = rc+1;					
			}
		}
		while (rl < rr);
			
		suffixes[current].left = rl;
						
		// Get RIGHT limit
		rl = rule; rr = suffixes[0].right;

		do
		{
			level = 0;
			rc = 1+((rl+rr)/2);
	
			r[level].id = rc;
			r[level].len = lenrules->access(r[level].id);
			
			if (r[level].len > suffixes[0].cright)
			{
				jumpCharsDirect(suffixes[0].cright, r, p, d, &level);
				uint read = suffixes[0].cright;

				int cmp = expandRuleAndCompareDirect(suffixes[current].pattern, suffixes[current].length, &read, r, p, d, level);
				
				if (cmp > 0)
				{
					rr = rc-1;
					suffixes[0].cleft = read;							
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
		
		suffixes[current].right = rl;
	}
		
	delete [] r;
	delete [] p;
	delete [] d;
}

void 
wRePairSLPIndex::jumpCharsReverse(uint discard, Rule *r, uint *p, bool *d, int *level)
{
	uint jump = 0;
		
	while (true)
	{
		r[*level].right = slp->firstPointForLabel_Y(r[*level].id, &(p[*level]));
		d[*level] = false;
			
		(*level)++;			
		r[*level].len = lenrules->access(r[(*level)-1].right);
			
		if (jump+r[*level].len <= discard)
		{
			jump += r[*level].len;
				
			// Traversing the left child
			r[(*level)-1].left = permutation->pi(slp->firstPointForLabel_X(r[(*level)-1].id, p[(*level)-1]));
			r[*level].id = r[(*level)-1].left;
			r[*level].len = r[(*level)-1].len-r[*level].len;
			d[(*level)-1] = true;
				
			if (r[*level].len == 1) break;								
		}			
		else
		{
			// Traversing the right child
			r[*level].id = r[(*level)-1].right;
				
			if (r[*level].len == 1) break;
		}
	}
}
	
void 
wRePairSLPIndex::extractCharsReverse(uint rule, uint len, uint discard, uint chars, uint *snippet)
{
	Rule *r = new Rule[PROOF];		// Stores traversed rules
	uint *p = new uint[PROOF];		// Stores rule position in the SLP
	bool *d = new bool[PROOF];		// Stores level state
		
	int level = 0;
	uint read = chars-1;

	r[level].id = rule;
	r[level].len = len;
		
	if (discard > 0) jumpCharsReverse(discard, r, p, d, &level);
		
	while (true)
	{			
		if (r[level].len > 1)
		{
			r[level].right = slp->firstPointForLabel_Y(r[level].id, &(p[level]));
			d[level] = false;
			
			level++;
			r[level].id = r[level-1].right;
		}
		else
		{
			snippet[read] = Y->rank1(r[level].id);
				
			if (read == 0) break;
			read--;
				
			do level--; while (d[level]);
				
			r[level].left = permutation->pi(slp->firstPointForLabel_X(r[level].id, p[level]));
			d[level] = true;
				
			level++;
			r[level].id = r[level-1].left;
		}
				
		r[level].len = lenrules->access(r[level].id);
	}
		
	delete [] r;
	delete [] p;
	delete [] d;
}

void 
wRePairSLPIndex::extractAndCompareReverse(PSpattern *prefixes, uint size)
{
	uint current = 0;
		
	if (prefixes[current].left > prefixes[current].right)
	{		
		for (; current < size; current++)
		{
			// The pattern is not in the grammar
			prefixes[current].left = NOTFOUND;
			prefixes[current].right = NOTFOUND;
		}
					
		return;
	}

	Rule *r = new Rule[PROOF];		
	uint *p = new uint[PROOF];		
	bool *d = new bool[PROOF];				

	int level = 0;
		
	uint pivot = (prefixes[current].left + prefixes[current].right)/2;
	r[level].id = permutation->pi(pivot);
	r[level].len = lenrules->access(r[level].id);

	uint discard = prefixes[current].cleft;
	if (prefixes[current].cright < discard) discard = prefixes[current].cright;
		
	int less = -1, equal = -1, greater = -1, pless = -1;	

	if (r[level].len > discard)
	{
		jumpCharsReverse(discard, r, p, d, &level);
			
		uint c;
		uint read = discard;
			
		// Extracting the rule and comparing with respect to the prefixes
		while (true)
		{		
			if (r[level].len > 1)
			{
				r[level].right = slp->firstPointForLabel_Y(r[level].id, &(p[level]));
				d[level] = false;
					
				level++;
				r[level].id = r[level-1].right;
			}
			else
			{
				c = Y->rank1(r[level].id);
					
				if (c < prefixes[current].pattern[read])
				{			
					// The current rule is previous to all patterns from 'current'
					prefixes[current].left = pivot+1;
					prefixes[current].cleft = read;
					greater = size-1;
					break;
				}
				else
				{					
					if (c > prefixes[current].pattern[read])
					{
						do
						{
							prefixes[current].right = pivot-1;
							prefixes[current].cright = read;
							current++;
						}
						while ((current < size) && (prefixes[current].lcp > read));

						less = current-1;
							
						if (current == size) break;
						else
						{
							if (prefixes[current].lcp < read)
							{
								prefixes[current].left = pivot+1;
								prefixes[current].cleft = prefixes[current].lcp;
								greater = size-1;
								break;
							}
							else
							{
								while ((current < size) && (c > prefixes[current].pattern[read]))
								{
									prefixes[current].right = pivot-1;
									prefixes[current].cright = read;
									current++;
								}
									
								less = current-1;
									
								if (current == size) break;
									
								if (c < prefixes[current].pattern[read])
								{
									prefixes[current].left = pivot+1;
									prefixes[current].cleft = prefixes[current].lcp;
									greater = size-1;
									break;
								}
							}
						}
					}
				}
					
				read++;
					
				if (read == prefixes[current].length) 
				{
					if (less != -1) pless = less;
					
					equal = current;
					current++;
						
					if (current == size) break;
						
					if (prefixes[current].lcp < read)
					{						
						// The current rule is previous to all patterns from 'current'
						prefixes[current].left = pivot+1;
						prefixes[current].cleft = prefixes[current].lcp;
						greater = size-1;
						break;
					}
				}
					
				if (read == r[0].len)
				{
					// All rule has been extracted: it is previous...
					prefixes[current].left = pivot+1;
					prefixes[current].cleft = read;
					greater = size-1;
					break;					
				}
				
				do level--; while (d[level]);
				
				r[level].left = permutation->pi(slp->firstPointForLabel_X(r[level].id, p[level]));
				d[level] = true;
					
				level++;
				r[level].id = r[level-1].left;
			}

			r[level].len = lenrules->access(r[level].id);
		}
	}
	else
	{
		greater = 0;
		prefixes[current].left = pivot+1;
		prefixes[current].cleft = r[level].len;
	}
	
	delete [] r;
	delete [] p;
	delete [] d;

	if (less >= 0)
	{
		if (equal >= 0)
		{				
			if (greater >= 0)
			{
				if ((equal < size-1) && (prefixes[equal].length == prefixes[equal+1].lcp))
				{	
					if (less == pless)
					{
						extractAndCompareReverse(prefixes, less+1);
						obtainRangeReverse(pivot, prefixes+less+1, equal-less);
						
						prefixes[equal+1].left = prefixes[equal].left;
						prefixes[equal+1].cleft = prefixes[equal+1].lcp;
						prefixes[equal+1].right = prefixes[equal].right;
						prefixes[equal+1].cright = prefixes[equal+1].lcp;
							
						extractAndCompareReverse(prefixes+equal+1, greater-equal);
					}
					else
					{
						if (pless >= 0)
						{
							extractAndCompareReverse(prefixes, pless+1);
						}
						obtainRangeReverse(pivot, prefixes+pless+1, equal-pless);
						
						prefixes[equal+1].left = prefixes[equal].left;
					prefixes[equal+1].cleft = prefixes[equal+1].lcp;
						prefixes[equal+1].right = prefixes[equal].right;
						prefixes[equal+1].cright = prefixes[equal+1].lcp;
							
						extractAndCompareReverse(prefixes+equal+1, less-equal);						
						extractAndCompareReverse(prefixes+less+1, greater-less);
					}
				}
				else
				{	
					extractAndCompareReverse(prefixes, less+1);
					obtainRangeReverse(pivot, prefixes+less+1, equal-less);
					extractAndCompareReverse(prefixes+equal+1, greater-equal);
				}
			}
			else
			{
				if ((equal < size-1) && (prefixes[equal].length == prefixes[equal+1].lcp))
				{
					if (less == pless)
					{
						extractAndCompareReverse(prefixes, less+1);
						obtainRangeReverse(pivot, prefixes+less+1, equal-less);							
					}
					else
					{
						if (pless >= 0)
						{
							extractAndCompareReverse(prefixes, pless+1);
						}
						obtainRangeReverse(pivot, prefixes+pless+1, equal-pless);

						prefixes[equal+1].left = prefixes[equal].left;
						prefixes[equal+1].cleft = prefixes[equal+1].lcp;
						prefixes[equal+1].right = prefixes[equal].right;
						prefixes[equal+1].cright = prefixes[equal+1].lcp;
							
						extractAndCompareReverse(prefixes+equal+1, less-equal);													
					}
				}
				else
				{
					extractAndCompareReverse(prefixes, less+1);
					obtainRangeReverse(pivot, prefixes+less+1, equal-less);
				}
			}
		}
		else
		{
			if (greater >= 0)
			{
				extractAndCompareReverse(prefixes, less+1);
				extractAndCompareReverse(prefixes+less+1, greater-less);
			}
			else
			{
				extractAndCompareReverse(prefixes, size);
			}
		}
	}
	else
	{
		if (equal >= 0)
		{
			if (greater >= 0)
			{
				obtainRangeReverse(pivot, prefixes, equal+1);
				extractAndCompareReverse(prefixes+equal+1, greater-equal);
			}
			else
			{
				obtainRangeReverse(pivot, prefixes, size);
			}
		}
		else
		{
			extractAndCompareReverse(prefixes, size);
		}			
	}
}

int 
wRePairSLPIndex::expandRuleAndCompareReverse(uint *pattern, uint length, uint *read, Rule *r, uint *p, bool *d, int level)
{	
	uint c;
			
	while (true)
	{			
		if (r[level].len > 1)
		{
			r[level].right = slp->firstPointForLabel_Y(r[level].id, &(p[level]));
			d[level] = false;
				
			level++;
			r[level].id = r[level-1].right;
		}
		else
		{
			c = Y->rank1(r[level].id);
				
			if (c != pattern[*read]) return (c - pattern[*read]);
			(*read)++;
			if ((*read) == length) return 0;
			if ((*read) == r[0].len) return (-pattern[*read]);
				
			do level--; while (d[level]);
				
			r[level].left = permutation->pi(slp->firstPointForLabel_X(r[level].id, p[level]));
			d[level] = true;
				
			level++;
			r[level].id = r[level-1].left;
		}
				
		r[level].len = lenrules->access(r[level].id);
	}
		
	delete [] r;
	delete [] p;
	delete [] d;
}

void 
wRePairSLPIndex::obtainRangeReverse(uint rule, PSpattern *prefixes, uint size)
{
	uint rl, rr, rc;
	uint pivot;
		
	Rule *r = new Rule[PROOF];		
	uint *p = new uint[PROOF];		
	bool *d = new bool[PROOF];
		
	int level;
		
	for (uint current=0; current<size; current++)
	{
		// Get LEFT limit
		rl = prefixes[0].left; rr = rule;

		do
		{
			level = 0;
			pivot = (rl+rr)/2;
			rc = permutation->pi(pivot);
		
			r[level].id = rc;
			r[level].len = lenrules->access(r[level].id);

			if (r[level].len > prefixes[0].cleft)
			{
				jumpCharsReverse(prefixes[0].cleft, r, p, d, &level);
				uint read = prefixes[0].cleft;
				
				int cmp = expandRuleAndCompareReverse(prefixes[current].pattern, prefixes[current].length, &read, r, p, d, level);
				
				if (cmp < 0)
				{
					rl = pivot+1;
					prefixes[0].cleft = read;							
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
		
		prefixes[current].left = rl;

		// Get RIGHT limit
		rl = rule; rr = prefixes[0].right;

		do
		{
			level = 0;
			pivot = 1+((rl+rr)/2);
			rc = permutation->pi(pivot);
	
			r[level].id = rc;
			r[level].len = lenrules->access(r[level].id);

			if (r[level].len > prefixes[0].cright)
			{
				jumpCharsReverse(prefixes[0].cright, r, p, d, &level);
				uint read = prefixes[0].cright;

				int cmp = expandRuleAndCompareReverse(prefixes[current].pattern, prefixes[current].length, &read, r, p, d, level);
				
				if (cmp > 0)
				{
					rr = pivot-1;
					prefixes[0].cleft = read;							
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
		
		prefixes[current].right = rl;
	}
		
	delete [] r;
	delete [] p;
	delete [] d;
}

int
wRePairSLPIndex::getSuffixesInSequence(uint *pattern, uint length, RangeQuery *q, PSpattern *suffixes, uint lsuff)
{								
	for (uint i=0; i<lsuff; i++)
	{
		suffixes[i].left = 2; suffixes[i].cleft = 0;
		suffixes[i].right = seqR->getNRows()-1; suffixes[i].cright = 0;
	}
			
	// SE PODRÍA CREAR UN NUEVO BITMAP QUE MARQUE LAS POSICIONES DE
	// LA SECUENCIA DE SUFIJOS EN LA QUE SE "CAMBIA" DE SIMBOLO
	// TERMINAL
	extractAndCompareDirectInSequence(suffixes, lsuff);
		
	for (uint i=0; i<lsuff; i++)
	{
		q[length-suffixes[i].length-1].columns.left = suffixes[i].left;
		q[length-suffixes[i].length-1].columns.right = suffixes[i].right;
	}

	return lsuff;
}

void
wRePairSLPIndex::extractAndCompareDirectInSequence(PSpattern *suffixes, uint size)
{
	uint current = 0;
	if (suffixes[current].left > suffixes[current].right)
	{
		for (; current < size; current++)
		{
			// The pattern is not in the compressed sequence
			suffixes[current].left = NOTFOUND;
			suffixes[current].right = NOTFOUND;
		}
		
		return;
	}
		
	Rule *r = new Rule[PROOF];		
	uint *p = new uint[PROOF];		
	bool *d = new bool[PROOF];				
		
	int level = 0;
	
	uint discard = suffixes[current].cleft;
	if (suffixes[current].cright < discard) discard = suffixes[current].cright;
	uint read = discard;
	uint cprev = 0;
		
	uint pivot = (suffixes[current].left + suffixes[current].right)/2;
	uint pos = seqR->labelForRowNoBitSeq(pivot)-1;
		
	while (true)
	{
		r[level].id = permutation->pi(seqR->retrieveColumnPerLabelNoBitSeq(pos+2));
		r[level].len = lenrules->access(r[level].id);
		
		if (discard < r[level].len) break;
		
		discard -= r[level].len;
		cprev += r[level].len;
		pos++;
	}
	
	if (discard > 0) jumpCharsDirect(discard, r, p, d, &level);
	
	int less = -1, equal = -1, greater = -1, pless = -1;		
	uint c;
			
	// Extracting the rule and comparing with respect to the suffixes
	while (true)
	{					
		if (r[level].len > 1)
		{
			r[level].left = permutation->pi(slp->firstPointForLabel_X(r[level].id, &(p[level])));
			d[level] = false;
			
			level++;
			r[level].id = r[level-1].left;
		}
		else
		{
			c = Y->rank1(r[level].id);
				
			if (c < suffixes[current].pattern[read])
			{			
				suffixes[current].left = pivot+1;
				suffixes[current].cleft = read;
				greater = size-1;
				break;
			}
			else
			{
				if (c > suffixes[current].pattern[read])
				{
					suffixes[current].right = pivot-1;
					suffixes[current].cright = read;
						
					do
					{
						suffixes[current].right = pivot-1;
						suffixes[current].cright = read;
						current++;
					}
					while ((current < size) && (suffixes[current].lcp > read));
						
					less = current-1;
						
					if (current == size) break;
					else
					{
						if (suffixes[current].lcp < read)
						{
							suffixes[current].left = pivot+1;
							suffixes[current].cleft = suffixes[current].lcp;
							greater = size-1;
							break;
						}
						else
						{
							while ((current < size) && (c > suffixes[current].pattern[read]))
							{
								suffixes[current].right = pivot-1;
								suffixes[current].cright = read;
								current++;
							}
							
							less = current-1;
								
							if (current == size) break;
								
							if (c < suffixes[current].pattern[read])
							{
								suffixes[current].left = pivot+1;
								suffixes[current].cleft = suffixes[current].lcp;
								greater = size-1;
								break;
							}
						}
					}
				}
			}
			
			read++;
				
			if (read == suffixes[current].length) 
			{
				if (less != -1) pless = less;
					
				equal = current;
				current++;
					
				if (current == size) break;
					
				if (suffixes[current].lcp < read)
				{						
					suffixes[current].left = pivot+1;
					suffixes[current].cleft = suffixes[current].lcp;
					greater = size-1;
					break;
				}
			}
			
			if (read == (r[0].len+cprev))
			{					
				// Extracting the next position in the compressed sequence
				level=0;					
				cprev += r[level].len;
				pos++;

				r[level].id = permutation->pi(seqR->retrieveColumnPerLabelNoBitSeq(pos+2));
				r[level].len = lenrules->access(r[level].id);
			}
			else
			{
				do level--; while (d[level]);
					
				r[level].right = slp->firstPointForLabel_Y(r[level].id, p[level]);
				d[level] = true;
				
				level++;
				r[level].id = r[level-1].right;
			}
		}
		
		r[level].len = lenrules->access(r[level].id);
	}
	
	delete [] r;
	delete [] p;
	delete [] d;

	if (less >= 0)
	{
		if (equal >= 0)
		{				
			if (greater >= 0)
			{
				if ((equal < size-1) && (suffixes[equal].length == suffixes[equal+1].lcp))
				{	
					if (less == pless)
					{
						extractAndCompareDirectInSequence(suffixes, less+1);
						obtainRangeDirectInSequence(pivot, suffixes+less+1, equal-less);
						
						suffixes[equal+1].left = suffixes[equal].left;
						suffixes[equal+1].cleft = suffixes[equal+1].lcp;
						suffixes[equal+1].right = suffixes[equal].right;
						suffixes[equal+1].cright = suffixes[equal+1].lcp;
						
						extractAndCompareDirectInSequence(suffixes+equal+1, greater-equal);
					}
					else
					{
						if (pless >= 0) extractAndCompareDirectInSequence(suffixes, pless+1);
						obtainRangeDirectInSequence(pivot, suffixes+pless+1, equal-pless);
						
						suffixes[equal+1].left = suffixes[equal].left;
						suffixes[equal+1].cleft = suffixes[equal+1].lcp;
						suffixes[equal+1].right = suffixes[equal].right;
						suffixes[equal+1].cright = suffixes[equal+1].lcp;
						
						extractAndCompareDirectInSequence(suffixes+equal+1, less-equal);						
						extractAndCompareDirectInSequence(suffixes+less+1, greater-less);
					}
				}
				else
				{					
					extractAndCompareDirectInSequence(suffixes, less+1);
					obtainRangeDirectInSequence(pivot, suffixes+less+1, equal-less);
					extractAndCompareDirectInSequence(suffixes+equal+1, greater-equal);
				}
			}
			else
			{
				if ((equal < size-1) && (suffixes[equal].length == suffixes[equal+1].lcp))
				{
					if (less == pless)
					{
						extractAndCompareDirectInSequence(suffixes, less+1);
						obtainRangeDirectInSequence(pivot, suffixes+less+1, equal-less);							
					}
					else
					{
						if (pless >= 0) extractAndCompareDirectInSequence(suffixes, pless+1);
						obtainRangeDirectInSequence(pivot, suffixes+pless+1, equal-pless);

						suffixes[equal+1].left = suffixes[equal].left;
						suffixes[equal+1].cleft = suffixes[equal+1].lcp;
						suffixes[equal+1].right = suffixes[equal].right;
						suffixes[equal+1].cright = suffixes[equal+1].lcp;
							
						extractAndCompareDirectInSequence(suffixes+equal+1, less-equal);													
					}
				}
				else
				{
					extractAndCompareDirectInSequence(suffixes, less+1);
					obtainRangeDirectInSequence(pivot, suffixes+less+1, equal-less);
				}
			}
		}
		else
		{
			if (greater >= 0)
			{
				extractAndCompareDirectInSequence(suffixes, less+1);
				extractAndCompareDirectInSequence(suffixes+less+1, greater-less);
			}
			else
			{
				extractAndCompareDirectInSequence(suffixes, size);
			}
		}
	}
	else
	{
		if (equal >= 0)
		{
			if (greater >= 0)
			{
				obtainRangeDirectInSequence(pivot, suffixes, equal+1);
				extractAndCompareDirectInSequence(suffixes+equal+1, greater-equal);
			}
			else
			{
				obtainRangeDirectInSequence(pivot, suffixes, size);
			}
		}
		else
		{
			extractAndCompareDirectInSequence(suffixes, size);
		}			
	}
}

void 
wRePairSLPIndex::obtainRangeDirectInSequence(uint rule, PSpattern *suffixes, uint size)
{
	uint rl, rr, rc;
		
	Rule *r = new Rule[PROOF];		
	uint *p = new uint[PROOF];		
	bool *d = new bool[PROOF];
		
	int level;
		
	for (uint current=0; current<size; current++)
	{
		// Get LEFT limit
		rl = suffixes[0].left; rr = rule;
					
		do
		{
			level = 0;
			uint discard = suffixes[0].cleft;
			uint cprev = 0;
			
			rc = (rl+rr)/2;
			uint pos = seqR->labelForRowNoBitSeq(rc)-1;
			
			while (true)
			{
				r[level].id = permutation->pi(seqR->retrieveColumnPerLabelNoBitSeq(pos+2));
				r[level].len = lenrules->access(r[level].id);
				
				if (discard < r[level].len) break;
				discard -= r[level].len;
				cprev += r[level].len;
				pos++;
			}
			
			if (discard > 0) jumpCharsDirect(discard, r, p, d, &level);
			uint read = suffixes[0].cleft;
				
			int cmp = expandRuleAndCompareDirectInSequence(suffixes[current].pattern, suffixes[current].length, pos, &read, cprev, r, p, d, level);
				
			if (cmp < 0)
			{
				rl = rc+1;
				suffixes[0].cleft = read;					
			}
			else
			{
				rr = rc;
			}				
		}
		while (rl < rr);
		
		suffixes[current].left = rl;

		// Get RIGHT limit
		rl = rule; rr = suffixes[0].right;
	
		do
		{
			level = 0;
			uint discard = suffixes[0].cright;
			uint cprev = 0;
			
			rc = 1+((rl+rr)/2);
			uint pos = seqR->labelForRowNoBitSeq(rc)-1;
			
			while (true)
			{			
				r[level].id = permutation->pi(seqR->retrieveColumnPerLabelNoBitSeq(pos+2));
				r[level].len = lenrules->access(r[level].id);
				
				if (discard < r[level].len) break;
				discard -= r[level].len;
				cprev += r[level].len;
				pos++;
			}
			
			if (discard > 0) jumpCharsDirect(discard, r, p, d, &level);
			uint read = suffixes[0].cright;
			
			int cmp = expandRuleAndCompareDirectInSequence(suffixes[current].pattern, suffixes[current].length, pos, &read, cprev, r, p, d, level);
				
			if (cmp > 0)
			{
				rr = rc-1;
				suffixes[0].cleft = read;							
			}
			else
			{
				rl = rc;
			}
		}
		while (rl < rr);
		
		suffixes[current].right = rl;
	}			
	
	delete [] r;
	delete [] p;
	delete [] d;
}

int 
wRePairSLPIndex::expandRuleAndCompareDirectInSequence(uint *pattern, uint length, uint pos, uint *read, uint cprev, Rule *r, uint *p, bool *d, int level)
{
	uint c;
		
	while (true)
	{					
		if (r[level].len > 1)
		{
			r[level].left = permutation->pi(slp->firstPointForLabel_X(r[level].id, &(p[level])));
			d[level] = false;
				
			level++;
			r[level].id = r[level-1].left;
		}
		else
		{				
			c = Y->rank1(r[level].id);

			if (c != pattern[*read]) return (c - pattern[*read]);
			(*read)++; 					
			if ((*read) == length) return 0;
			
			if ((*read) == (r[0].len+cprev))
			{
				// Extracting the next position in the compressed sequence
				level=0;					
				cprev += r[level].len;
				pos++;

				r[level].id = permutation->pi(seqR->retrieveColumnPerLabelNoBitSeq(pos+2));
				r[level].len = lenrules->access(r[level].id);
			}
			else
			{
				do level--; while (d[level]);
					
				r[level].right = slp->firstPointForLabel_Y(r[level].id, p[level]);
				d[level] = true;
					
				level++;
				r[level].id = r[level-1].right;
			}
		}
			
		r[level].len = lenrules->access(r[level].id);
	}
}

void 
wRePairSLPIndex::expandRule(uint rule, uint len, uint *sequence)
{
	if (len == 1)
	{
		// Identifying the terminal symbol			
		sequence[0] = Y->rank1(rule);
	}
	else
	{
		Point p = slp->firstPointForLabel(rule);
		
		uint lid = permutation->pi(p.left); 
		uint llen = lenrules->access(lid);
		
		expandRule(lid, llen, sequence);
		expandRule(p.right, lenrules->access(p.right), sequence+llen);
	}
}

wRePairSLPIndex::~wRePairSLPIndex()
{
	delete wm;
	delete Y;
	delete lenrules;
	delete permutation;
	delete slp;

	delete seqB;
	delete seqR;

	if (mapper != NULL) delete mapper;
	if (doc_array != NULL) delete [] doc_array;
}	

