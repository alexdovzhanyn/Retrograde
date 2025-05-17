#include "Entity.hpp"

namespace Retrograde {
  class Lander : public Entity {
  public:
    // TODO: Dont use default box collider
    Lander() : Entity(0, 0, "assets/lander.bmp", 50, 50, true) {}
  };
}
