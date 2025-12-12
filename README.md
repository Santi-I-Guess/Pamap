# 3D Pathway Visualizer
* Yep, it's that simple
* Basically just to help map out tunnels, caves, and other semi-complex areas
  in videogames

# Building
Running should be as simple as running `make`. The depend rule in the Makefile 
assumes you have GNU `sed` on your system, available at
https://www.gnu.org/software/sed/. The  graphics interface is ran with raylib,
which is available at https://www.raylib.com/

# Input File Syntax
The input file has a syntax such that specifying a connection should be as
easy and unprone to error as possible.

A given file input file has each "trail" as a series of points inside square
brackets. Consecutive points inside a given pathway will automatically be
connectd with an edge.

To mark that two points on different trails have an edge with each other,
put a set of curly braces in front with single letters that mark each
connection. Uppercase and lowercase letters are considered seperate
symbols for edges. Each symbol is 1 uppercase or lowercase letter, allowing
for 52 cross-trail edges.

For example: \
\[  \
&emsp;(0,0,0), (1,2,0), {A}(3,3,0), (5,5,0), {B}(6,8,0) \
\] \
\[ \
&emsp;(0,3,0), (1,7,0), (2,7,0), {A,B}(3,5,0) \
\]

Single line comments can be marked with "//". This needs at least one space
between tokens, if it's on the same line as other tokens

## Macros
Some macros are provided for customization and removing redundancy. Every
macro is placed either inside or outside a trail, and it's argument is read
to the newline. Subsequent macro calls in the same context simply overwrite
each other, if applicable. %d refers to an integer.
- \#start_pos %d,%d,%d: specifiy the position to put the camera at the
    beginning of the program / the next file reload. May be placed outside
    or inside trail backets.
- \#teleport_pos %d,%d,%d: specify the position to teleport the camera when
    V is pressed. May be placed outside or inside trail backets.
- \#node_color xxxxxx: specify the node color all nodes in a given trail.
    must be used inside a trail, must have 6 digit hex color without prefix
- \#edge_color xxxxxx: specify the edge color all edges in a given trail.
    must be used inside a trail, must have 6 digit hex color without prefix
