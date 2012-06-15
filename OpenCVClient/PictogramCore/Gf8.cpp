//
//  gf8.cpp
//  rscode_cpp
//
//  Created by Chat F Mak on 1/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "gf8.h"

Gf8::Gf8()
{
    a[0] = 1; //a^0
    a[1] = 2; //a^1
    a[2] = 4; //a^2
    a[3] = 3; //a^3
    a[4] = 6; //a^4
    a[5] = 7; //a^5
    a[6] = 5; //a^6
    
    ia[1]=0;
    ia[2]=1;
    ia[3]=3;
    ia[4]=2;
    ia[5]=6;
    ia[6]=4;
    ia[7]=5;
    
    int tmpmult[7][7]=
    {{0,1,2,3,4,5,6},
        {1,2,3,4,5,6,0},
        {2,3,4,5,6,0,1},
        {3,4,5,6,0,1,2},
        {4,5,6,0,1,2,3},
        {5,6,0,1,2,3,4},
        {6,0,1,2,3,4,5}};
    
    for(int i=0;i<7;i++)
    {
        for(int j=0;j<7;j++)
            mult[i][j] = tmpmult[i][j];
    }
    
    //genPoly for RS(7,3)
    genPoly[4] = 1;
    genPoly[3] = 3;
    genPoly[2] = 1;
    genPoly[1] = 2;
    genPoly[0] = 3;
}

int Gf8::gmult(int x1, int x2)
{
    if(x1==0 || x2==0)
        return 0;
    return a[mult[ia[x1]][ia[x2]]];
}

int Gf8::gexp(int x)
{
    return a[x];
}

int Gf8::glog(int x)
{
    return ia[x];
}

int Gf8::ginv(int x)
{
    return a[(7-ia[x])%7];
}