#include "ReactiveAdaptationManager3.h"
#include "managers/adaptation/UtilityScorer.h"
#include "managers/execution/AllTactics.h"

using namespace std;

Define_Module(ReactiveAdaptationManager3);

Tactic* ReactiveAdaptationManager3::evaluate() {
    MacroTactic* pMacroTactic = new MacroTactic;
        Model* pModel = getModel();
        const double dimmerStep = 0.2;
        double dimmer = pModel->getDimmerFactor();
        bool isServerBooting = pModel->getServers() > pModel->getActiveServers();
        int noOfActiveServers = pModel->getActiveServers();
        int maxSeverCount = pModel->getMaxServers();
        double basicArrivalRate = pModel->getObservations().basicArrivalRate;
        double avgArrivalRate = pModel->getObservations().avgArrivalRate;
        double servUtil = pModel->getActiveServers() - pModel->getObservations().utilization;
        double responseTime = pModel->getObservations().avgResponseTime;
        bool reduceDimmerFlag = true;

        if(basicArrivalRate > 50 && noOfActiveServers == 2 ){
            if (dimmer > 0.4){
                        dimmer = max(0.4, dimmer-dimmerStep);
                        pMacroTactic->addTactic(new SetDimmerTactic(dimmer));
            }
            else if (!isServerBooting && pModel->getServers() < pModel->getMaxServers()) {
                pMacroTactic->addTactic(new AddServerTactic);
            }
            return pMacroTactic;
        }

        if(responseTime > RT_THRESHOLD){ //we have higher response rate
            //first try to reduce the dimmer value
            if (dimmer > 0.4){
                dimmer = max(0.4, dimmer-dimmerStep);
                pMacroTactic->addTactic(new SetDimmerTactic(dimmer));

                if(responseTime > RT_THRESHOLD * 2){
                    if (!isServerBooting && pModel->getServers() < pModel->getMaxServers()) {
                        pMacroTactic->addTactic(new AddServerTactic);
                        dimmer = 0.2;
                        pMacroTactic->addTactic(new SetDimmerTactic(dimmer));
                        return pMacroTactic;

                    }
                }
            }else{
                if (!isServerBooting && pModel->getServers() < pModel->getMaxServers()) {
                    pMacroTactic->addTactic(new AddServerTactic);
                }
                else{
                    dimmer = max(0.4 ,dimmer -dimmerStep);
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
            if(responseTime > RT_THRESHOLD / 2){
                dimmer = 1.0;
                pMacroTactic->addTactic(new SetDimmerTactic(dimmer));
            }
        }
        return pMacroTactic;
}
