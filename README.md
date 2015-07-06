# ParalelizacaoArq2
Trabalho de laboratório de arquitetura de computadores 2 2015/1

Alexandre Dias Negretti 558214

Beatriz Leite Negrão    610534

O objetivo desse trabalho era paralelizar o algoritmo de Mandelbrot com instruções SIMD e Inline Assembly. Decidimos fazê-lo tanto com a técnica SSE e AVX, já que possuímos computadores compatíveis com ambas.

## Abordagem

Inicialmente o código foi paralelizado usando AVX, em seguida usamos o código já paralelizado para usar a técnica SSE.

Nem todos os loops eram paralelizáveis já que o mais interno dependia de certas situações para cada iteração. Dessa  forma, para os loops paralelizáveis, calculamos o valor de Cx e Cy oito a oito por iteração para a técnica AVX e quatro em quatro para o SSE. Entretanto, os registradores do AVX são de 256 bits, e para colocar 8 valores de uma só vez, precisamos alterar o tipo das variáveis do código original de Double para Float, além disso só podiamos usar um a um no loop mais interno para calcular Zx e Zy, já que esse loop não era paralelizável.

## Resultados

Para testar os novos algoritmos utilizamos dois computadores diferentes, um com o processador AMD A10-4600M (Compatível com AVX).
Com o processador AMD, foram feitos 10 testes com cada um dos três algoritmos e os resultados de testes de tempo de execução são mostrados no gráfico a seguir:

![](http://i.imgur.com/FIOG0Ry.png?1)

A partir do gráfico é possível reconhecer uma significante diferença entre os códigos paralelos e o código original, mesmo com o desvio padrão. A surpresa fica para a diferença entre os códigos paralelos, mesmo sendo mais recente e calculando o dobro de valores por vez, a técnica AVX foi levemente mais lenta em média comparada com o a SSE.

As imagens geradas pelos 3 códigos são idênticas, desse modo a imagem seguinte é do código AVX. A imagem foi redimensionada e não é do mesmo formato do saída original, já que a imagem gerada possui mais de 700mb:

![](http://i.imgur.com/pSUKqVH.png?1)

Ao comparar as imagens geradas, concluímos que não houve perda significante de qualidade ao mudar o tipo de variável dos códigos de Double para Float.

[em construção]
