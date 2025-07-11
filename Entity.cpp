//
// Created by DirgeWuff on 6/27/25.
//

#include "Entity.h"
#include "box2d/box2d.h"

Entity::Entity (
    const float centerX,
    const float centerY,
    const float fullWidth,
    const float fullHeight,
    const bodyConfig config,
    const b2WorldId world) :
BoxBody(
    centerX,
    centerY,
    fullWidth,
    fullHeight,
    bodyConfig{b2_dynamicBody, false, 8.0f, 0.50f},
    world)
{

}
