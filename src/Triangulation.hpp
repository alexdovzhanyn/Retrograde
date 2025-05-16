#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include <list>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

namespace Retrograde {
  enum AngleTurn {
    Convex,
    Reflex,
    Colinear
  };

  class Triangulation {
    public:
    static std::vector<SDL_Vertex> earClipping(std::vector<SDL_FPoint> vec) {
      std::vector<SDL_Vertex> triangles;

      if (vec.size() < 3) return triangles;
      
      // Ensure polygon points are in CCW order
      if (!isCounterClockwisePoly(vec)) std::reverse(vec.begin(), vec.end());

      std::list<SDL_FPoint> points(vec.begin(), vec.end());
  
      // If we have multiple points on the same line just remove them to clean things up and make
      // the geometry more simple
      removeColinearPoints(points);

      while (points.size() > 3) {
        bool earClipped = false;
    
        auto curr = points.begin();
        while (curr != points.end()) {
          auto prev = (curr == points.begin() ? std::prev(points.end()) : std::prev(curr));
          auto next = (curr == std::prev(points.end()) ? points.begin() : std::next(curr));

          const SDL_FPoint &point1 = *prev;
          const SDL_FPoint &point2 = *curr;
          const SDL_FPoint &point3 = *next;

          // We can only clip ears if they are convex triangles
          if (calculateAngleTurn(point1, point2, point3) != AngleTurn::Convex) {
            ++curr;
            continue;
          };
    
          bool isValidEar = true;
          for (auto it = points.begin(); it != points.end(); ++it) {
            if (it == prev || it == curr || it == next) continue;

            // Make sure no other point in the polygon exists within this triangle
            if (isPointWithinTriangle(*it, point1, point2, point3)) {
              isValidEar = false;
              break;
            }
          }
        
          if (!isValidEar) {
            ++curr;
            continue;
          }

          earClipped = true;

          triangles.push_back({ point1, { 0, 0, 0, 1}, { 0, 0 } }); 
          triangles.push_back({ point2, { 0, 0, 0, 1}, { 0, 0 } }); 
          triangles.push_back({ point3, { 0, 0, 0, 1}, { 0, 0 } }); 

          curr = points.erase(curr);
          break;
        }

        if (!earClipped) throw std::runtime_error("Couldn't earclip polygon - no valid ear found");
      }

      // Add the remaining 3 points
      for (auto point : points) {
        triangles.push_back({ point, { 0, 0, 0, 1 }, { 0, 0} });
      }

      return triangles;
    }
    
    private:
    inline static const float EPSILON = 1e-6f;

    static void removeColinearPoints(std::list<SDL_FPoint> &points) {
      bool removedAny = true;
      while (removedAny && points.size() > 3) {
        removedAny = false;
        auto curr = points.begin();
        while (curr != points.end()) {
          auto prev = (curr == points.begin() ? std::prev(points.end()) : std::prev(curr));
          auto next = (curr == std::prev(points.end()) ? points.begin() : std::next(curr));

          if (calculateAngleTurn(*prev, *curr, *next) == AngleTurn::Colinear) {
            curr = points.erase(curr);
            removedAny = true;
          } else {
            ++curr;
          }
        }
      }
    }

    static float calculateSignedArea(const std::vector<SDL_FPoint> &points) {
      float totalArea = 0;

      for (int i = 0; i < points.size(); ++i) {
        const SDL_FPoint &p1 = points[i];
        const SDL_FPoint &p2 = points[(i + 1) % points.size()]; // wrap around at the end
      
        totalArea += (p1.x * p2.y) - (p2.x * p1.y);
      }

      return totalArea * 0.5f;
    }

    static bool isCounterClockwisePoly(const std::vector<SDL_FPoint> &points) {
      return calculateSignedArea(points) > 0;
    }

    // When traversing a polygon in counterclockwise order, any 3 points that make a left turn must be convex,
    // and any 3 points that make a right turn must be reflex (indent into the shape)  
    static AngleTurn calculateAngleTurn(const SDL_FPoint &p1, const SDL_FPoint &p2, const SDL_FPoint &p3) {
      const SDL_FPoint v1 = {p2.x - p1.x, p2.y - p1.y};
      const SDL_FPoint v2 = {p3.x - p1.x, p3.y - p1.y};

      float prod = crossProduct(v1, v2);

      if (prod > EPSILON) return AngleTurn::Convex;
      if (prod < -EPSILON) return AngleTurn::Reflex;
      return AngleTurn::Colinear;
    }

    static bool isPointWithinTriangle(const SDL_FPoint &point, const SDL_FPoint &a, const SDL_FPoint &b, const SDL_FPoint &c) {
      const SDL_FPoint ab = {b.x - a.x, b.y - a.y};
      const SDL_FPoint ap = {point.x - a.x, point.y - a.y};
      const SDL_FPoint bc = {c.x - b.x, c.y - b.y};
      const SDL_FPoint bp = {point.x - b.x, point.y - b.y};
      const SDL_FPoint ca = {a.x - c.x, a.y - c.y};
      const SDL_FPoint cp = {point.x - c.x, point.y - c.y};

      float cross1 = crossProduct(ab, ap); 
      float cross2 = crossProduct(bc, bp);
      float cross3 = crossProduct(ca, cp); 

      if (std::abs(cross1) < EPSILON) cross1 = 0;
      if (std::abs(cross2) < EPSILON) cross2 = 0;
      if (std::abs(cross3) < EPSILON) cross3 = 0;

      // We check to see if any of the cross products are indicating that the point is to a specific side.
      // A negative crossproduct indicates that the point is within the reflex of the angle. A positive
      // indicates that the point is in the convex edge of that angle. A point can not be both within 
      // all reflexive angles of a triangle AND be on the convex end of an angle at the same time, so we
      // know that as long as we can have at least one angle of the triangle indicate that the point is
      // in its reflexive side while another angle is indicating that the point is in its convex side,
      // the point MUST NOT be within the triangle
      bool has_neg = (cross1 < 0) || (cross2 < 0) || (cross3 < 0);
      bool has_pos = (cross1 > 0) || (cross2 > 0) || (cross3 > 0);

      return !(has_neg && has_pos);
    }

    static float crossProduct(const SDL_FPoint &v1, const SDL_FPoint &v2) {
      return (v1.x * v2.y) - (v1.y * v2.x); 
    }
  };
}
