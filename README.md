# Raytracer
A raytracer built during the CS410 (Graphics) course at CSU

This repo is the result of a term project built in CS410 at CSU. The graphics course has students develop a fully functional ray tracer over the course of 15 weeks, doing 5 increments, each over 3 weeks. The current features are as follows:

- Read in a customized driver file to describe a scene
- Read in models in the Wavefront Object file format
- Read in materials in the Material Template Library format
- Render spheres/triangular models based on material description
- Apply ambient light, Lambertian lighting, and specular highlights to objects
- Render reflections, with various coefficients of attenuation
- Render shadows

# How to use
This Raytracer makes use of the C++ linear algebra library, [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page#Download). To use this raytracer, you must download Eigen and provide it to the raytracer at compile time. Although it may work with other versions, this program was developed with Eigen 3.3.7. The repo contains a Makefile with an `EIGEN_PATH` variable, which you should set to the path of your Eigen directory. Alternatively, the default path in the Makefile is `./Eigen`, so you may also make a symbolic link to Eigen in the same directory as the Makefile.

The executable can be run as shown:
<pre>./raytracer (inputDriverFile) (outputPPMfile)</pre>
    
The following instructions assume you have some knowledge of graphics scenes and models. There is an example driver file in the repo that you may use if you are not. This driver file should be run in the same directory as

In the driver file, comments may be indicated with a #. All positions in the file are in world coordinates, and all colors are on a continuous scale from 0-1 (although exceeding these bounds is allowed, and may produce some very fun effects). The format of the driver file is as follows (order of elements is not important):
<pre>
# Position of the camera
eye x y z
# The point at which the camera looks
look x y z
# The up vector
up x y z
# The distance of the near clipping plane from the camera. The magnitude is important,
# but the sign isn't, so you will always look towards the look-at point.
d m
# The edges of the near near clipping plane
bounds left right up down
# How many times to reflect light. recursionlevel 0 indicates no reflection.
recursionlevel n
# The color of the uniform ambient light applied to all objects
ambient r g b

# All previous elements are unique, and are overridden if specified multiple times. The rest are
# cumulative, and will define a new element in the scene.

# A light with position x,y,z and color r,g,b. If atPoint is 1, the light is a point light source at the
# given position. If it is 0, the light is infinitely far away in the direction given by x,y,z, such that
# it is relatively in the same position for all objects in the scene.
light x y z atPoint r g b

# A true sphere to be rendered in the scene with center x,y,z, and with 12 color coefficients:
# Ka - Ambient coefficient | Kd - Lambertian/diffuse coefficient 
# Ks - Specular Coefficient | Kr - Reflective coefficient
sphere x y z radius KaR KaG KaB KdR KdG KdB KsR KsG KsB KrR KrG KrB

# A model to be loaded in, transformed, and rendered in the scene.
# The transformations applied are as follows (in this order):
# An axis-angle rotation of theta degrees about (wx, wy, wz)
# A uniform scaling given by scale
# A translation by (tx, ty, tz) units
model wx wy wz theta scale tx ty tz model.obj
</pre>

The final parameter for any model line is the path (relative to the runtime directory of the program, or for ease of use, an absolute path to the file) of a Wavefront Object model file. The only lines which impact the render are vertices, faces, matlib, and usemtl. Other lines are not featured in the ray tracer, and ignored, including lines which are completely invalid. Similarly to the model, any mtllib files used should be relative to the runtime directory or absolute.

The only lines in a .mtl file which impact the render are newmtl, Ka, Kd, Ks, Ns, and illum. However, illum only has binary effect- if it is 3, the material will be reflective, otherwise, it will behave as a 2, rendering color and specular highlights.

# Final Warning
This program was not designed with fault tolerance in mind. Although you shouldn't be able to break it too terribly, it doesn't react to invalid .obj or .mtl files. If you provide invalid parameters/lines in a driver file, it should react tolerably, but it will ignore extra parameters. 
