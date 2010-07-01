/*
Copyright 2007 nVidia, Inc.
Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. 

You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 

See the License for the specific language governing permissions and limitations under the License.
*/
#pragma once
#ifndef _SHAPES_TWO_H
#define _SHAPES_TWO_H

// shapes for two regions

#define NREGIONS 2
#define NSHAPES 64
#define SHAPEBITS 6

static int shapes[NSHAPES*16] = 
{
0, 0, 1, 1,   0, 0, 0, 1,   0, 1, 1, 1,   0, 0, 0, 1,   
0, 0, 1, 1,   0, 0, 0, 1,   0, 1, 1, 1,   0, 0, 1, 1,   
0, 0, 1, 1,   0, 0, 0, 1,   0, 1, 1, 1,   0, 0, 1, 1,   
0, 0, 1, 1,   0, 0, 0, 1,   0, 1, 1, 1,   0, 1, 1, 1,   

0, 0, 0, 0,   0, 0, 1, 1,   0, 0, 0, 1,   0, 0, 0, 0,   
0, 0, 0, 1,   0, 1, 1, 1,   0, 0, 1, 1,   0, 0, 0, 1,   
0, 0, 0, 1,   0, 1, 1, 1,   0, 1, 1, 1,   0, 0, 1, 1,   
0, 0, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   0, 1, 1, 1,   

0, 0, 0, 0,   0, 0, 1, 1,   0, 0, 0, 0,   0, 0, 0, 0,   
0, 0, 0, 0,   0, 1, 1, 1,   0, 0, 0, 1,   0, 0, 0, 0,   
0, 0, 0, 1,   1, 1, 1, 1,   0, 1, 1, 1,   0, 0, 0, 1,   
0, 0, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   0, 1, 1, 1,   

0, 0, 0, 1,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   
0, 1, 1, 1,   0, 0, 0, 0,   1, 1, 1, 1,   0, 0, 0, 0,   
1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   0, 0, 0, 0,   
1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   

0, 0, 0, 0,   0, 1, 1, 1,   0, 0, 0, 0,   0, 1, 1, 1,   
1, 0, 0, 0,   0, 0, 0, 1,   0, 0, 0, 0,   0, 0, 1, 1,   
1, 1, 1, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 1,   
1, 1, 1, 1,   0, 0, 0, 0,   1, 1, 1, 0,   0, 0, 0, 0,   

0, 0, 1, 1,   0, 0, 0, 0,   0, 0, 0, 0,   0, 1, 1, 1,   
0, 0, 0, 1,   1, 0, 0, 0,   0, 0, 0, 0,   0, 0, 1, 1,   
0, 0, 0, 0,   1, 1, 0, 0,   1, 0, 0, 0,   0, 0, 1, 1,   
0, 0, 0, 0,   1, 1, 1, 0,   1, 1, 0, 0,   0, 0, 0, 1,   

0, 0, 1, 1,   0, 0, 0, 0,   0, 1, 1, 0,   0, 0, 1, 1,   
0, 0, 0, 1,   1, 0, 0, 0,   0, 1, 1, 0,   0, 1, 1, 0,   
0, 0, 0, 1,   1, 0, 0, 0,   0, 1, 1, 0,   0, 1, 1, 0,   
0, 0, 0, 0,   1, 1, 0, 0,   0, 1, 1, 0,   1, 1, 0, 0,   

0, 0, 0, 1,   0, 0, 0, 0,   0, 1, 1, 1,   0, 0, 1, 1,   
0, 1, 1, 1,   1, 1, 1, 1,   0, 0, 0, 1,   1, 0, 0, 1,   
1, 1, 1, 0,   1, 1, 1, 1,   1, 0, 0, 0,   1, 0, 0, 1,   
1, 0, 0, 0,   0, 0, 0, 0,   1, 1, 1, 0,   1, 1, 0, 0,   

0, 1, 0, 1,   0, 0, 0, 0,   0, 1, 0, 1,   0, 0, 1, 1,   
0, 1, 0, 1,   1, 1, 1, 1,   1, 0, 1, 0,   0, 0, 1, 1,   
0, 1, 0, 1,   0, 0, 0, 0,   0, 1, 0, 1,   1, 1, 0, 0,   
0, 1, 0, 1,   1, 1, 1, 1,   1, 0, 1, 0,   1, 1, 0, 0,   

0, 0, 1, 1,   0, 1, 0, 1,   0, 1, 1, 0,   0, 1, 0, 1,   
1, 1, 0, 0,   0, 1, 0, 1,   1, 0, 0, 1,   1, 0, 1, 0,   
0, 0, 1, 1,   1, 0, 1, 0,   0, 1, 1, 0,   1, 0, 1, 0,   
1, 1, 0, 0,   1, 0, 1, 0,   1, 0, 0, 1,   0, 1, 0, 1,   

0, 1, 1, 1,   0, 0, 0, 1,   0, 0, 1, 1,   0, 0, 1, 1,   
0, 0, 1, 1,   0, 0, 1, 1,   0, 0, 1, 0,   1, 0, 1, 1,   
1, 1, 0, 0,   1, 1, 0, 0,   0, 1, 0, 0,   1, 1, 0, 1,   
1, 1, 1, 0,   1, 0, 0, 0,   1, 1, 0, 0,   1, 1, 0, 0,   

0, 1, 1, 0,   0, 0, 1, 1,   0, 1, 1, 0,   0, 0, 0, 0,   
1, 0, 0, 1,   1, 1, 0, 0,   0, 1, 1, 0,   0, 1, 1, 0,   
1, 0, 0, 1,   1, 1, 0, 0,   1, 0, 0, 1,   0, 1, 1, 0,   
0, 1, 1, 0,   0, 0, 1, 1,   1, 0, 0, 1,   0, 0, 0, 0,   

0, 1, 0, 0,   0, 0, 1, 0,   0, 0, 0, 0,   0, 0, 0, 0,   
1, 1, 1, 0,   0, 1, 1, 1,   0, 0, 1, 0,   0, 1, 0, 0,   
0, 1, 0, 0,   0, 0, 1, 0,   0, 1, 1, 1,   1, 1, 1, 0,   
0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 1, 0,   0, 1, 0, 0,   

0, 1, 1, 0,   0, 0, 1, 1,   0, 1, 1, 0,   0, 0, 1, 1,   
1, 1, 0, 0,   0, 1, 1, 0,   0, 0, 1, 1,   1, 0, 0, 1,   
1, 0, 0, 1,   1, 1, 0, 0,   1, 0, 0, 1,   1, 1, 0, 0,   
0, 0, 1, 1,   1, 0, 0, 1,   1, 1, 0, 0,   0, 1, 1, 0,   

0, 1, 1, 0,   0, 1, 1, 0,   0, 1, 1, 1,   0, 0, 0, 1,   
1, 1, 0, 0,   0, 0, 1, 1,   1, 1, 1, 0,   1, 0, 0, 0,   
1, 1, 0, 0,   0, 0, 1, 1,   1, 0, 0, 0,   1, 1, 1, 0,   
1, 0, 0, 1,   1, 0, 0, 1,   0, 0, 0, 1,   0, 1, 1, 1,   

0, 0, 0, 0,   0, 0, 1, 1,   0, 0, 1, 0,   0, 1, 0, 0,   
1, 1, 1, 1,   0, 0, 1, 1,   0, 0, 1, 0,   0, 1, 0, 0,   
0, 0, 1, 1,   1, 1, 1, 1,   1, 1, 1, 0,   0, 1, 1, 1,   
0, 0, 1, 1,   0, 0, 0, 0,   1, 1, 1, 0,   0, 1, 1, 1,   

};

#define	REGION(x,y,si)	shapes[((si)&3)*4+((si)>>2)*64+(x)+(y)*16]

static int shapeindex_to_compressed_indices[NSHAPES*2] = 
{
	0,15,  0,15,  0,15,  0,15,
	0,15,  0,15,  0,15,  0,15,
	0,15,  0,15,  0,15,  0,15,
	0,15,  0,15,  0,15,  0,15,

	0,15,  0, 2,  0, 8,  0, 2,
	0, 2,  0, 8,  0, 8,  0,15,
	0, 2,  0, 8,  0, 2,  0, 2,
	0, 8,  0, 8,  0, 2,  0, 2,

	0,15,  0,15,  0, 6,  0, 8,
	0, 2,  0, 8,  0,15,  0,15,
	0, 2,  0, 8,  0, 2,  0, 2,
	0, 2,  0,15,  0,15,  0, 6,

	0, 6,  0, 2,  0, 6,  0, 8,
	0,15,  0,15,  0, 2,  0, 2,
	0,15,  0,15,  0,15,  0,15,
	0,15,  0, 2,  0, 2,  0,15

};
#define SHAPEINDEX_TO_COMPRESSED_INDICES(si,region)  shapeindex_to_compressed_indices[(si)*2+(region)]

#endif