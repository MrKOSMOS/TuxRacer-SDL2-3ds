tux_course_light 0 -on -position { 1 1 0 0 } \
	-diffuse {0.7 0.7 0.7 1.0} \
	-ambient {0.8 0.55 0.5 1.0} 
  
tux_course_light 1 -on -position { 1 1 2 0 } \
	-specular { 0.9 0.55 0.5 1 } 

tux_particle_color { 0.9 0.7 0.35 1.0 }

#
# Environmental sphere map
    
tux_load_texture alpine1-sphere courses/textures/conditions/eveningenv.png 0
tux_bind_texture terrain_envmap alpine1-sphere

tux_particle_colour { 0.51 0.30 0.30 1.0 }

tux_load_texture alpine1-front courses/textures/conditions/eveningfront.png 0
tux_load_texture alpine1-right courses/textures/conditions/eveningright.png 0
tux_load_texture alpine1-left courses/textures/conditions/eveningleft.png 0
tux_load_texture alpine1-back courses/textures/conditions/eveningback.png 0
tux_load_texture alpine1-top courses/textures/conditions/eveningtop.png 0
tux_load_texture alpine1-bottom courses/textures/conditions/eveningbottom.png 0

tux_bind_texture sky_front alpine1-front
tux_bind_texture sky_right alpine1-right
tux_bind_texture sky_left alpine1-left
tux_bind_texture sky_back alpine1-back
tux_bind_texture sky_top alpine1-top
tux_bind_texture sky_bottom alpine1-bottom