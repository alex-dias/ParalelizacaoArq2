# ParalelizacaoArq2
Trabalho de laboratório de arquitetura de computadores 2 2015/1

Alexandre Dias Negretti 558214

Beatriz Leite Negrão    610534

O objetivo desse trabalho é paralelizar o algoritmo de Mandelbrot com instruções SIMD e Inline Assembly. Decidimos fazê-lo tanto com a técnica SSE e AVX, já que possuímos computadores compatíveis com ambas.

## Abordagem

Inicialmente o código foi paralelizado usando AVX, em seguida usamos o código já paralelizado para usar a técnica SSE.

Nem todos os loops eram paralelizáveis já que o mais interno dependia de certas situações para cada iteração. Dessa  forma, para os loops paralelizáveis, calculamos o valor de Cx e Cy oito a oito por iteração para a técnica AVX e quatro em quatro para o SSE. Entretanto, os registradores do AVX são de 256 bits, e para colocar 8 valores de uma só vez, precisamos alterar o tipo das variáveis do código original de Double para Float, além disso só podiamos usar um a um no loop mais interno para calcular Zx e Zy, já que esse loop não era paralelizável.

## Resultados

Para testar os novos algoritmos utilizamos dois computadores diferentes, um com o processador AMD A10-4600M (Compatível com AVX) e outro com processador i5 CPU M 480 @ 2,67GHz 2,66GHz (Não compatível com AVX).

Com o processador AMD, foram feitos 10 testes com cada um dos três algoritmos e os resultados de testes de tempo de execução são mostrados no gráfico a seguir:

![](http://i.imgur.com/FIOG0Ry.png?1)

A partir do gráfico é possível reconhecer uma significante diferença entre os códigos paralelos e o código original, mesmo com o desvio padrão. A surpresa fica para a diferença entre os códigos paralelos, mesmo sendo mais recente e calculando o dobro de valores por vez, a técnica AVX foi levemente mais lenta em média comparada com o a SSE.

Com o processador i5, foram realizados 10 testes com os algoritmos original e SSE, obtendo resultados que seguem abaixo:

![](http://i62.tinypic.com/2hyl82p.png) 

O desvio padrão foi relativamente alto, porém a paralelização SSE reduziu o tempo de execução do algoritmo de mandelbrot em média, 2 segundos, uma redução de 2%.

Quanto aos processadores diferentes, obtemos resultados no segundo processador 63% mais rápido que o primeiro, porém, se analisarmos o quanto a paralelização SSE acelerou o algoritmo original, temos uma maior porcentagem no primeiro processador, de 5,5% mais rápido no primeiro, e 2% no segundo.
A paralelização em AVX só foi possível no primeiro processador já que o segundo não possi suporte para esse tipo de instrução.
Mas em relação ao processador AMD, o algoritmo paralelizado em AVX foi 5% mais rápido em relação ao original.

As imagens geradas pelos 3 códigos são idênticas, como pode ser visto abaixo. As imagens foram redimensionadas e não são do mesmo formato da saída original, já que a imagem gerada possui mais de 700mb. Cada imagem abaixo possui 512x512 pixels:

![](http://i59.tinypic.com/2qlzf2a.png)

Ao comparar as imagens geradas, concluímos que não houve perda significante de qualidade ao mudar o tipo de variável dos códigos de Double para Float.

