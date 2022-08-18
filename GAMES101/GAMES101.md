# GAMES101

## Lecture 1

- Course Topics
  - Rasterization
  - Curves and Meshes
  - Ray Tracing
  - Animation/Simulation

## Lecture 2

- Graphics' Dependencies

  - Basic mathmatics

    Linear algebra/ calculus, statistics

  - Basic phsics

  - Misc

    Signal processing/ Numerical analysis

## Lecture 3 Transformation

Modeling & Viewing

- 2D-Transformation

  - Scale Transform

    <img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-08 00.07.22.png" alt="截屏2022-07-08 00.07.22" style="zoom:50%;" />

  - Reflection Matrix(y-axis)

    <img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-08 00.09.16.png" alt="截屏2022-07-08 00.09.16" style="zoom:50%;" />

  - Shear Matrix(x-axis剪切)

    <img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-08 00.11.37.png" alt="截屏2022-07-08 00.11.37" style="zoom:50%;" />

  - Rotate(about the origin (0, 0), CCW/逆时针 by default)

    <img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-08 00.14.05.png" alt="截屏2022-07-08 00.14.05" style="zoom:50%;" />

  - Linear Transforms = Matrices

    <img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-08 00.17.53.png" alt="截屏2022-07-08 00.17.53" style="zoom:50%;" />

    **平移操作不可看作线性变换**

    <img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-08 00.20.41.png" alt="截屏2022-07-08 00.20.41" style="zoom:50%;" />

- Homogenous Coordinates

  To solve the simple move operation -->

  **Add a third coordinate**

  *2D point* = $(x, y, 1)^T$

  *2D vector* = $(x, y, 0)^T$

  Matrix representation of translations

​		<img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-08 00.25.02.png" alt="截屏2022-07-08 00.25.02" style="zoom:50%;" />

​		**Point + Point = ?**

<img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-08 00.26.49.png" alt="截屏2022-07-08 00.26.49" style="zoom:50%;" />

​		**Affine Transformations**	

​		<img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-08 00.28.33.png" alt="截屏2022-07-08 00.28.33" style="zoom:50%;" />

- Inverse Transform

  $M^{-1}$ is the inverse of transform M in both a matrix and geometric sense

​		The order of the transformations is important(矩阵乘法不满足交换律) 

## Lecture 4 Transformation Cont.

- 3D Transformations

  Rotation around x-, y-, or z-axis

  <img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-08 00.49.09.png" alt="截屏2022-07-08 00.49.09" style="zoom:50%;" />

  <img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-08 00.50.27.png" alt="截屏2022-07-08 00.50.27" style="zoom:50%;" />

- Viewing Transformations

  How to take a photo(MVP)
  
  a. Model transformation
  
  b. View transformation
  
  c. Projection transformation
  
  - View/Camera transformation
  
    Define the camera first
  
    - Position $\vec{e}$
    - Look-at/ gaze direction $\hat{g}$
    - Up direction $\hat{t}$
  
    Key observation
  
    - **transform the camera to up at Y, look at Z-**
  
    - transform the objects along with the camera
  
    - How to transform the camera by $M_{view}$
  
      <img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-08 12.45.14.png" alt="截屏2022-07-08 12.45.14" style="zoom:50%;" />
  
  - Projection transformation
    - Orthographic projection
    
      Map a cuboid $[l, r]\times[b,t]\times[f,n]$ to the canonical cube $[-1,1]^3$
    
      <img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-08 12.59.42.png" alt="截屏2022-07-08 12.59.42" style="zoom:50%;" />
    
    - Perspective projection
    
      - Most common in CG, art, visual system
      - Further objects are smaller
      - Parallel lines not parallel; converge to single point
      - $M_{persp} = M_{ortho}M_{persp\to ortho}$

## Lecture 5 Rasterization(Triangles)