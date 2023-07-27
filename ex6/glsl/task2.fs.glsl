varying vec4 oNormal;
varying vec4 oPosition;

vec2 dispHash(int i, int j) {
    float x = float(i);
    float y = float(j);

    for (int i = 0; i < 2; ++i) {
        x = mod(87.0 * x + 23.0 * y, 257.0);
        y = mod(87.0 * x + 23.0 * y, 1009.0);
    }
    return vec2(x / 257.0, y / 1009.0);
}

vec3 colHash(int i, int j) {
    float r = float(i);
    float g = float(j);
    float b = float(i + j);

    for (int i = 0; i < 2; ++i) {
        r = mod(87.0 * r + 23.0 * g + 125.0 * b, 257.0);
        g = mod(87.0 * r + 23.0 * g + 125.0 * b, 1009.0);
        b = mod(87.0 * r + 23.0 * g + 125.0 * b, 21001.0);
    }
    return vec3(r / 257.0, g / 1009.0, b / 21001.0) ;
}

vec3 vor2d(vec2 pos) {
    float step = 10.0;

    int xi = int(floor(pos.x / step));
    int yj = int(floor(pos.y / step));

    ivec2 nearest = ivec2(0,0);
    float min_dist = 1e5;

    for (int i = xi - 1; i <= xi + 1; ++i) {
        for (int j = yj - 1; j <= yj + 1; ++j) {
            vec2 disp = dispHash(i, j);
            vec2 seed = vec2(
                (float(i) + disp.x) * step,
                (float(j) + disp.y) * step
            );
            float dist = length(pos - seed);
            if (dist < min_dist) {
                min_dist = dist;
                nearest = ivec2(i, j);
            }
        }
    }

    vec3 col = colHash(nearest.x, nearest.y);
    return col;
}

vec2 map(vec3 xyz) {
    vec2 p;
    float xyz_len = sqrt(xyz.x*xyz.x + xyz.y*xyz.y + xyz.z*xyz.z);
    p.x = acos(xyz.x / xyz_len);
    p.y = atan(xyz.y / xyz.z);
    return p;
}

void main(){
    // light position in eye space
  vec4 light2 = vec4(10.0, 5.0, 1.0, 1.0);

  // vertex in eye space
  vec4 V = oPosition;

  // vertex to light direction
  vec4 L = normalize(light2 - V);

  // normal in eye space
  vec4 N = oNormal;

  // apply a (pseudo-random) perturbation to the normal
  // N.xyz = noisemap(N.xyz);
  N.w = 0.0;
  N = normalize(N);

  // material
  vec4 amb = vec4(0.1, 0.1, 0.1, 1.0);
  vec4 diff = vec4(1.4, 0.7, 0.6, 1.0);
  vec4 spec = vec4(1.0, 1.0, 1.0, 1.0);
  vec4 light3 = vec4(-5.0, 3.0, 8.0, 1.0);
  vec4 L2 = normalize(light3 - V);

  float shiny = 2.5;
  
  // light color
  vec4 lcol = vec4(1.0, 1.0, 1.0, 1.0);

  // Phong shading model
  //
  // reflected light vector:
  vec4 R = reflect(-L, N);
  vec4 View = normalize(-V);

  // lighting
  float ndl2 = clamp(dot(N, L2), 0.0, 1.0);
  float vdr2 = pow(clamp(dot(View, reflect(-L2, N)), 0.0, 1.0), shiny);


    vec2 pos = map(oPosition.xyz);
    vec3 col = vor2d(50.0 * pos);
    diff = vec4(col, 1.0);
    gl_FragColor = vec4(col, 1.0);

  vec4 lin = vec4(0.0);
  lin += amb * lcol;
  lin += ndl2 * diff * lcol;
  lin += vdr2 * spec * lcol;

  gl_FragColor = lin;
}