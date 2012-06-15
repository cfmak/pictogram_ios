//
//  rs.h
//  rscode_cpp
//
//  Created by Chat F Mak on 1/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef rscode_cpp_rs_h
#define rscode_cpp_rs_h

#include "gf8.h"

class Rs : public Gf8{
public:
    Rs();
    void initRs();
    bool decode(unsigned char *rx);
    bool getDecodedCode(unsigned char* dec);
    bool getDecodedMessage(unsigned char* decmsg);
    
    void encode (unsigned char* msg, unsigned char* dst);
private: 
    
    //decode methods
    void findSyndrome();
    void findSigma();
    void evalSigma();
    bool correctErrors();
    
    bool decodeSucceeded;
    
    unsigned char rx[7];
    unsigned char decoded[7];
        
    // Decoder variables
    /* Decoder syndrome bytes */
    unsigned char synBytes[4];

    /* sigma */
    unsigned char sigma[2];
    unsigned char sigmaEval[7];
    
    /*error locator*/
    unsigned char errorLocator[2]; //error location value; to get power of a=> glog(eloc); to get array index=> 6-glog(eloc)
    unsigned char nerr;  //number of found errors
};

#endif
