//  GBM by Greg Ridgeway  Copyright (C) 2003

#include "laplace.h"

CLaplace::CLaplace()
{

}

CLaplace::~CLaplace()
{

}


GBMRESULT CLaplace::ComputeWorkingResponse
(
    double *adY,
    double *adMisc,
    double *adOffset,
    double *adF, 
    double *adZ, 
    double *adWeight,
    bool *afInBag,
    unsigned long nTrain
)
{
    unsigned long i = 0;

    if(adOffset == NULL)
    {
        for(i=0; i<nTrain; i++)
        {
            adZ[i] = (adY[i] - adF[i]) > 0.0 ? 1.0 : -1.0;
        }
    }
    else
    {
        for(i=0; i<nTrain; i++)
        {
            adZ[i] = (adY[i] - adOffset[i] - adF[i]) > 0.0 ? 1.0 : -1.0;
        }
    }

    return GBM_OK;
}



// DEBUG: needs weighted median
GBMRESULT CLaplace::InitF
(
    double *adY,
    double *adMisc,
    double *adOffset, 
    double *adWeight,
    double &dInitF, 
    unsigned long cLength
)
{
    double dOffset=0.0;
    unsigned long i=0;

    vecd.resize(cLength);
    for(i=0; i<cLength; i++)
    {
        dOffset = (adOffset==NULL) ? 0.0 : adOffset[i];
        vecd[i] = adY[i] - dOffset;
    }

    itMedian = vecd.begin() + (vecd.end() - vecd.begin())/2;
    nth_element(vecd.begin(), itMedian, vecd.end()) ;

    dInitF = *itMedian;

    return GBM_OK;
}


double CLaplace::LogLikelihood
(
    double *adY,
    double *adMisc,
    double *adOffset, 
    double *adWeight,
    double *adF,
    unsigned long cLength
)
{
    unsigned long i=0;
    double dL = 0.0;
    double dW = 0.0;

    if(adOffset == NULL)
    {
        for(i=0; i<cLength; i++)
        {
            dL += adWeight[i]*fabs(adY[i]-adF[i]);
            dW += adWeight[i];
        }
    }
    else
    {
        for(i=0; i<cLength; i++)
        {
            dL += adWeight[i]*fabs(adY[i]-adOffset[i]-adF[i]);
            dW += adWeight[i];
        }
    }

    return dL/dW;
}


// DEBUG: needs weighted median
GBMRESULT CLaplace::FitBestConstant
(
    double *adY,
    double *adMisc,
    double *adOffset,
    double *adW,
    double *adF,
    double *adZ,
    unsigned long *aiNodeAssign,
    unsigned long nTrain,
    VEC_P_NODETERMINAL vecpTermNodes,
    unsigned long cTermNodes,
    unsigned long cMinObsInNode,
    bool *afInBag,
    double *adFadj
)
{
    GBMRESULT hr = GBM_OK;

    unsigned long iNode = 0;
    unsigned long iObs = 0;
    unsigned long iVecd = 0;
    double dOffset;

    vecd.resize(nTrain); // should already be this size from InitF
    for(iNode=0; iNode<cTermNodes; iNode++)
    {
        if(vecpTermNodes[iNode]->cN >= cMinObsInNode)
        {
            iVecd = 0;
            for(iObs=0; iObs<nTrain; iObs++)
            {
                if(afInBag[iObs] && (aiNodeAssign[iObs] == iNode))
                {
                    dOffset = (adOffset==NULL) ? 0.0 : adOffset[iObs];
                    vecd[iVecd] = adY[iObs] - dOffset - adF[iObs];
                    iVecd++;
                }
            }
            itMedian = vecd.begin() + ((vecd.begin()+iVecd) - vecd.begin())/2;
            nth_element(vecd.begin(), itMedian, (vecd.begin()+iVecd)) ;
            vecpTermNodes[iNode]->dPrediction = *itMedian;
        }
    }

    return hr;
}



double CLaplace::BagImprovement
(
    double *adY,
    double *adMisc,
    double *adOffset,
    double *adWeight,
    double *adF,
    double *adFadj,
    bool *afInBag,
    double dStepSize,
    unsigned long nTrain
)
{
    double dReturnValue = 0.0;
    double dF = 0.0;
    double dW = 0.0;
    unsigned long i = 0;

    for(i=0; i<nTrain; i++)
    {
        if(!afInBag[i])
        {
            dF = adF[i] + ((adOffset==NULL) ? 0.0 : adOffset[i]);
            
            dReturnValue += 
                adWeight[i]*(fabs(adY[i]-dF) - fabs(adY[i]-dF-dStepSize*adFadj[i]));
            dW += adWeight[i];
        }
    }

    return dReturnValue/dW;
}




