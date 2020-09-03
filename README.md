# Computer Graphics: Scene processing and texturing 🛸 🌌

Linear algebra has wide application in the field of engineering and computer science. Here is described the graphic process of texturing objects. You will need: 

[OpenGL](https://www.opengl.org/) and GLUT Utility Toolkit

The idea is to apply the texture and altimetry of the planet Mars 🟤 (available at [here](http://planetpixelemporium.com/mars.html) ) in a sphere. For this, knowledge of linear algebra is needed, specifically transformations and geometry.

    I. Texture
    
   ![Figure](https://github.com/roscibely/Computer-graphics/blob/master/figures/f1.jpg)


    II. Altimetry
    
   ![Figure](https://github.com/roscibely/Computer-graphics/blob/master/figures/f2.png)
   
    III. Modeling
       
   ![Figure](https://github.com/roscibely/Computer-graphics/blob/master/figures/e2.png)
   
   The sphere radius can be written as a function of the circle radius formed by a section $ R $, and the distance between the plane of the section and the center of the sphere R-h,
   
   ![Figure](https://github.com/roscibely/Computer-graphics/blob/master/figures/eq1.gif)
    $$r = \sqrt{R^2 - (R- h)^2}.$$
   
  2D radius coordinates are given by,   
 
   $$x = cos(\theta)r, \quad y = \frac{h-D}{2}, \quad z = sin(\theta)r,$$
    IV. Final result 
    
   ![Figure](https://github.com/roscibely/Computer-graphics/blob/master/figures/proj4.png)

[Result](https://www.youtube.com/watch?v=VKxlAa5HHTw)
