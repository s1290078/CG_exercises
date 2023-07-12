#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <float.h>
#include <stdlib.h>

#include "Scene.h"
#include "RayTrace.h"
#include "Geometry.h"


// Clamp c's entries between low and high. 
static void clamp(Color* c, float low, float high) {
    c->_red = fminf(fmaxf(c->_red, low), high);
    c->_green = fminf(fmaxf(c->_green, low), high);
    c->_blue = fminf(fmaxf(c->_blue, low), high);
}


// Complete
// Given a ray (origin, direction), check if it intersects a given
// sphere.
// Return 1 if there is an intersection, 0 otherwise.
// *t contains the distance to the closest intersection point, if any.
static int
hitSphere(Vector3 origin, Vector3 direction, Sphere sphere, float* t)
{
    Vector3 oc;
    float a, b, c, d;
    sub(origin, sphere._center, &oc); // origin - center 
    computeDotProduct(direction, direction, &a); // |direction|^2
    computeDotProduct(direction, oc, &b); 
    b *= 2.f; // 2*(oc . direction)
    computeDotProduct(oc, oc, &c);
    c -= sphere._radius * sphere._radius; // |oc|^2 - radius^2
    d = b * b - 4.f * a * c; // discriminant 
    if (d < 0) return 0; // no real roots, so no intersection 
    *t = (-b - sqrtf(d)) / (2.f * a);
    return (*t > 0 ? 1 : 0); // return true if intersection 
}

// Check if the ray defined by (scene._camera, direction) is intersecting
// any of the spheres defined in the scene.
// Return 0 if there is no intersection, and 1 otherwise.
//
// If there is an intersection:
// - the position of the intersection with the closest sphere is computed 
// in hit_pos
// - the normal to the surface at the intersection point in hit_normal
// - the diffuse color and specular color of the intersected sphere
// in hit_color and hit_spec
static int
hitScene(Vector3 origin, Vector3 direction, Scene scene,
    Vector3* hit_pos, Vector3* hit_normal,
    Color* hit_color, Color* hit_spec)
{
    Vector3 o = origin;
    Vector3 d = direction;

    float t_min = FLT_MAX;
    int hit_idx = -1;
    Sphere hit_sph;

    // For each sphere in the scene
    int i;
    for (i = 0; i < scene._number_spheres; ++i) {
        Sphere curr = scene._spheres[i];
        float t = 0.0f;
        if (hitSphere(o, d, curr, &t)) {
            if (t < t_min) {
                hit_idx = i;
                t_min = t;
                hit_sph = curr;
            }
        }
    }

    if (hit_idx == -1) return 0;

    Vector3 td;
    mulAV(t_min, d, &td);
    add(o, td, hit_pos);

    Vector3 n;
    sub(*hit_pos, hit_sph._center, &n);
    mulAV(1.0f / hit_sph._radius, n, hit_normal);

    // Save the color of the intersected sphere in hit_color and hit_spec
    *hit_color = hit_sph._color;
    *hit_spec = hit_sph._color_spec;

    return 1;
}

// Save the image in a raw buffer (texture)
// The memory for texture is allocated in this function. It needs to 
// be freed in the caller.
static void saveRaw(Color** image, int width, int height, GLubyte** texture) {
    int count = 0;
    int i;
    int j;
    *texture = (GLubyte*)malloc(sizeof(GLubyte) * 3 * width * height);

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            unsigned char red = (unsigned char)(image[i][j]._red * 255.0f);
            unsigned char green = (unsigned char)(image[i][j]._green * 255.0f);
            unsigned char blue = (unsigned char)(image[i][j]._blue * 255.0f);

            (*texture)[count] = red;
            count++;

            (*texture)[count] = green;
            count++;

            (*texture)[count] = blue;
            count++;
        }
    }
}


// Complete
// Given an intersection point (hit_pos),
// the normal to the surface at the intersection point (hit_normal),
// and the color (diffuse and specular) terms at the intersection point,
// compute the colot intensity at the point by applying the Phong
// shading model.
// Return the color intensity in *color.
static void
shade(Vector3 hit_pos, Vector3 hit_normal,
    Color hit_color, Color hit_spec, Scene scene, Color* color)
{
    // Complete
    // ambient component
    color->_red = scene._ambient._red;
    color->_green = scene._ambient._green;
    color->_blue = scene._ambient._blue;

    // for each light in the scene
    int l;
    for (l = 0; l < scene._number_lights; l++) {
        // Complete
        // Form a shadow ray and check if the hit point is under
        // direct illumination from the light source
        Vector3 L;
        sub(scene._lights[l]._light_pos, hit_pos, &L);
        normalize(L, &L);
        
        Vector3 tmp;
        Vector3 tmp2;
        Color tmp_color;
        Color tmp_color2;
        int hit = hitScene(hit_pos, L, scene, &tmp, &tmp, &tmp_color, &tmp_color);
        if (hit) continue;
        
        // Complete
        // diffuse component
        GLfloat LN;
        computeDotProduct(L, hit_normal, &LN);

        color->_red += scene._lights[l]._light_color._red * hit_color._red * fmaxf(LN, 0.f);
        color->_green += scene._lights[l]._light_color._green * hit_color._green * fmaxf(LN, 0.f);
        color->_blue += scene._lights[l]._light_color._blue * hit_color._blue * fmaxf(LN, 0.f);
        
        // Complete
        // specular component
        Vector3 V, R, pL;
        sub(scene._camera, hit_pos, &V);
        normalize(V, &V);
        sub(hit_pos, scene._lights->_light_pos, &pL);
        normalize(pL, &pL);
        GLfloat IN;
        computeDotProduct(pL, hit_normal, &IN);
        R._x = pL._x - 2 * IN * hit_normal._x;
        R._y = pL._y - 2 * IN * hit_normal._y;
        R._z = pL._z - 2 * IN * hit_normal._z;

        GLfloat VR;
        computeDotProduct(V, R, &VR);
        GLfloat mp;
        GLfloat shiny = 64.f;
        mp = pow(fmaxf(VR, 0.f), shiny);
        color->_red += hit_spec._red * scene._lights[l]._light_color._red * mp;
        color->_green += hit_spec._red * scene._lights[l]._light_color._green * mp;
        color->_blue += hit_spec._red * scene._lights[l]._light_color._blue * mp;

        color->_red = fminf(fmaxf(color->_red, 0.f), 1.f);
        color->_green = fminf(fmaxf(color->_green, 0.f), 1.f);
        color->_blue = fminf(fmaxf(color->_blue, 0.f), 1.f);
    }
}


static void rayTrace(Vector3 origin, Vector3 direction_normalized,
    Scene scene, Color* color)
{
    Vector3 hit_pos;
    Vector3 hit_normal;
    Color hit_color;
    Color hit_spec;
    int hit1;

    Vector3 hit_pos2;
    Vector3 hit_normal2;
    Color hit_color2;
    Color hit_spec2;
    int hit2;

    // does the ray intersect an object in the scene?
    hit1 =
        hitScene(origin, direction_normalized, scene,
            &hit_pos, &hit_normal, &hit_color,
            &hit_spec);
    
    // no hit
    if (!hit1) {
        color->_red = scene._background_color._red;
        color->_green = scene._background_color._green;
        color->_blue = scene._background_color._blue;
        return;
    }

    // otherwise, apply the shading model at the intersection point
    shade(hit_pos, hit_normal, hit_color, hit_spec, scene, color);
}


void rayTraceScene(Scene scene, int width, int height, GLubyte** texture) {
    Color** image;
    int i;
    int j;

    Vector3 camera_pos;
    float screen_scale;

    image = (Color**)malloc(height * sizeof(Color*));
    for (i = 0; i < height; i++) {
        image[i] = (Color*)malloc(width * sizeof(Color));
    }

    // get parameters for the camera position and the screen fov
    camera_pos._x = scene._camera._x;
    camera_pos._y = scene._camera._y;
    camera_pos._z = scene._camera._z;

    screen_scale = scene._scale;

    // go through each pixel
    // and check for intersection between the ray and the scene
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            // Compute (x,y) coordinates for the current pixel 
            // in scene space
            float x = screen_scale * j - 0.5f * screen_scale * width;
            float y = screen_scale * i - 0.5f * screen_scale * height;
            
            int num_rays = 10;
            Color accumulated_color = {0.f, 0.f, 0.f};
            for (int ray = 0; ray < num_rays; ray++) {
                // Generate random offsets within the pixel
                float random_offset = ((float)rand() / RAND_MAX) * (screen_scale) - screen_scale / 2.0f;
                float x_offset = x + random_offset;
                float y_offset = y + random_offset;

                // Form the vector camera to current pixel
                Vector3 direction;
                Vector3 direction_normalized;

                direction._x = x_offset - camera_pos._x;
                direction._y = y_offset - camera_pos._y;
                direction._z = -camera_pos._z;

                normalize(direction, &direction_normalized);

                Vector3 origin = scene._camera;
                Color color;
                color._red = 0.f;
                color._green = 0.f;
                color._blue = 0.f;
                rayTrace(origin, direction_normalized, scene, &color);

                accumulated_color._red += color._red;
                accumulated_color._green += color._green;
                accumulated_color._blue += color._blue;
            }

            // Average color
            accumulated_color._red /= num_rays;
            accumulated_color._green /= num_rays;
            accumulated_color._blue /= num_rays;

            // Gamma 
            accumulated_color._red = accumulated_color._red * 1.1f - 0.02f;
            accumulated_color._green = accumulated_color._green * 1.1f - 0.02f;
            accumulated_color._blue = accumulated_color._blue * 1.1f - 0.02f;
            clamp(&accumulated_color, 0.f, 1.f);
            accumulated_color._red = powf(accumulated_color._red, 0.4545f);
            accumulated_color._green = powf(accumulated_color._green, 0.4545f);
            accumulated_color._blue = powf(accumulated_color._blue, 0.4545f);

            // Contrast 
            accumulated_color._red = accumulated_color._red * accumulated_color._red * (3.f - 2.f*accumulated_color._red);
            accumulated_color._green = accumulated_color._green * accumulated_color._green * (3.f - 2.f*accumulated_color._green);
            accumulated_color._blue = accumulated_color._blue * accumulated_color._blue * (3.f - 2.f*accumulated_color._blue);

            image[i][j] = accumulated_color;
        }
    }

    // save image to texture buffer
    saveRaw(image, width, height, texture);

    for (i = 0; i < height; i++) {
        free(image[i]);
    }

    free(image);
}