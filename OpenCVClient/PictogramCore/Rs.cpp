//
//  Rs.cpp
//  rscode_cpp
//
//  Created by Chat F Mak on 1/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "rs.h"

Rs::Rs()
{
    initRs();
}


void Rs::initRs()
{
    memset(rx, 0, sizeof(unsigned char)*7);
    memset(decoded, 0, sizeof(unsigned char)*7);
    memset(synBytes, 0, sizeof(unsigned char)*4);
    
    /* sigma */
    memset(sigma, 0, sizeof(unsigned char)*2);
    memset(sigmaEval, 0, sizeof(unsigned char)*7);
    
    /*error locator*/
    memset(errorLocator, 0, sizeof(unsigned char)*2);
    nerr=0;
    
    decodeSucceeded = false;
}

void Rs::encode (unsigned char* msg, unsigned char* dst)
{
    int NPAR = 4;
    int nbytes = 3;
    int i, LFSR[NPAR+1],dbyte, j;
	
    for(i=0; i < NPAR+1; i++) LFSR[i]=0;
    
    for (i = 0; i < nbytes; i++) {
        dbyte = msg[i] ^ LFSR[NPAR-1];
        for (j = NPAR-1; j > 0; j--) {
            LFSR[j] = LFSR[j-1] ^ gmult(genPoly[j], dbyte);
        }
        LFSR[0] = gmult(genPoly[0], dbyte);
    }
    
    unsigned char pBytes[NPAR];
    for (i = 0; i < NPAR; i++) 
        pBytes[i] = LFSR[i];
    
    for (i = 0; i < nbytes; i++) dst[i] = msg[i];
    for (i = 0; i < NPAR; i++) {
        dst[i+nbytes] = pBytes[NPAR-1-i];
    }
}

bool Rs::decode(unsigned char *receivedCode)
{
    initRs();
    
    memcpy(rx, receivedCode, sizeof(unsigned char)*7);
    findSyndrome();
    if(synBytes[0]==0 && synBytes[1]==0 && synBytes[2]==0 && synBytes[3]==0) //no error
    {
        memcpy(decoded, rx, sizeof(unsigned char)*7);
        decodeSucceeded = true;
        return true;
    }
    findSigma();
    evalSigma();
    decodeSucceeded = correctErrors();
    return decodeSucceeded;
}

bool Rs::getDecodedCode(unsigned char* dec)
{
    if(dec==0)
        return false;
    if(decodeSucceeded)
        memcpy(dec, decoded, sizeof(unsigned char)*7);
    return decodeSucceeded;
}

bool Rs::getDecodedMessage(unsigned char* decmsg)
{
    if(decmsg==0)
        return false;
    if(decodeSucceeded)
    {
        decmsg[0]=decoded[0];
        decmsg[1]=decoded[1];
        decmsg[2]=decoded[2];
    }
    return decodeSucceeded;
}

void Rs::findSyndrome()
{
    int i, j, sum;
    printf("\nRx : ");
    for(int i=0;i<7;i++)
        printf("%u\t", rx[i]);
    printf("\nSyndrome: ");
    for (j = 0; j < 4;  j++) {
        sum	= 0;
        for (i = 0; i < 7; i++) {
            sum = rx[i] ^ gmult(gexp(j+1), sum);
        }
        synBytes[j]  = sum;
        printf("%d\t", synBytes[j]);
    }
    //    for(int i=0;i<4;i++)
    //    {
    //        synBytes[i]=0;
    //        for(int j=0;j<7;j++)
    //        {
    //            synBytes[i] = synBytes[i] ^ gmult(data[j], gexp((i+1)*(6-j)%7));
    //        }
    //    }
}

/* sigma(X) = a0 + sigma[0]*X + sigma[1]*X2*/
void Rs::findSigma()
{
    int det = gmult(synBytes[0], synBytes[2]) ^ gmult(synBytes[1], synBytes[1]);
    printf("\ndet: %d", det);
    
    if(det!=0) //have 2 or more errors
    {
        //        int invS[4] = {synBytes[2], synBytes[1], synBytes[1], synBytes[0]};
        //        
        //        printf("\ninvS:");
        //        for(int i=0;i<4;i++)
        //        {
        //            invS[i] = gmult(invS[i], ginv(det));
        //            printf("%d\t", invS[i]);
        //        }
        //        
        //        sigma[1] = gmult(invS[0], synBytes[2]) ^ gmult(invS[1], synBytes[3]);
        //        sigma[0] = gmult(invS[2], synBytes[2]) ^ gmult(invS[3], synBytes[3]);
        
        sigma[0] = gmult(synBytes[1], synBytes[2]) ^ gmult(synBytes[0], synBytes[3]);
        sigma[0] = gmult(sigma[0], ginv(det));
        sigma[1] = gmult(synBytes[2], synBytes[2]) ^ gmult(synBytes[1], synBytes[3]);
        sigma[1] = gmult(sigma[1], ginv(det));
        
        printf("\nsigma: %d\t%d", sigma[0], sigma[1]);
    }
    else //have only one error
    {
        sigma[0] = ginv(gmult(synBytes[0], ginv(synBytes[1])));
        sigma[1] = 0;
        printf("\nsigma: %d\t%d", sigma[0], sigma[1]);
    }
}

//evaluate sigma(X) at X=a0,...,a6
void Rs::evalSigma()
{
    int x = 0;
    int x2 = 0;
    
    printf("\nsigmaEval: ");
    for(int i=6;i>=0;i--)
    {
        x = gexp(i);
        x2 = gmult(x,x);
        sigmaEval[6-i] = gexp(0) ^ gmult(sigma[0], x) ^ gmult(sigma[1], x2);
        if(sigmaEval[6-i]==0)
        {
            errorLocator[nerr++] = ginv(x);
        }
    }
    for(int i=0;i<7;i++)
        printf("%d\t", sigmaEval[i]);
    printf("\nerror loc: ");
    for(int i=0;i<nerr;i++)
    {
        printf("%d\t", errorLocator[i]);
    }
}

bool Rs::correctErrors()
{
    int numerator=0;
    int demoninator = 1;
    int idx=0;
    int y[2]; //error magnitudes
    
    for(int i=0;i<7;i++)
        decoded[i] = rx[i];
    
    if(nerr==1)
    {
        numerator = synBytes[0];
        demoninator = errorLocator[0];
        y[0] = gmult(numerator, ginv(demoninator));
        
        idx = 6-glog(errorLocator[0]);
        decoded[idx] = rx[idx]^y[0];
        return true;
    }
    else if(nerr==2)
    {
        numerator = gmult(errorLocator[1], synBytes[0]) ^ synBytes[1];
        demoninator = gmult(errorLocator[0], (errorLocator[0]^errorLocator[1]));
        y[0] = gmult(numerator, ginv(demoninator));
        
        numerator = gmult(errorLocator[0], synBytes[0]) ^ synBytes[1];
        demoninator = gmult(errorLocator[1], (errorLocator[0]^errorLocator[1]));
        y[1] = gmult(numerator, ginv(demoninator));
        printf("\ny: %d\t%d", y[0],y[1]);
        
        idx = 6-glog(errorLocator[0]);
        decoded[idx] = rx[idx]^y[0];
        idx = 6-glog(errorLocator[1]);
        decoded[idx] = rx[idx]^y[1];
        
        return true;
    }
    return false;
}