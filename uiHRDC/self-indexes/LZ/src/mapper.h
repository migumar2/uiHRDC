/* mapper.h
 * Copyright (C) 2008, Sebastian Kreft C., all rights reserved.
 *
 * mapper definition
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
 */

#ifndef __MAPPER_H__
#define __MAPPER_H__

#include <cstdio>

namespace lz77index{
/** Mapper that converts bettwen unsigned char ans effective sigma chars
 *  It stores two arrays, one for mapping from unsigned char: uses 256*1 bytes, could use slightly less
 *  the other, maps from sigma to unsigned char:  uses sigma*1 bytes  
 *
 *  @author Sebastian Kreft C.
 */
class mapper {
  public:
        mapper(unsigned char* text, unsigned int len);
        ~mapper();
        unsigned int map(unsigned char* s,unsigned int len);
        unsigned int unmap(unsigned char* s,unsigned int len);
        unsigned char mapchar(unsigned char s);
        unsigned char unmapchar(unsigned char s);
        unsigned int size();
        unsigned int sigma();
        unsigned int save(FILE *fp);
        static mapper* load(FILE *fp);
    protected:
        mapper();
        unsigned char* a_map;
        unsigned char* a_unmap;
        unsigned int sigma_size;
};

}

#endif /* _MAPPER_H */
