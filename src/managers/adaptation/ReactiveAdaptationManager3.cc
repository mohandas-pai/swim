/*******************************************************************************
 * Simulator of Web Infrastructure and Management
 * Copyright (c) 2016 Carnegie Mellon University.
 * All Rights Reserved.
 *  
 * THIS SOFTWARE IS PROVIDED "AS IS," WITH NO WARRANTIES WHATSOEVER. CARNEGIE
 * MELLON UNIVERSITY EXPRESSLY DISCLAIMS TO THE FULLEST EXTENT PERMITTED BY LAW
 * ALL EXPRESS, IMPLIED, AND STATUTORY WARRANTIES, INCLUDING, WITHOUT
 * LIMITATION, THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, AND NON-INFRINGEMENT OF PROPRIETARY RIGHTS.
 *  
 * Released under a BSD license, please see license.txt for full terms.
 * DM-0003883
 *******************************************************************************/

#include "ReactiveAdaptationManager3.h"
#include "managers/adaptation/UtilityScorer.h"
#include "managers/execution/AllTactics.h"

using namespace std;

Define_Module(ReactiveAdaptationManager3);

/**
 * Reactive adaptation
 *
 * RT = response time
 * RTT = response time threshold
 *
 * - if RT > RTT, add a server if possible, if not decrease dimmer if possible
 * - if RT < RTT
 *      -if dimmer < 1, increase dimmer else if servers > 1 and no server booting remove server
 */
Tactic* ReactiveAdaptationManager3::evaluate() {
    MacroTactic* pMacroTactic = new MacroTactic;
    Model* pModel = getModel();
    const double dimmerStep = 1.0 / (pModel->getNumberOfDimmerLevels() - 1);
    double dimmer = pModel->getDimmerFactor();
    bool isServerBooting = pModel->getServers() > pModel->getActiveServers();
    int noOfActiveServers = pModel->getActiveServers();
    int maxSeverCount = pModel->getMaxServers();
    double avgArrivalRate = pModel->getArrivalRate();
    double responseTime = pModel->getObservations().avgResponseTime;

    if(responseTime > RT_THRESHOLD){ //we have higher response rate
        if(avgArrivalRate < ARR_THRESHOLD){ //we have lesser arrival rate so it is better to reduce dimmer value
            dimmer = dimmer - 0.25;
            pMacroTactic->addTactic(new SetDimmerTactic(dimmer));
        }
        else{ //we have higer traffic so we should add server.(we can also check if reducing dimmer will help)
            if(pModel->getServers() < pModel->getMaxServers()){
                pMacroTactic->addTactic(new AddServerTactic);
            }
            if(avgArrivalRate - ARR_THRESHOLD > 10){ //10 is a random number now; it should notify some very high arrival rate;
                dimmer = dimmer - 0.5;
                pMacroTactic->addTactic(new SetDimmerTactic(dimmer));
            }
        }
    }else{ //we have low response rate, so we first try to increase the dimmer factor by one
        dimmer = dimmer + 0.25;
        pMacroTactic->addTactic(new SetDimmerTactic(dimmer));
        if(pModel->getServers() < noOfActiveServers){ // we have more servers so we either remove server or increase dimmer factor
            //TODO: Need a good If loop here
            pMacroTactic->addTactic(new RemoveServerTactic);
            dimmer = dimmer + 0.25;
            pMacroTactic->addTactic(new SetDimmerTactic(dimmer));
        }

    }
    return pMacroTactic;
}
