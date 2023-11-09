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
uniform vec2 offset;
vec2 squareComplex(vec2 complex) {
    return vec2(complex.x * complex.x - complex.y*complex.y, 2.0*complex.x*complex.y);
}

float absComplex(vec2 complex) {
    return sqrt(complex.x*complex.x + complex.y*complex.y);
}

void color(int iters, float val) {
    vec4 gradients[17];
    gradients[0] = vec4(66.0, 30.0, 15.0, 255.0);
    gradients[1] = vec4(25.0, 7.0, 26.0, 255.0);
    gradients[3] = vec4(9.0, 1.0, 47.0, 255.0);
    gradients[4] = vec4(4.0, 4.0, 73.0, 255.0);
    gradients[5] = vec4(0.0, 7.0, 100.0, 255.0);
    gradients[6] = vec4(12.0, 44.0, 138.0, 255.0);
    gradients[7] = vec4(24.0, 82.0, 177.0, 255.0);
    gradients[8] = vec4(57.0, 125.0, 209.0, 255.0);
    gradients[9] = vec4(134.0, 181.0, 229.0, 255.0);
    gradients[10] = vec4(211.0, 236.0, 248.0, 255.0);
    gradients[11] = vec4(241.0, 233.0, 191.0, 255.0);
    gradients[12] = vec4(248.0, 201.0, 95.0, 255.0);
    gradients[13] = vec4(255.0, 170.0, 0.0, 255.0);
    gradients[14] = vec4(204.0, 128.0, 0.0, 255.0);
    gradients[15] = vec4(153.0, 87.0, 0.0, 255.0);
    gradients[16] = vec4(106.0, 52.0, 3.0, 255.0);


    
    gl_FragColor = gradients[iters % 16] / 255.0;
}
void compute_fractal(vec2 c) {
    vec2 f = u_constant; //u_constant defined in cpp.
    vec2 z;
    if (fractal_type != 1) {
        z = f; //Mandelbrot case and Burning ship
    } else {
        z = c; //Julia case
        c = f;
    }
    for (int x = 0; x < precision; x ++) {
        if (fractal_type == 2) { //Burning ship
            z.x = abs(z.x);
            z.y = abs(z.y);
        }
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
    vec2 st = (gl_FragCoord.st - u_resolution*0.5) / (zoom_factor);
    st = (st + offset)/u_resolution;
    compute_fractal(st);
} 
