//
// Created by DirgeWuff on 8/8/2025.
//

#include "box2d/box2d.h"
#include "EventCollider.h"

void EventCollider::disableCollider() const noexcept {
    b2Body_Disable(m_body);
}

[[nodiscard]] Vector2 EventCollider::getSizePx() const noexcept {
    return m_sizePx;
}

[[nodiscard]] Vector2 EventCollider::getPosPixels() const noexcept {
    return m_cornerPosition;
}



