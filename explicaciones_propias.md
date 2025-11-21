** MulMod (multiplicación rusa sin exceder el tamaños del tipo) **
```math
        O(n * log^4 n) > O(n * log^3 n)
	O(n * log^4 n) / O(n * log^3 n) = O( log(n) )
```
De aquí que el método de multiplicación rusa es $O(log(n))$ veces más lento que usando registros de doble tamaño.
Supongamos que el número queremos saber la primalidad de $ n = 2^64 $, usando `mulmod` tardará $64\cdot log(2)$ veces más que con `__int128`