// GBM by Greg Ridgeway  Copyright (C) 2003

#include "bernoulli.h"

CBernoulli::CBernoulli()
{
}

CBernoulli::~CBernoulli()
{
}


HRESULT CBernoulli::ComputeWorkingResponse
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
    double dProb = 0.0;
    double dF = 0.0;

    assert(adY != NULL);
    assert(adF != NULL);
    assert(adZ != NULL);
    assert(adWeight != NULL);

    for(i=0; i<nTrain; i++)
    {
        dF = adF[i] + ((adOffset==NULL) ? 0.0 : adOffset[i]);
        dProb = 1.0/(1.0+exp(-dF));

        adZ[i] = adY[i] - dProb;
    }

    return S_OK;
}



HRESULT CBernoulli::InitF
(
    double *adY,
    double *adMisc,
    double *adOffset,
    double *adWeight,
    double &dInitF, 
    unsigned long cLength
)
{
    HRESULT hr = S_OK;

    unsigned long i=0;
    double dTemp=0.0;

    assert(adY != NULL);

    if(adOffset==NULL)
    {
        double dSum=0.0;
        for(i=0; i<cLength; i++)
        {
            dSum += adWeight[i]*adY[i];
            dTemp += adWeight[i];
        }
        dInitF = log(dSum/(dTemp-dSum));
    }
    else
    { 
        // Newton method for solving for F
        // should take about 3-6 iterations.
        double dNum=0.0;         // numerator
        double dDen=0.0;         // denominator
        double dNewtonStep=1.0;  // change
        dInitF = 0.0;
        while(dNewtonStep > 0.0001)
        {
            dNum=0.0;
            dDen=0.0;
            for(i=0; i<cLength; i++)
            {
                dTemp = 1.0/(1.0+exp(-(adOffset[i] + dInitF)));
                dNum += adWeight[i]*(adY[i]-dTemp);
                dDen += adWeight[i]*dTemp*(1.0-dTemp);
            }
            dNewtonStep = dNum/dDen;
            dInitF += dNewtonStep;
        }
    }

    return hr;
}



double CBernoulli::LogLikelihood
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
    double dF = 0.0;

    assert(adY != NULL);
    assert(adF != NULL);
    
    for(i=0; i<cLength; i++)
    {
        dF = adF[i] + ((adOffset==NULL) ? 0.0 : adOffset[i]);
        dL += adWeight[i]*(adY[i]*dF - log(1.0+exp(dF)));
    }

    return dL;
}


HRESULT CBernoulli::FitBestConstant
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
    HRESULT hr = S_OK;

    unsigned long iObs = 0;
    unsigned long iNode = 0;
    vecdNum.resize(cTermNodes);
    vecdNum.assign(vecdNum.size(),0.0);
    vecdDen.resize(cTermNodes);
    vecdDen.assign(vecdDen.size(),0.0);

    for(iObs=0; iObs<nTrain; iObs++)
    {
        if(afInBag[iObs])
        {
            vecdNum[aiNodeAssign[iObs]] += adW[iObs]*adZ[iObs];
            vecdDen[aiNodeAssign[iObs]] += 
                adW[iObs]*(adY[iObs]-adZ[iObs])*(1-adY[iObs]+adZ[iObs]);
        }
    }

    for(iNode=0; iNode<cTermNodes; iNode++)
    {
        if(vecpTermNodes[iNode]!=NULL)
        {
            if(vecdDen[iNode] == 0)
            {
                vecpTermNodes[iNode]->dPrediction = 0.0;
            }
            else
            {
                vecpTermNodes[iNode]->dPrediction = 
                    vecdNum[iNode]/vecdDen[iNode];
            }
        }
    }

    return hr;
}


double CBernoulli::BagImprovement
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
    unsigned long i = 0;

    for(i=0; i<nTrain; i++)
    {
        if(!afInBag[i])
        {
            dF = adF[i] + ((adOffset==NULL) ? 0.0 : adOffset[i]);

            if(adY[i]==1.0)
            {
                dReturnValue += adWeight[i]*dStepSize*adFadj[i];
            }
            dReturnValue += adWeight[i]*
                            (log(1.0+exp(dF)) - 
                             log(1.0+exp(dF+dStepSize*adFadj[i])));
        }
    }

    return dReturnValue;
}
