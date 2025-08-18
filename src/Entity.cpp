//
// Created by DirgeWuff on 6/27/25.
//

#include "../Entity.h"

Entity::Entity (
    const float centerX,
    const float centerY,
    const float fullWidth,
    const float fullHeight,
    const bodyConfig& config,
    const b2WorldId world) :
BoxBody(
    centerX,
    centerY,
    fullWidth,
    fullHeight,
    config,
    world)
{

}
