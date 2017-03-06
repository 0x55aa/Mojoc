/*
 *
 *
 *  Created on: 2014-1-16
 *      Author: scott.cgi
 */

#include <malloc.h>

#include "Engine/Toolkit/Define/Def.h"
#include "Engine/Application/Scheduler.h"
#include "Engine/Toolkit/Platform/Log.h"
#include "Engine/Toolkit/Utils/ArrayIntMap.h"


static ArrayList(Scheduler*) schedulerRun [1] = AArrayListInit(Scheduler*, 30);
static ArrayList(Scheduler*) schedulerList[1] = AArrayListInit  (Scheduler*, 30);


static inline Scheduler* GetScheduler(SchedulerUpdate Update, float intervalTime, void* userData)
{
    ALogA(Update != NULL, "schedule SchedulerUpdateFunction must not NULL");

    Scheduler* scheduler = AArrayListPop(schedulerList, Scheduler*);

    if (scheduler == NULL)
    {
        scheduler = (Scheduler*) malloc(sizeof(Scheduler));
    }

    scheduler->userData     = userData;
    scheduler->currentTime  = 0.0f;
    scheduler->intervalTime = intervalTime;
    scheduler->Update       = Update;

	AArrayListAdd(schedulerRun, scheduler);

    return scheduler;
}

static Scheduler* Schedule(SchedulerUpdate Update, float intervalTime, void* userData)
{
    Scheduler* scheduler = GetScheduler(Update, intervalTime, userData);
    scheduler->isCancel  = false;
    return scheduler;
}

static Scheduler* ScheduleOnce(SchedulerUpdate Update, float intervalTime, void* userData)
{
    Scheduler* scheduler = GetScheduler(Update, intervalTime, userData);
    scheduler->isCancel  = true;
    return scheduler;
}

static void Update(float deltaTime)
{
	for (int i = schedulerRun->size - 1; i > -1; i--)
	{
		Scheduler* scheduler = AArrayListGet(schedulerRun, i, Scheduler*);

		if (scheduler->currentTime < scheduler->intervalTime)
		{
			scheduler->currentTime += deltaTime;
		}
		else
		{
			scheduler->Update(scheduler, deltaTime);

			if (scheduler->isCancel)
			{
				AArrayList->RemoveByLast(schedulerRun, i);
				AArrayListAdd(schedulerList, scheduler);
			}
			else
			{
				scheduler->currentTime = 0.0f;
			}
		}
	}
}


struct AScheduler AScheduler[1] =
{
	Schedule,
	ScheduleOnce,
	Update,
};