
attribute  vec4 vPosition;

attribute vec2 vTexCoord;

attribute vec4 vColor;
varying vec4 color;
varying vec2 texCoord;

uniform mat4 ModelView;
uniform mat4 Projection;

void main()
{

    gl_Position = Projection * ModelView * vPosition;
    //texCoord = vTexCoord;
    color = vColor;
}