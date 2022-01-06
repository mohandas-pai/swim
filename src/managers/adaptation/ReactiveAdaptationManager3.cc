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
    double servUtil = pModel->getActiveServers() - pModel->getObservations().utilization;
    double responseTime = pModel->getObservations().avgResponseTime;
    bool reduceDimmerFlag = true;

    if(responseTime > RT_THRESHOLD){ //we have higher response rate
        //first try to reduce the dimmer value
        while(reduceDimmerFlag){
            dimmer = max(0.2, dimmer-dimmerStep);
            pModel->addTactic(new SetDimmerTactic(dimmer));
            if(dimmer = 0.2 && responseTime > RT_THRESHOLD){
                reduceDimmerFlag = false;
            }
        }
        if(reduceDimmerFlag = false){
            //add server
            if (!isServerBooting && pModel->getServers() < pModel->getMaxServers()) {
                pMacroTactic->addTactic(new AddServerTactic);
                reduceDimmerFLag = true;
                //should now increase dimmer value
            }
        }

    }else{  //we have low response time
        // first remove server
        if (spareUtilization > 1) {
            if (!isServerBooting && pModel->getServers() > 1) {
                pMacroTactic->addTactic(new RemoveServerTactic);
            }
        // if still low then increase dimmer to 0.75 or 1
        }else{
            dimmer = min(1.0, dimmer + dimmerStep);
            pMacroTactic->addTactic(new SetDimmerTactic(dimmer));
        }
    }
}

