//
// Author: DirgeWuff
// Created on: 1/27/26
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Event structs to be used/processed by EventDispatcher<T>.
// Each struct contains relevant data to the event for identification and
// handling purposes.

#ifndef EVENT_H
#define EVENT_H

struct playerCollisionEvent  {
    bool contactBegan;
    b2ShapeId visitorShape;
    sensorInfo info;
};

#endif //EVENT_H
