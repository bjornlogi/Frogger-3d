
varying  vec4 color;

varying vec2 texCoord;

uniform sampler2D texture;
uniform vec4 cubecolor;

void main() 
{ 
    //gl_FragColor = cubecolor * texture2D( texture, texCoord );
    gl_FragColor = color;
    //gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
} 