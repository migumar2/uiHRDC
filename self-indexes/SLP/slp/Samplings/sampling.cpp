#include "sampling.h"

Sampling::Sampling()
{
}

Sampling::Sampling(uchar *samples, uint length, uint *pointers, uint cptrs, uint blocksize, uint terminals, uint *posterms, uint delta_sampling)
{
	prefixes = cptrs;
	dictionary = new csd::CSD_HTFC(samples, length, blocksize);

	uint *deltas = new uint[cptrs+1];

	for(unsigned int i=0;i<=cptrs;i++)
		deltas[i] = pointers[i+1]-pointers[i];

	ptrRules = new DeltaCodes(deltas, cptrs, delta_sampling);

	delete [] deltas;

	this->terminals = terminals;
	this->posterms = posterms;
}

Sampling::~Sampling()
{
	delete dictionary;
	delete ptrRules;
	delete [] posterms;
}

size_t 
Sampling::getSize()
{
	return dictionary->getSize() + ptrRules->size()+terminals*sizeof(uint)+sizeof(Sampling);
}

size_t 
Sampling::getBlocks()
{
	return dictionary->getBlocks();
}

size_t 
Sampling::getMaxLength()
{
	return dictionary->getMaxLength()-1;
}

uint
Sampling::getTerminals()
{
	return terminals;
}

uint 
Sampling::extract(size_t id, uchar* s)
{
	// Searching for the rule id
	uint l = 1, r = prefixes, c = 0;
	uint val = 0;
	id--;

	while ((l+1) < r)
	{
		c = (l+r)/2;
		val = ptrRules->select(c);

		if (val < id) l = c;
		else
		{
			if (val > id) r = c;
			else break;
		}
	}

	int size;
	if (id < val){ size = dictionary->extract(c-1, s); }
	else{ size = dictionary->extract(c, s); }
	return size;
}

void 
Sampling::locatePrefix(uchar *pattern, uint len, uint *left, uint *right, uint *bleft, uint *bright)
{
    dictionary->locateByPrefix(pattern, len, left, right, bleft, bright);

    if (*left > 0)
    {
      *left = ptrRules->select(*left)+1;
      if (*right != prefixes) *right = ptrRules->select(*right+1);
      else *right = ptrRules->select(*right);
    }
    else
    {
      *left = 0;
      *right = 0;
    }
}

void 
Sampling::locatePattern(uchar *pattern, uint len, uint *left, uint *right, uint *block)
{
    dictionary->locate(pattern, len, left, block);

    if (*left > 0)
    {
      if (*left != prefixes) *right = ptrRules->select(*left+1);
      else *right = ptrRules->select(*left);
      *left = ptrRules->select(*left)+1;
    }
    else
    {
      *left = 0;
      *right = 0;
    }
}

void 
Sampling::save(ofstream & fp)
{
	saveValue(fp, prefixes);
	dictionary->save(fp);
	ptrRules->save(fp);
	saveValue(fp, terminals);
	saveValue(fp, posterms, terminals);
}

Sampling*
Sampling::load(ifstream & fp)
{
	Sampling *samp = new Sampling();

	samp->prefixes = loadValue<uint>(fp);
	samp->dictionary = (csd::CSD_HTFC*)csd::CSD::load(fp);
	samp->ptrRules = DeltaCodes::load(fp);

	samp->terminals = loadValue<uint>(fp);
	samp->posterms = loadValue<uint>(fp,samp->terminals);

	return samp;
}

size_t
Sampling::leftBlockForTerminal(uint terminal)
{
	if (terminal < terminals)
		return posterms[terminal-1];
	else
		return getBlocks()-1;
}

size_t
Sampling::rightBlockForTerminal(uint terminal)
{
	if (terminal < terminals)
		return posterms[terminal];
	else 
		return getBlocks()-1;
}

void
Sampling::decompress()
{
	uchar *dict;
	dictionary->decompress(&dict);
	delete [] dict;
}

