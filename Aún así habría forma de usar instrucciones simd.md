# **Análisis: ¿Por qué no usamos registros SIMD (SSE/AVX) para int128\_t?**

Es muy común pensar que, dado que las CPUs modernas tienen registros de 128 bits (registros XMM) o incluso 256/512 bits (YMM/ZMM), deberíamos usarlos para representar nuestros enteros grandes. Aunque es técnicamente posible, generalmente **no es la mejor opción** para aritmética de números enteros individuales (escalares).  
Aquí explicamos las razones técnicas a nivel de arquitectura.

## **1\. Diferencia de Propósito: Vectorial vs Escalar**

### **Registros SIMD (XMM)**

* **Diseño:** Están hechos para **paralelismo**. Su objetivo es tomar un registro de 128 bits y tratarlo como:  
  * 4 números de 32 bits.  
  * 2 números de 64 bits.  
* **Operación:** Instrucciones como PADDQ (Packed Add Quadword) suman dos números de 64 bits en paralelo, pero **sin comunicación entre ellos**.  
* **Problema:** No propagan el **acarreo (carry)** automáticamente. Si la mitad baja se desborda, no suma 1 a la mitad alta automáticamente. Simular esto en SIMD es costoso.

### **Registros de Propósito General (GPR \- RAX, RDX, etc.)**

* **Diseño:** Son de 64 bits, pero la ALU (Unidad Aritmética Lógica) tiene un **Carry Flag (CF)** físico.  
* **Solución Nativa (\_\_int128):** El compilador une dos registros de 64 bits (ej: rax y rdx) y usa instrucciones encadenadas:  
  1. add rax, rbx (Suma las partes bajas).  
  2. adc rdx, rcx (**Add with Carry**: Suma las partes altas más el acarreo de la anterior).  
* **Eficiencia:** Esto ocurre en ciclos de reloj mínimos y es extremadamente rápido.

## **2\. El caso de la Multiplicación**

Para nuestra función mulmod, la multiplicación es crítica.

* **En GPR (Nativo):** La instrucción MUL r64 de x86-64 toma un número de 64 bits, lo multiplica por otro, y **automáticamente** guarda el resultado completo de 128 bits dividido en dos registros (RDX:RAX). Es hardware dedicado para exactamente lo que necesitamos.  
* **En SIMD:** No existe una instrucción única que diga "multiplica dos escalares de 128 bits y dame el resultado". Tienes que descomponer los números, multiplicar las partes cruzadas y sumar los resultados, lo cual requiere muchas más instrucciones y movimiento de datos.

## **3\. Latencia de Movimiento**

Mover datos entre los registros normales (donde viven los punteros y variables normales) y los registros SIMD tiene una latencia. Si solo vas a hacer una operación (como un mulmod), el tiempo que pierdes moviendo el dato al registro XMM y devolviéndolo puede ser mayor que el tiempo de cálculo.

## **Resumen**

| Característica | unsigned \_\_int128 (Nativo GCC/Clang) | SIMD (\_\_m128i intrínsecos) |
| :---- | :---- | :---- |
| **Implementación** | Pares de registros 64-bit (GPR) | Registros XMM 128-bit |
| **Suma** | ADD \+ ADC (Hardware Carry) | Compleja (emular carry manualmente) |
| **Multiplicación** | MUL (1 instrucción, resultado 128-bit) | Múltiples instrucciones |
| **Uso ideal** | Aritmética de alta precisión (Criptografía) | Procesamiento de arrays (Imágenes, Audio) |

**Conclusión:** La implementación que hemos detectado automáticamente (\_\_int128) ya utiliza la forma más eficiente que el hardware x64 ofrece para aritmética escalar. Usar SIMD solo valdría la pena si quisiéramos hacer mulmod a **muchos números a la vez** (ej: un array de 1000 primos).