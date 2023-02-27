# Rust Note

> THU 程序设计训练之 Rust 编程语言

## Lec1 基本语法

`cargo`

```bash
cargo new guessing-game

cargo build
cargo run 
```

### 变量绑定variable binding

使用`let`进行变量绑定

```rust
let x = 16; // 隐式类型(types)绑定
```

⚠️ 不是变量**赋值**

- 变量类型可指定

  ```rust
  let x: i16 = 16;
  ```

- 默认情况下变量不可变；如需令变量可变，使用`mut`修饰

- 绑定可以被掩盖 shadowing

  ```rust
  let x = 16;
  let y = 1;
  
  let x = "Shadowed!";
  ```

### 常量

```rust
const PI: f64 = 3.1415926;
const MAGIC: i32 = 42;
```

### 表达式

### 基本类型

- bool

- char

- 数值

  - i8, i16, i32, i64, i128, isize
  - u8, u16, u32, u64, u128, usize
  - f32, f64

- 数组arrays、切片slices、str字符串strings、元组tuple

  - 数组arrays

    类型的形式为`[T; N]`

    `N`是编译时常数compile-time constant

    ```rust
    let arr = [2; 32]; // arrays of 32 2's
    ```

  - 切片slices

    类型的形式为`&[T]`

  - 字符串

    - String

      在堆上分配空间，可以增长的字符序列

    - &str

      String的切片类型

  - 元组tuple

    ```rust
    let foo: (i32, char, f64) = (72, 'H', 5.1);
    let (x, y, z) = (72, 'H', 5.1);
    
    println!("{}", foo.0); // Output: 72
    println!("{}", foo.1); // Output: H
    println!("{}", foo.2); // Output: 5.1
    ```

  - 向量Vec\<T\>

    通过`Vec::new()` 或 `vec!`宏来创建Vec

    索引必须使用**usize类型**的值

    ```rust
    let i: i8 = 2;
    let y = v2[i as usize];
    ```

- 函数

### 类型转换cast

使用`as` ===> 一般来说，只能在可以安全转换的类型之间进行转换操作

### 引用

用 & 来取引用（和 C++ 类似）

用 * 来解引用（和 C++ 类似）

### 条件语句 & 循环语句

**条件**

**循环**

- while
- loop
- for

### 匹配语句

```rust
let x = 3;
match x {
	1 => println!("one fish"), // <- comma required
	2 => {
		println!("two fish");
		println!("two fish");
	}, // <- comma optional when using braces
	_ => println!("no fish for you"), // "otherwise" case
}
```

