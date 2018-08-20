/*
* @author: Praveen Suryawanshi
* @mailto: suryawanshi.praveen007@gmail.com
* please dont remove @author or modify it.
* any changes or bug fixes are done please annotate the locations where bug fixed.
*/

__kernel void vertices_reduction_function(__global float *pfVertices, int vertices_cnt,__global float *vbo )
{
	int stride_i = get_global_id(0);
	if (stride_i < vertices_cnt)
	{
		vbo[stride_i] = pfVertices[stride_i];
	}
}

/*__kernel void vertices_reduction_function(
	__global float *pfVertices,

	int vertices_cnt,

	__global unsigned int *_pnVerticesIndex,

	int _nVerticesIndexCnt,

	__global int *verticesDuplcation,

	__global float *_pfUniqueVertices
)
{
	const int TOKEN = 123;

	__global  static int isUniqueVerticesElement = 1;

	__global static int vertices_duplcation_cnt = 0;

	__global static int _nUniqueVerticesIndexCnt = 0;


	int stride_i = get_global_id(0);

	if (stride_i < (int)vertices_cnt / 3)
	{

		int i_1 = (stride_i * 3) + 0;
		int i_2 = (stride_i * 3) + 1;
		int i_3 = (stride_i * 3) + 2;


		for (int stride_j = stride_i + 1; stride_j < vertices_cnt / 3; stride_j++)
		{

			int j_1 = (stride_j * 3) + 0;
			int j_2 = (stride_j * 3) + 1;
			int j_3 = (stride_j * 3) + 2;

			//compare vertices for duplication
			if (stride_j < vertices_cnt / 3)
			{
				isUniqueVerticesElement = 1;

				if (pfVertices[i_1] == TOKEN && pfVertices[i_2] == TOKEN && pfVertices[i_3] == TOKEN)
				{
					isUniqueVerticesElement = 0;
				}

				if (isUniqueVerticesElement != 0 && pfVertices[i_1] == pfVertices[j_1] && pfVertices[i_2] == pfVertices[j_2] && pfVertices[i_3] == pfVertices[j_3])
				{
					verticesDuplcation[vertices_duplcation_cnt++] = stride_i;
					verticesDuplcation[vertices_duplcation_cnt++] = stride_j;

					pfVertices[j_1] = TOKEN;
					pfVertices[j_2] = TOKEN;
					pfVertices[j_3] = TOKEN;

					isUniqueVerticesElement = 1;

					if (stride_j != _nUniqueVerticesIndexCnt)
					{
						for (size_t i = 0; i < _nVerticesIndexCnt; i++)
						{
							if (_pnVerticesIndex[i] == stride_j)
							{
								_pnVerticesIndex[i] = _nUniqueVerticesIndexCnt / 3;
							}
						}

					}
					
				}
			}
			//end

		} //end of for


			//copy unique vertices in new array
			if (isUniqueVerticesElement == 1)
			{
				//update the vertices index in face table
				if (stride_i != _nUniqueVerticesIndexCnt)
				{
					for (size_t i = 0; i < _nVerticesIndexCnt; i++)
					{
						if (_pnVerticesIndex[i] == stride_i)
						{
							_pnVerticesIndex[i] = _nUniqueVerticesIndexCnt / 3;
						}
					}

				}//end

				_pfUniqueVertices[_nUniqueVerticesIndexCnt++] = pfVertices[i_1];
				_pfUniqueVertices[_nUniqueVerticesIndexCnt++] = pfVertices[i_2];
				_pfUniqueVertices[_nUniqueVerticesIndexCnt++] = pfVertices[i_3];

				isUniqueVerticesElement = 0;

			}
			//end		
		
	}//end of if
	

}*/