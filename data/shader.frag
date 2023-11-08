#ifdef GL_ES
precision mediump float;
#endif
 
uniform vec2 u_resolution;
uniform vec2 u_constant;
uniform vec3 u_mouse;
uniform float u_time;
uniform float zoom_factor;
uniform int fractal_type;
uniform float precision;
uniform float threshold;
vec2 squareComplex(vec2 complex) {
    return vec2(complex.x * complex.x - complex.y*complex.y, 2.0*complex.x*complex.y);
}

float absComplex(vec2 complex) {
    return sqrt(complex.x*complex.x + complex.y*complex.y);
}

void color(int iters, float val) {
    gl_FragColor = vec4(iters/precision, 0.0,0.0, 1.0);
}
void compute_fractal(vec2 c) {
    vec2 f = u_constant; //u_constant defined in cpp.
    vec2 z;
    if (fractal_type == 0) {
        z = f; //Mandelbrot case
    } else {
        z = c; //Julia case
        c = f;
    }
    for (int x = 0; x < precision; x ++) {
        z = squareComplex(z) + c;
        float a = absComplex(z);
        if (a > threshold) {
            color(x, a);
            return;
        } 
    }
    gl_FragColor = vec4(0.0,0.0,0.0,0.0);
    
}


void main() {
    vec2 st = gl_FragCoord.st/u_resolution;
    vec2 c = vec2(st.x, 1-st.y);
    c = (c * 2)-1; // going from 0;1 to -1:1
    c = c / zoom_factor;
    compute_fractal(c);
} 
