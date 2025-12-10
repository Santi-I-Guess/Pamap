
# 3D Pathway Visualizer
* Yep, it's that simple
* Basically just to help map out tunnels, caves, and other semi-complex areas
  in videogames

# Input File Syntax
A given file input file has each "trail" as a series of points inside square
brackets. Consecutive points inside a given pathway will automatically be
connectd with an edge.

To mark that two points on different trails have an edge with each other,
put a set of curly braces in front with single letters that mark each
connection. Uppercase and lowercase letters are considered seperate
symbols for edges.

Each symbol is 1 uppercase or lowercase letter, allowing for 52 cross-trail
edges.

For example: \
\[  \
&emsp;(0,0,0), (1,2,0), {A}(3,3,0), (5,5,0), {B}(6,8,0) \
\] \
\[ \
&emsp;(0,3,0), (1,7,0), (2,7,0), {A,B}(3,5,0) \
\]
