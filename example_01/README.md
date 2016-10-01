# CS 184 Fall 2016
Instructor: James O'Brien
Claire Asselstine. SID: 23628013. Accnt: cs-184-aay
James Musk. SID: 23851414. Accnt: cs184-adm

# USAGE
1. ‘cd’ into your assignment directory, type into the Terminal the following step by step
2. mkdir build
3. cd build
4. cmake ..
5. make

# INPUT OPTIONS
When running ./as1 after making here are the following commands which can be run:
-ka r g b : Sets ambient term
-ks r g b : Sets specular term
-kd r g b : Sets diffuse term
-spu pu : Specular coefficient in the u direction
-spv pv : Specular coefficient in the v direction
-sp p : Specular coefficient in the isotropic case
-pl x y z r g b : Places a point light at x, y, z with color r, g, b
-dl x y z r g b : Places directional light in direction x, y, z with color r, g, b
-asm : implements Ashikhmin-Shirley Shading


# Keyboard features
1. 'ESC': Exit
2. 'Q': Exit
3. 'F': Full screen
4. '↓': Translate objects down
5. '↑': Translate objects up
6. '←': Translate objects left
7. '→': Translate objects right
