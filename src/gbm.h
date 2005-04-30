//------------------------------------------------------------------------------
//  GBM by Greg Ridgeway  Copyright (C) 2003
//
//  File:       gbm.h
//
//  License:    GNU GPL (version 2 or later)
//
//  Contents:   Entry point for gbm.dll
//        	  
//  Owner:      gregr@rand.org
//
//  History:    2/14/2003   gregr created
//
//------------------------------------------------------------------------------

#include<vector>
#include "dataset.h"
#include "distribution.h"
#include "bernoulli.h"
#include "adaboost.h"
#include "poisson.h"
#include "gaussian.h"
#include "coxph.h"
#include "laplace.h"
#include "gbm_engine.h"

typedef vector<char> VEC_CATEGORIES;
typedef vector<VEC_CATEGORIES> VEC_VEC_CATEGORIES;

GBMRESULT gbm_setup
(
    double *adY,
    double *adOffset,
    double *adX,
    int *aiXOrder,
    double *adWeight,
    double *adMisc,
    int cRows,
    int cCols,
    int *acVarClasses,
    int *alMonotoneVar,
    char *pszFamily,
    int cTrees,
    int cLeaves,
    int cMinObsInNode,
    double dShrinkage,
    double dBagFraction,
    int cTrain,
    CDataset *pData,
    PCDistribution &pDist
);


GBMRESULT gbm_transfer_to_R
(
    CGBM *pGBM,
    VEC_VEC_CATEGORIES &vecSplitCodes,
    int *aiSplitVar,
    double *adSplitPoint,
    int *aiLeftNode,
    int *aiRightNode,
    int *aiMissingNode,
    double *adErrorReduction,
    double *adWeight,
    double *adPred,
    int cCatSplitsOld
);


GBMRESULT gbm_transfer_catsplits_to_R
(
    int iCatSplit,
    VEC_VEC_CATEGORIES &vecSplitCodes,
    int *aiSplitCodes
);


int size_of_vector
(
    VEC_VEC_CATEGORIES &vec,
    int i
);


