#Course initialization script
#sets basic course paramaters before the lighting is set.

tux_goto_data_dir

tux_eval courses/common/tree_polyhedron.tcl

tux_load_texture trees courses/textures/models/trees.png 0
tux_bind_texture trees trees

tux_tree_props -name tree3 -diameter 1.4 -height 1.0 \
      -texture trees -index 2 -colour {0 255 48} -polyhedron $tree_poly \
      -size_varies 0.5 

tux_tree_props -name tree1 -diameter 1.4 -height 2.5 \
      -texture trees -index 1 -colour {255 255 255} -polyhedron $tree_poly \
      -size_varies 0.5 

tux_tree_props -name tree2 -diameter 1.4 -height 2.5 \
      -texture trees -index 0 -colour {255 96 0} -polyhedron $tree_poly \
      -size_varies 0.5 

tux_load_texture items courses/textures/items/items.png 0
tux_bind_texture items items

tux_item_spec -name herring -diameter 1.0 -height 1.0 \
      -atlaspos 1 -colour {28 185 204} -above_ground 0.2

tux_item_spec -name flag -diameter 1.0 -height 1.0 \
      -atlaspos 2 -colour {194 40 40} -nocollision

tux_item_spec -name finish -diameter 9.0 -height 6.0 \
		-atlaspos 3 -colour {255 255 0} -nocollision \
                -normal {0 0 1}

tux_item_spec -name start -diameter 9.0 -height 6.0 \
		-atlaspos 0 -colour {128 128 0} -nocollision \
                -normal {0 0 1}

tux_item_spec -name float -nocollision -colour {255 128 255} -reset_point


tux_trees "$::course_dir/trees.png"


tux_ice_tex courses/textures/terrain/ice.png
tux_rock_tex courses/textures/terrain/rock.png
tux_snow_tex courses/textures/terrain/snow.png

tux_friction 0.22 0.9 0.35

#
# Introductory animation keyframe data
#
tux_eval courses/common/tux_walk.tcl

#
# Lighting
#
set conditions [tux_get_race_conditions]
if { $conditions == "sunny" } {
    tux_eval courses/common/sunny_light.tcl
} elseif { $conditions == "night" } {
    tux_eval courses/common/night_light.tcl
} elseif { $conditions == "evening" } {
    tux_eval courses/common/evening_light.tcl
} 

