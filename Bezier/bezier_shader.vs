uniform int n = 0;
uniform int controlPointsSize = 0;
varying vec2 controlPoints[10];

void interpolate(float time, int begin, int end, int size)
{
	vec2[10][10] tempPoints;	
	for (int j = 0; j < size; j++)
	{
		tempPoints[0][j] = controlPoints[j];
	}
	for (int i = 1; i < size; i++)
	{
		for (int j = 0; j < size-i; j++)
		{
			tempPoints[i][j] = time*tempPoints[i-1][j+1] + (1-time)*tempPoints[i-1][j];
		}
	}
	gl_Position = gl_ModelViewProjectionMatrix * vec4( tempPoints[size-1][0], 0.0, 1.0 );
	//glVertex2d(xTab[size*(size-1)], yTab[size*(size-1)]);
	//DRAW
}

void main()
{
	if (n >= 2 && controlPointsSize >= 2) {
		int begin = 0;
		int end = controlPointsSize - 1;
		for (int i = 0; i < n; ++i) {
			float t = i/(n-1);
			interpolate(t, begin, end, controlPointsSize);
		}
	}
}