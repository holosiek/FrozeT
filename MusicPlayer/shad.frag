uniform sampler2D texture;

void main(){
    vec4 Color = texture2D(texture, gl_TexCoord[0].xy );
    gl_FragColor = vec4( Color.r, Color.g, Color.b, Color.a);
}