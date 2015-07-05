#define _CRT_SECURE_NO_WARNINGS								// Exigido pelo VS para usar a operação fopen
#include <time.h>											// Biblioteca para medir o tempo de execução
#include <immintrin.h>										// Biblioteca para o uso de instruções intrínsecas 
#include <stdio.h>
#include <xmmintrin.h>
#include <math.h>

int main()
{
	clock_t start, end;	
	start = clock();										// Início da contagem de tempo

	const int iXmax = 16384;								// Tamanho da imagem
	const int iYmax = 16384;
	
	float cpu_time_used;

	// A variavel Cx teve seu nome trocado para evitar conflito com o registrador cx do Assembly, Cy tambem foi trocado para manter o padrao
	float CxPara[4], CyPara[4];			
	const float CxMin = -2.5;								// O tipo das variaveis foram mudadas de Double para Float para possibilitar o
	const float CxMax = 1.5;								// uso de 8 variaveis por registrador YMM do AVX, e mantidas para comparação com SSE
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
	
	// Declaração intrínseca com os 4 valores iniciais de iY
	__m128 iY = _mm_set_ps(0.0f, 1.0f, 2.0f, 3.0f);

	// Todas as variaves utilizadas para calcular CyPara e CxPara devem ser vetores de Float possibilitando a paralelização
	__m128 PixelHeight256 = _mm_set1_ps(PixelHeight);
	__m128 CyMin256 = _mm_set1_ps(CyMin);
	__m128 PixelWidth256 = _mm_set1_ps(PixelWidth);
	__m128 CxMin256 = _mm_set1_ps(CxMin);

	// Essa vetor incrementa todos os valores de iX e iY para prepará-lo para a próxima iteração
	__m128 soma = _mm_set1_ps(4.0f);

	for (int i = 0; i < iYmax / 4; i++)				// iYmax/4 pois são calculados 4 valores de uma só vez
	{
		// Inline Assembly
		__asm {
			movaps xmm0, [iY]
			mulps xmm0, [PixelHeight256]
			addps xmm0, [CyMin256]
			movups [CyPara], xmm0
			movaps xmm0, [iY]
			addps xmm0, [soma]				// Novos valores para a próxima iteração
			movups [iY], xmm0
		}

		for (int j = 0; j < 4; j++)					// Esse loop é necessário para passar cada valor de CyPara calculado
		{
			// Declaração intrínseca com os 4 valores iniciais de iX
			// Perceba que o valor de iX deve ser reiniciado para cada valor de iY, por isso a declaração de iX está dentro desse "for"
			__m128 iX = _mm_set_ps(0.0f, 1.0f, 2.0f, 3.0f);

			if (fabs(CyPara[j]) < PixelHeight / 2) CyPara[j] = 0.0;

			for (int k = 0; k < iXmax / 4; k++)
			{
				// Inline Assembly
				__asm {
					movaps xmm0, [iX]
					mulps xmm0, [PixelWidth256]
					addps xmm0, [CxMin256]
					movups [CxPara], xmm0
					movaps xmm0, [iX]
					addps xmm0, [soma]				// Novos valores para a próxima iteração
					movups [iX], xmm0
				}

				for (int l = 0; l < 4; l++)
				{
					Zx = 0.0;
					Zy = 0.0;
					Zx2 = 0.0;							// O código original executava uma operação desnecessária nesse trecho
					Zy2 = 0.0;

					for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2); Iteration++)
					{
						// CyPara e CxPara são calculados 4 a 4, mas Zy e Zx precisam ser calculados 1 a 1 pois esse loop não é paralelizável
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