/* Range search data structure
 */

#ifndef RANGEINCLUDED
#define RANGEINCLUDED

namespace lz77index{
namespace basics{

typedef struct srange *range;
/* builds the range data structure for values (x,y[x]), x=1..n-1
 * y gets freed
 */
range createRange(unsigned int *y, unsigned int n);

/* executes process(processData,y) for each pair (x,y) stored in the 
 * rectangle [x1..x2] x [y1..y2]
 */
void searchRange(range R, unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2, void (*process)(void*, unsigned int),void *processData);

/* frees the range data structure */
void destroyRange(range R);

/* saves the range */
unsigned int saveRange(range R, FILE* fp);

/* loads the range */
range loadRange(FILE* fp);

unsigned int sizeOfRange(range R);

}
}

#endif

