uniform sampler2D texture;
float darkness = 0.5;

void main(){
    vec4 Color = texture2D(texture, gl_TexCoord[0].xy);
	gl_FragColor = vec4(Color.r*darkness, Color.g*darkness, Color.b*darkness, Color.a);
}