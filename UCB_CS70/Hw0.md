# Hw0

## 5. Propositional Practice

(a) $(\exist x_0\in\mathbb{R})(x_0^2=0)$ ==Fail to guarantee the only x0 to satisfy the equation==

> **Solution**: $(\exist x_0\in\mathbb{R})((x_0^2=0)\and((\forall x_1\in\mathbb{R}) (x_1^2=0)\iff(x_0=x_1)))$

(b)$(\forall x,y\in\mathbb{Q})((x\neq y)\Rightarrow((x\lt z\lt y)\or(y\lt z\lt x)))$

(c) $(\forall x\in\mathbb{Z})(x^2>4\Rightarrow((x>2)\or (x<-2)))$

(d) All real numbers are complex numbers.

(e) There isn't any solution to the equation $x^2-y^2=10$

(f) 哥德巴赫猜想

## 6. Implication

(a) True, 

(b) False. Counterexample: Q(x, y) = "x+y = 0". The left one is **true**, while the right one is false.

(c) True, 

(d) False. Counterexample: Q(x, y) = "xy = 0". The left one is true, while the right one is false.

## 7. Equivalences with Quantifiers

## 8. Preserving Set Operations

(a) 

- $\forall f(x)\in A\cap B$ ===> $x\in f^{-1}(A\cap B)$ & $(f(x)\in A) \and (f(x)\in B)$ 

  ===> $(x\in f^{-1}(A))\and(x\in f^{-1}(B))$ 

  ===> $x\in f^{-1}(A)\cap f^{-1}(B)$

  ===> $f^{-1}(A\cap B) \subseteq f^{-1}(A)\cap f^{-1}(B)$

- $\forall x\in f^{-1}(A)\cap f^{-1}(B)$ ===> $(x\in f^{-1}(A))\and(x\in f^{-1}(B))$

  ===> $(f(x)\in A) \and (f(x)\in B)$

  ===> $f(x) \in A\cap B$

  ===> $f^{-1}(A\cap B) \supseteq f^{-1}(A)\cap f^{-1}(B)$

====>  $f^{-1}(A\cap B) = f^{-1}(A)\cap f^{-1}(B)$

(b)

- $\forall f(x)\in A\setminus B$ ===> $(f(x)\in A)\and(f(x)\notin B)$

  ===> $(x\in f^{-1}(A))\and(x\notin f^{-1}(B))$ 

  ===> $x\in f^{-1}(A)\setminus f^{-1}(B)$

  ===> $f^{-1}(A\setminus B) \subseteq f^{-1}(A)\setminus f^{-1}(B)$

- $\forall x\in f^{-1}(A)\setminus f^{-1}(B)$ ===> $(x\in f^{-1}(A))\and(x\notin f^{-1}(B))$

  ===> $(f(x)\in A) \and (f(x)\notin B)$

  ===> $f(x) \in A\setminus B$

  ===> $f^{-1}(A\setminus B) \supseteq f^{-1}(A)\setminus f^{-1}(B)$

====>  $f^{-1}(A\setminus B) = f^{-1}(A)\setminus f^{-1}(B)$

(c)

(d)