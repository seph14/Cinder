attribute vec4 aPosition;

uniform mat4 uProjection;
uniform mat4 uModelView;

void main() {
  gl_Position = uProjection * uModelView * aPosition;
}

