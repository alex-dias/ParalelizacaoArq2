#define _CRT_SECURE_NO_WARNINGS								// Exigido pelo VS para usar a operação fopen
#include <time.h>											// Biblioteca para medir o tempo de execução
#include <immintrin.h>										// Biblioteca para o uso de instruções intrínsecas do AVX
#include <stdio.h>
#include <math.h>

int main()
{
	clock_t start, end;	
	start = clock();										// Início da contagem de tempo

	const int iXmax = 16384;								// Tamanho da imagem
	const int iYmax = 16384;
	
	float cpu_time_used;

	// A variavel Cx teve seu nome trocado para evitar conflito com o registrador cx do Assembly, Cy tambem foi trocado para manter o padrao
	float CxPara[8], CyPara[8];			
	const float CxMin = -2.5;								// O tipo das variaveis foram mudadas de Double para Float para possibilitar o
	const float CxMax = 1.5;								// uso de 8 variaveis por registrador YMM do AVX
	const float CyMin = -2.0;
	const float CyMax = 2.0;
	
	float PixelWidth = (CxMax - CxMin) / iXmax;
	float PixelHeight = (CyMax - CyMin) / iYmax;
	
	const int MaxColorComponentValue = 255;
	FILE * fp;
	char *filename = "mandelbrot.ppm";
	static unsigned char color[3];
	
	float Zx, Zy;
	float Zx2, Zy2; 

	int Iteration;
	const int IterationMax = 256;
	
	const float EscapeRadius = 2;
	float ER2 = EscapeRadius*EscapeRadius;
	
	fp = fopen(filename, "wb"); 
	
	fprintf(fp, "P6\n %d\n %d\n %d\n", iXmax, iYmax, MaxColorComponentValue);
	
	// Declaração intrínseca com os 8 valores iniciais de iY
	__m256 iY = _mm256_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);

	// Todas as variaves utilizadas para calcular CyPara e CxPara devem ser vetores de Float possibilitando a paralelização
	__m256 PixelHeight256 = _mm256_set1_ps(PixelHeight);
	__m256 CyMin256 = _mm256_set1_ps(CyMin);
	__m256 PixelWidth256 = _mm256_set1_ps(PixelWidth);
	__m256 CxMin256 = _mm256_set1_ps(CxMin);

	// Essa vetor incrementa todos os valores de iX e iY para prepará-lo para a próxima iteração
	__m256 soma = _mm256_set1_ps(8.0f);

	for (int i = 0; i < iYmax / 8; i++)				// iYmax/8 pois são calculados 8 valores de uma só vez
	{
		// Inline Assembly
		__asm {
			vmovaps ymm0, [iY]
			vmulps ymm1, ymm0, [PixelHeight256]
			vaddps ymm1, ymm1, [CyMin256]
			vmovups [CyPara], ymm1
			vaddps ymm0, ymm0, [soma]				// Novos valores para a próxima iteração
			vmovups [iY], ymm0
		}

		for (int j = 0; j < 8; j++)					// Esse loop é necessário para passar cada valor de CyPara calculado
		{
			// Declaração intrínseca com os 8 valores iniciais de iX
			// Perceba que o valor de iX deve ser reiniciado para cada valor de iY, por isso a declaração de iX está dentro desse "for"
			__m256 iX = _mm256_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);

			if (fabs(CyPara[j]) < PixelHeight / 2) CyPara[j] = 0.0;

			for (int k = 0; k < iXmax / 8; k++)
			{
				// Inline Assembly
				__asm {
					vmovaps ymm0, [iX]
					vmulps ymm1, ymm0, [PixelWidth256]
					vaddps ymm1, ymm1, [CxMin256]
					vmovups [CxPara], ymm1
					vaddps ymm0, ymm0, [soma]		// Novos valores para a próxima iteração
					vmovups [iX], ymm0
				}

				for (int l = 0; l < 8; l++)
				{
					Zx = 0.0;
					Zy = 0.0;
					Zx2 = 0.0;							// O código original executava uma operação desnecessária nesse trecho
					Zy2 = 0.0;

					for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2); Iteration++)
					{
						// CyPara e CxPara são calculados 8 a 8, mas Zy e Zx precisam ser calculados 1 a 1 pois esse loop não é paralelizável
						Zy = 2 * Zx*Zy + CyPara[j];		
						Zx = Zx2 - Zy2 + CxPara[l];
						Zx2 = Zx*Zx;
						Zy2 = Zy*Zy;
					};
					
					if (Iteration == IterationMax)
					{ 
						color[0] = 0;
						color[1] = 0;
						color[2] = 0;
					}

					else
					{
						color[0] = ((IterationMax - Iteration) % 8) * 63;   /* Red */
						color[1] = ((IterationMax - Iteration) % 4) * 127;  /* Green */
						color[2] = ((IterationMax - Iteration) % 2) * 255;  /* Blue */
					};
					
					fwrite(color, 1, 3, fp);
				}
			}
		}
	}

	fclose(fp);
	end = clock();								// fim da contagem de tempo
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("time = %f seconds\n", cpu_time_used);
	return 0;

}