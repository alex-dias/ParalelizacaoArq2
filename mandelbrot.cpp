/*
Modified from: http://rosettacode.org/wiki/Mandelbrot_set#PPM_non_interactive

c program:
--------------------------------
1. draws Mandelbrot set for Fc(z)=z*z +c
using Mandelbrot algorithm ( boolean escape time )
-------------------------------
2. technique of creating ppm file is  based on the code of Claudio Rocchini
http://en.wikipedia.org/wiki/Image:Color_complex_plot.jpg
create 24 bit color graphic file ,  portable pixmap file = PPM
see http://en.wikipedia.org/wiki/Portable_pixmap
to see the file use external application ( graphic viewer)
*/

#define _CRT_SECURE_NO_WARNINGS
#include <immintrin.h>
#include <stdio.h>
#include <math.h>

int main()
{
	/* screen ( integer) coordinate */
	const int iXmax = 16384;
	const int iYmax = 16384;
	/* world ( double) coordinate = parameter plane*/
	float CxPara[8], CyPara[8], cpu_time_used;
	const float CxMin = -2.5;
	const float CxMax = 1.5;
	const float CyMin = -2.0;
	const float CyMax = 2.0;
	/* */
	float PixelWidth = (CxMax - CxMin) / iXmax;
	float PixelHeight = (CyMax - CyMin) / iYmax;
	/* color component ( R or G or B) is coded from 0 to 255 */
	/* it is 24 bit color RGB file */
	const int MaxColorComponentValue = 255;
	FILE * fp;
	char *filename = "mandelbrot.ppm";
	static unsigned char color[3];
	/* Z=Zx+Zy*i  ;   Z0 = 0 */
	float Zx, Zy;
	float Zx2, Zy2; /* Zx2=Zx*Zx;  Zy2=Zy*Zy  */

	int Iteration;
	const int IterationMax = 256;
	/* bail-out value , radius of circle ;  */
	const float EscapeRadius = 2;
	float ER2 = EscapeRadius*EscapeRadius;
	/*create new file,give it a name and open it in binary mode  */
	fp = fopen(filename, "wb"); /* b -  binary mode */
	/*write ASCII header to the file*/
	fprintf(fp, "P6\n %d\n %d\n %d\n", iXmax, iYmax, MaxColorComponentValue);
	/* compute and write image data bytes to the file*/

	__m256 iY = _mm256_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);
	__m256 iX = _mm256_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);
	__m256 PixelWidth256 = _mm256_set1_ps(PixelWidth);
	__m256 CxMin256 = _mm256_set1_ps(CxMin);
	__m256 PixelHeight256 = _mm256_set1_ps(PixelHeight);
	__m256 CyMin256 = _mm256_set1_ps(CyMin);
	__m256 soma = _mm256_set1_ps(8.0f);

	for (int i = 0; i < iYmax / 8; i++)
	{
		__asm {
			vmovaps ymm0, [iY]
			vmulps ymm1, ymm0, [PixelHeight256]
			vaddps ymm1, ymm1, [CyMin256]
			vmovups [CyPara], ymm1
			vaddps ymm0, ymm0, [soma]
			vmovups [iY], ymm0
		}

		for (int j = 0; j < 8; j++)
		{

			__m256 iX = _mm256_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);

			if (fabs(CyPara[j]) < PixelHeight / 2) CyPara[j] = 0.0; /* Main antenna */
			for (int k = 0; k < iXmax / 8; k++)
			{
				__asm {
					vmovaps ymm0, [iX]
					vmulps ymm1, ymm0, [PixelWidth256]
					vaddps ymm1, ymm1, [CxMin256]
					vmovups [CxPara], ymm1
					vaddps ymm0, ymm0, [soma]
					vmovups [iX], ymm0
				}
				/* initial value of orbit = critical point Z= 0 */

				/* */
				for (int l = 0; l < 8; l++)
				{
					Zx = 0.0;
					Zy = 0.0;
					Zx2 = 0.0;
					Zy2 = 0.0;
					for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2); Iteration++)
					{
						Zy = 2 * Zx*Zy + CyPara[j];
						Zx = Zx2 - Zy2 + CxPara[l];
						Zx2 = Zx*Zx;
						Zy2 = Zy*Zy;
					};
					/* compute  pixel color (24 bit = 3 bytes) */
					if (Iteration == IterationMax)
					{ /*  interior of Mandelbrot set = black */
						color[0] = 0;
						color[1] = 0;
						color[2] = 0;
					}
					else
					{ /* exterior of Mandelbrot set = white */

						color[0] = ((IterationMax - Iteration) % 8) * 63;  /* Red */
						color[1] = ((IterationMax - Iteration) % 4) * 127;  /* Green */
						color[2] = ((IterationMax - Iteration) % 2) * 255;  /* Blue */
					};
					/*write color to the file*/
					fwrite(color, 1, 3, fp);
				}
			}
		}
	}
	fclose(fp);
	return 0;
}