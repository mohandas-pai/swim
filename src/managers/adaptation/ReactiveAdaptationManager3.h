/*
 * ReactiveAdaptationManager3.h
 *
 *  Created on: Dec 27, 2021
 *      Author: root
 */

#ifndef MANAGERS_ADAPTATION_REACTIVEADAPTATIONMANAGER3_H_
#define MANAGERS_ADAPTATION_REACTIVEADAPTATIONMANAGER3_H_


#include "BaseAdaptationManager.h"

/**
 * Simple reactive adaptation manager (alternative 3)
 */
class ReactiveAdaptationManager3 : public BaseAdaptationManager
{
  protected:
    virtual Tactic* evaluate();
};

#endif /* MANAGERS_ADAPTATION_REACTIVEADAPTATIONMANAGER3_H_ */
