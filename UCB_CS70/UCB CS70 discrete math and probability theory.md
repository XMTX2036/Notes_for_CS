# UCB CS70: discrete math and probability theory

> By DYY xmtxleo@gmail.com

## Lecture1 

### Outline

**The language of proofs**

- Propositions
- Propositional Forms
- Implication
- Truth Tables
- Quantifiers
- De Morgan's Laws

### Propositions: Statements that are true or false

Value of a proposition is *True* or *False*

### Propositional Forms

- Conjuction(and): $P\and Q$

- Disjunction(or): $P\or Q$

- Negation(not): $\neg P$

### Truth Tables

| P    | Q    | $\neg(P\or Q)$ | $\neg P\and\neg Q$ |
| ---- | ---- | -------------- | ------------------ |
| T    | T    | F              | F                  |
| T    | F    | F              | F                  |
| F    | T    | F              | F                  |
| F    | F    | T              | T                  |

===> $\neg(P\or Q) \equiv \neg P\and\neg Q$

### Implication

$P\Rightarrow Q$ interpreted as `if P, then Q`

True statements: $P$, $P\Rightarrow Q$

Conclude: Q is true

**Non-Consequences/consequences of Implication**

$P\Rightarrow Q$

- **Only** False if **P is True and Q is False**

- ==False implies nothing==

  P False ===> Q can be true/false 

- ==Anything implies true==

  P can be true/false if Q is true

`e.g.`

**P true ===> Q true** ✅

**Q true ===> P true** ❌

**Implication and English**

$P\Rightarrow Q$

- If P, then Q
- Q if P
- P only if Q
- P is sufficient for Q
- Q is necessary for P

**True Table: Implication**

| P     | Q     | $P\Rightarrow Q$/($\neg P\or Q$) |
| ----- | ----- | -------------------------------- |
| T     | T     | T                                |
| ==T== | ==F== | ==F==                            |
| F     | T     | T                                |
| F     | F     | T                                |

$P\Rightarrow Q\equiv\neg P\or Q$

### Contrapositive, Converse

**Contrapositive** of $P\Rightarrow Q$ is $\neg Q\Rightarrow \neg P$

**Converse** of $P\Rightarrow Q$ is $Q\Rightarrow P$

**Defn. ** If $P\Rightarrow Q$ and $Q\Rightarrow P$ is $P$ if and only if Q/$P\iff Q$

### Variables

**Predicate** e.g. $Q(x)=$"x is even"

### Quantifiers

**There exists quantifier**:

$(\exist x\in S)(P(x))$ ===> There exists an x in S where P(x) is true.

**For all quantifier:**

$(\forall x\in S)(P(x))$ ===> For all x in S, P(x) is true.

**Universes**

- $\mathbb{N} = \{0,1, ...\}$

- $\mathbb{Z}$
- $\mathbb{Z}^+$
- $\mathbb{R}$
- Any set S

**De Morgan's Law**

$\neg(\exist x\in S)(P(x))\iff(\forall x\in S)(\neg P(x))$

$\neg(\forall x\in S)(P(x))\iff(\exist x\in S)(\neg P(x))$