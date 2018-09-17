	float pyramid_vertices[] =
	{
		// triangle of front side
		// front-top
		0.0f, 1.0f, 0.0f,
		// front-right
		1.0f, -1.0f, -1.0f,
		// front-left
		-1.0f, -1.0f, -1.0f,

		// triangle of right side
		// right-top
		0.0f, 1.0f, 0.0f,
		// right-right
		1.0f, -1.0f, 1.0f,
		// right-left
		1.0f, -1.0f, -1.0f,

		// triangle of back side
		// back-top
		0.0f, 1.0f, 0.0f,
		// back-right
		-1.0f, -1.0f, 1.0f,
		// back-left
		1.0f, -1.0f, 1.0f,

		// triangle of left side
		// left-top
		0.0f, 1.0f, 0.0f,
		// left-right
		-1.0f, -1.0f, -1.0f,
		// left-left
		-1.0f, -1.0f, 1.0f,
	};

	float pyramid_colors[] =
	{
		// triangle of front side
		// R ( Top )
		1.0f, 0.0f, 0.0f,
		// B
		0.0f, 0.0f, 1.0f,
		// G
		0.0f, 1.0f, 0.0f,

		// triangle of right side
		// R ( Top )
		1.0f, 0.0f, 0.0f,
		// G
		0.0f, 1.0f, 0.0f,
		// B
		0.0f, 0.0f, 1.0f,

		// triangle of back side
		// R ( Top )
		1.0f, 0.0f, 0.0f,
		// B
		0.0f, 0.0f, 1.0f,
		// G
		0.0f, 1.0f, 0.0f,

		// triangle of left side
		// R ( Top )
		1.0f, 0.0f, 0.0f,
		// G
		0.0f, 1.0f, 0.0f,
		// B
		0.0f, 0.0f, 1.0f,
	};

	float pyramid_texcoords[] =
	{
		// triangle of front side
		// front-top
		0.5, 1.0,
		// front-right
		1.0, 0.0,
		// front-left
		0.0, 0.0,

		// triangle of right side
		// right-top
		0.5, 1.0,
		// right-right
		0.0, 0.0,
		// right-left
		1.0, 0.0,

		// triangle of back side
		// back-top
		0.5, 1.0,
		// back-right
		0.0, 0.0,
		// back-left
		1.0, 0.0,

		// triangle of left side
		// left-top
		0.5, 1.0,
		// left-right
		1.0, 0.0,
		// left-left
		0.0, 0.0,
	};

	float pyramid_normals[] =
	{
		// front
		0.0f, 0.447214f, -0.894427f,
		0.0f, 0.447214f, -0.894427f,
		0.0f, 0.447214f, -0.894427f,

		// right
		0.894427f, 0.447214f, 0.0f,
		0.894427f, 0.447214f, 0.0f,
		0.894427f, 0.447214f, 0.0f,

		// back
		0.0f, 0.447214f, 0.894427f,
		0.0f, 0.447214f, 0.894427f,
		0.0f, 0.447214f, 0.894427f,

		// left
		-0.894427f, 0.447214f, 0.0f,
		-0.894427f, 0.447214f, 0.0f,
		-0.894427f, 0.447214f, 0.0f
	};
