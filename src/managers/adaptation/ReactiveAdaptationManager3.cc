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
    const double dimmerStep = 0.2;
    double dimmer = pModel->getDimmerFactor();
    bool isServerBooting = pModel->getServers() > pModel->getActiveServers();
    int noOfActiveServers = pModel->getActiveServers();
    int maxSeverCount = pModel->getMaxServers();
    //double avgArrivalRate = pModel->getArrivalRate();
    double servUtil = pModel->getActiveServers() - pModel->getObservations().utilization;
    double responseTime = pModel->getObservations().avgResponseTime;
    bool reduceDimmerFlag = true;


    if(responseTime > RT_THRESHOLD){ //we have higher response rate
        //first try to reduce the dimmer value
        if (dimmer > 0.4){
            dimmer = max(0.4, dimmer-dimmerStep);
            pMacroTactic->addTactic(new SetDimmerTactic(dimmer));

            if(responseTime > RT_THRESHOLD * 2){
                if (!isServerBooting && pModel->getServers() < pModel->getMaxServers()) {
                    pMacroTactic->addTactic(new AddServerTactic);
                }
            }
        }else{
            if (!isServerBooting && pModel->getServers() < pModel->getMaxServers()) {
                pMacroTactic->addTactic(new AddServerTactic);
            }
            else{
                dimmer = max(0.2 ,dimmer -dimmerStep);
                pMacroTactic->addTactic(new SetDimmerTactic(dimmer));
            }
        }
    }else{  //we have low response time
        // first remove server
        dimmer = min(1.0, dimmer + dimmerStep);
        pMacroTactic->addTactic(new SetDimmerTactic(dimmer));
        if(dimmer == 1.0){
            if (servUtil > 1) {
                if (!isServerBooting && pModel->getServers() > 1) {
                    pMacroTactic->addTactic(new RemoveServerTactic);
                }
            }
        }
    }
    return pMacroTactic;
}
