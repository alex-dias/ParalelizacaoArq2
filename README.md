# ParalelizacaoArq2
Trabalho de laboratório de arquitetura de computadores 2 2015/1

Alexandre Dias Negretti 558214

Beatriz Leite Negrão    610534

O objetivo desse trabalho era paralelizar o algoritmo de Mandelbrot com instruções SIMD e Inline Assembly. Decidimos fazê-lo tanto com a técnica SSE e AVX, já que possuímos computadores compatíveis com ambas.

## Abordagem

Inicialmente o código foi paralelizado usando AVX, em seguida usamos o código já paralelizado para usar a técnica SSE.

Nem todos os loops eram paralelizáveis já que o mais interno dependia de certas situações para cada iteração. Dessa  forma, para os loops paralelizáveis, calculamos o valor de Cx e Cy oito a oito por iteração. Entretanto, só podiamos usar um a um no loop mais interno para calcular Zx e Zy.

## Resultados

[em construção]
