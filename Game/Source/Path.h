#pragma once
#ifndef __PATH_H__
#define __PATH_H__

#include "Point.h"

#define MAX_STEPS 25

class Animation;
struct Step
{
	uint framesDuration = 1;
	fPoint speed;
	Animation* animation = nullptr;
};

class Path
{
public:
	// Adds a new frame to the array
	void PushBack(fPoint speed, uint frames, Animation* animation = nullptr);

	// Called from Enemy's Update
	// Increases the frame count and, if necessary, increases step
	void Update();

	// Returns the position relative to the start of the path
	fPoint GetRelativePosition() const;
	// Returns the animation linked to the current step
	Animation* GetCurrentAnimation();
	void SetCurrentAnimation(Animation* _animation);

	// Resets the path to the first step
	void Reset();
	int getTotalSteps() { return totalSteps; }

public:
	// Defines wether the path should go back to the beginning when finished
	bool loop = true;

	// The set of steps that will define the path
	Step steps[MAX_STEPS];
	bool isFinish = false;

private:
	// Total amount of steps
	uint totalSteps = 0;
	// Current frame from the current step
	uint currentStepFrame = 0;

	// The position relative to the start of the path
	fPoint relativePosition = fPoint(0.0f, 0.0f);

	// The current step in the path
	uint currentStep = 0;
};

#endif // __PATH_H__