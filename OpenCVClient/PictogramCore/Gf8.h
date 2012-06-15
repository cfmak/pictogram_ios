//
//  gf8.h
//  rscode_cpp
//
//  Created by Chat F Mak on 1/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef rscode_cpp_gf8_h
#define rscode_cpp_gf8_h

class Gf8
{
public:
    /* generator polynomial */
    int genPoly[5];
    
    //element a^0 to a^7
    int a[7];
    
    //log element
    int ia[8];
    
    //multiplication table
    int mult[7][7]; //mult[1][2] = a^1 * a^2

    Gf8();

    int gmult(int x1, int x2);

    int gexp(int x);

    int glog(int x);

    int ginv(int x);
};

#endif
