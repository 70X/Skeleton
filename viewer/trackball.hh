
#ifndef __TRACKBALL_H
#define __TRACKBALL_H

void startTrackball (long x, long y, long originX, long originY,
		     long width, long height);
void rollToTrackball (long x, long y, float rot [4]); // rot is output rotation angle
void addToRotationTrackball (float * dA, float * A);

#endif
