#include "scene.h"

#include <iostream>

void Scene::setArcadeOffsetPosition(glm::mat4 model)
{
	/* v = (x,y,z,1) = (0, 0, 0, 1). */
	glm::vec4 offset(0.0f, 0.0f, 0.0f, 1.0f);
	/* v' = M*v = (x+tx, y+tx, z+tx, 1) = (tx, ty, tz, 1). */
	offset = model * offset;
	/* Implicit casting from vec4 to vec3. */
	m_arcadePosOffset = offset;
}

/* TODO: provare a sostituire distance con lenght2 per evitare i calcoli di radici quadrate
 * confronto: distance(v,w) > eps ------> lenght2(v,w) > eps*eps	dovrebbe essere più efficiente
 * in questo caso non cambierebbe quasi nulla, ma in casi dove per ogni chiamata di funzione ci sono
 * migliaia/milioni di glm::distance allora può fare molta differenza: distance calcola sqrt mentre lenght2 no
 * e sqrt è computazionalmente intensivo per la CPU. */
bool Scene::cameraAnimation()
{
	/* Retrieve current position and front vectors. */
	glm::vec3 currPos = cam3D.getPosition();
	glm::vec3 currFront = cam3D.getFront();

	/* Retrieve center and global normal vectors of the display. */
	glm::vec3 displayCenter = m_arcade.getModel().getMesh("display").getCenter() + m_arcadePosOffset;
	glm::vec3 displayNormal = m_arcade.getModel().getMesh("display").getGlobalNormal();
	

	/* Intermediate point position. Let it be dC + 3*dN. */
	glm::vec3 midPos = displayCenter + 3.0f * displayNormal;		
	/* The front vector should point at the display center, so it is the opposite of the display normal.*/
	glm::vec3 midFront = -displayNormal;							

	
	/* First time entering in the function, setting starting position and front vectors (Scene struct members). */
	if (!animationIsOn)
	{
		startPos = currPos;
		startFront = currFront;
		animationIsOn = true;
	}
	
	/* ----- Camera animation from starting position and front to the intermediate point position and front. ------ */

	/* Disable fixed height. */
	cam3D.setGrounded(false);
	
	/* Step < 2epsilon to ensure convergence. (epsilon >0.5step) */
	const float step = 0.005;
	float eps = step*0.51;			

	/* Initial vector distances for position and front.*/
	float initPosGap = glm::distance(startPos, midPos);
	float initFrontGap = glm::distance(startFront, midFront);

	/* Direction vectors for the interpolation. */
	glm::vec3 stepPosDir = glm::normalize(midPos - startPos);
	glm::vec3 stepFrontDir = glm::normalize(midFront - startFront);		

	/* Number of steps for both interpolation must be the same. 
	 * Number of steps = initial vector distance / step size.
	 * If one calls Lp: initPosGap, Lf:initFrontGap, STp: stepPos, STf: stepFront, then
	 * Lp/STp = Lf/STf ----> if one parametrizes STp = step, then
	 * if Lp is not zero --> STf = Lf/Lp * STp
	 * else STf = step; */
	float stepPos = step;
	float stepFront;
	if (initPosGap >= eps) stepFront = initFrontGap / initPosGap * stepPos;
	else stepFront = step;

	/* Current position gap. */
	float currPosGap = glm::distance(currPos, midPos);
	/* Position gap after the next step. */
	float stepPosGap = glm::distance(currPos + stepPos * stepPosDir, midPos);
	/* Current front gap. */
	float currFrontGap = glm::distance(currFront, midFront);
	/* Front gap after the next step. */
	float stepFrontGap = glm::distance(currFront + stepFront * stepFrontDir, midFront);

	/* If one of the gaps is still > epsilon the function returns animationIsOn = true, else returns false */
	if( currPosGap >= eps || currFrontGap >= eps )
	{
		/* This is valid for both position and front vectors.
		 * If the current gap is greater than epsilon, check if after a step it's still greater than epsilon.
		 * If it is, make the step, else make the current vector equal to the final one.
		 * Then update the camera vector member value. */

		if (currPosGap >= eps)
		{
			if (stepPosGap >= eps) currPos += stepPos * stepPosDir;
			else currPos = midPos;
			cam3D.setPosition(currPos);

		}
		if (currFrontGap >= eps)
		{
			if (stepFrontGap >= eps) currFront += stepFront * stepFrontDir;
			else currFront = midFront;
			cam3D.setFront(currFront);
		}

		return animationIsOn;
		}
		else
		{
			animationIsOn = false;
			return animationIsOn;
		}
}